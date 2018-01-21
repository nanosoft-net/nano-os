/*
Copyright(c) 2017 Cedric Jimenez

This file is part of Nano-OS.

Nano-OS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nano-OS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Nano-OS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nano_os_data.h"
#include "nano_os_port.h"
#include "nano_os_scheduler.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"
#include "nano_os_trace.h"

/** \brief Remove a task from the global task list */
static bool NANO_OS_TASK_RemoveFromGlobalList(nano_os_task_t* const task);





/** \brief Create a new task */
nano_os_error_t NANO_OS_TASK_Create(nano_os_task_t* const task, const nano_os_task_init_data_t* const task_init_data)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if ((task_init_data != NULL) &&
        (task != NULL) &&
        (task_init_data->stack_origin != NULL) &&
        (task_init_data->task_func != NULL))
    {
        /* Check if task context is not already in the list */
        nano_os_task_t* current = g_nano_os.tasks;
        ret = NOS_ERR_SUCCESS;
        while ((current != NULL) && (ret == NOS_ERR_SUCCESS))
        {
            if (current == task)
            {
                ret = NOS_ERR_INVALID_OBJECT;
            }
            else
            {
                current = current->global_next;
            }
        }
        if (ret == NOS_ERR_SUCCESS)
        {
            /* 0 init of task */
            (void)MEMSET(task, 0, sizeof(nano_os_task_t));

            /* Init generic task context */
            task->priority = task_init_data->base_priority;
            task->base_priority = task_init_data->base_priority;
            task->stack_origin = task_init_data->stack_origin;
            task->stack_size = task_init_data->stack_size;

            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            /* Task name */
            task->name = task_init_data->name;
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */

            #if (NANO_OS_TASK_ID_ENABLED == 1u)
            /* Global object id */
            task->object_id = g_nano_os.next_object_id;
            g_nano_os.next_object_id++;
            #endif /* (NANO_OS_TASK_ID_ENABLED == 1u) */

            #if (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u)
            /* Initialize stack with known value to evaluate stack usage at runtime */
            (void)MEMSET(task->stack_origin, NANO_OS_TASK_STACK_USAGE_MARKER, task->stack_size * sizeof(nano_os_stack_t));
            #endif /* (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u) */

            /* Port specific init */
            ret = NANO_OS_PORT_InitTask(task, task_init_data);

            #if (NANO_OS_TASK_SLEEP_ENABLED == 1u)
            /* Init sleep object */
            if (ret == NOS_ERR_SUCCESS)
            {
                ret = NANO_OS_WAIT_OBJECT_Initialize(&task->sleep_object, WOT_TASK, QT_PRIORITY);
            }
            #endif /* (NANO_OS_TASK_SLEEP_ENABLED == 1u) */

            #if (NANO_OS_TASK_JOIN_ENABLED == 1u)
            /* Init task join object */
            if (ret == NOS_ERR_SUCCESS)
            {
                ret = NANO_OS_WAIT_OBJECT_Initialize(&task->join_object, WOT_TASK, QT_PRIORITY);
            }
            #endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */


            /* Register task to scheduler */
            if (ret == NOS_ERR_SUCCESS)
            {
                ret = NANO_OS_SCHEDULER_RegisterTask(task);
                if (ret == NOS_ERR_SUCCESS)
                {
                    /* Add task to the task list */
                    task->global_next = g_nano_os.tasks;
                    g_nano_os.tasks = task;

                    /* Update number of tasks */
                    g_nano_os.task_count++;

                    /* Real time trace event */
                    NANO_OS_TRACE_TASK(NOS_TRACE_OBJ_CREATE, task);
                }
            }
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_TASK_SLEEP_ENABLED == 1u)

/** \brief Suspend the current task for fixed number of ticks */
nano_os_error_t NANO_OS_TASK_Sleep(const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Real time trace event */
    NANO_OS_TRACE_TASK(NOS_TRACE_TASK_SLEEP, g_nano_os.current_task);

    /* Suspend task */
    ret = NANO_OS_WAIT_OBJECT_Wait(&g_nano_os.current_task->sleep_object, tick_count);
    NANO_OS_ERROR_ASSERT((ret == NOS_ERR_TIMEOUT), NOS_ERR_FAILURE);
    ret = NOS_ERR_SUCCESS;
    
    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_TASK_SLEEP_ENABLED == 1u) */


#if (NANO_OS_TASK_JOIN_ENABLED == 1u)

/** \brief Wait for the end of a task for fixed number of ticks */
nano_os_error_t NANO_OS_TASK_Join(nano_os_task_t* const task, void** return_value, const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((task != NULL) && (return_value != NULL))
    {
        /* Real time trace event */
        NANO_OS_TRACE_TASK(NOS_TRACE_TASK_JOIN, g_nano_os.current_task);

        /* Wait for the task */
        ret = NANO_OS_WAIT_OBJECT_Wait(&task->join_object, tick_count);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Get the task return value */
            (*return_value) = task->ret_value;

            /* Remove the task from the global task list to enable re-use of the task context object */
            (void)NANO_OS_TASK_RemoveFromGlobalList(task);
        }
        else
        {
            /* Check for internal error */
            NANO_OS_ERROR_ASSERT((ret == NOS_ERR_TIMEOUT), NOS_ERR_FAILURE);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */


#if (NANO_OS_TASK_GET_ENABLED == 1u)

/** \brief Get the context of a task identified by its task id */
nano_os_error_t NANO_OS_TASK_Get(const uint16_t task_id, nano_os_task_t** task)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (task != NULL)
    {
        /* Look for the specified task in the global tasks list */
        nano_os_task_t* current = g_nano_os.tasks;
        (*task) = NULL;
        while ((current != NULL) && ((*task) == NULL))
        {
            if (current->object_id == task_id)
            {
                (*task) = current;
            }
            else
            {
                current = current->global_next;
            }
        }
        if ((*task) == NULL)
        {
            ret = NOS_ERR_INVALID_OBJECT;
        }
        else
        {
            ret = NOS_ERR_SUCCESS;
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_TASK_GET_ENABLED == 1u) */


/** \brief Task startup function (should not be called directly by application) */
void NANO_OS_TASK_Start(nano_os_task_t* const task, fp_nano_os_task_func_t task_func, void* const param)
{
    /* Check parameters */
    if ((task != NULL) && (task_func != NULL))
    {
        nano_os_error_t err;

        #if (NANO_OS_TRACE_ENABLED == 1u)
        /* Raise priviledges */
        NANO_OS_PORT_RAISE_PRIVILEDGES();

        /* Real time trace event */
        NANO_OS_TRACE_TASK(NOS_TRACE_TASK_START, g_nano_os.current_task);

        /* Lower priviledges */
        NANO_OS_PORT_LOWER_PRIVILEDGES();
        #endif /* (NANO_OS_TRACE_ENABLED == 1u) */

        /* Call application task function */
        #if (NANO_OS_TASK_JOIN_ENABLED == 1u)
        task->ret_value = task_func(param);
        #else
        (void)task_func(param);
        #endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */

        /* Disable interrupts */
        NANO_OS_PORT_DISABLE_INTERRUPTS();

        /* Unregister task from scheduler */
        err = NANO_OS_SCHEDULER_UnregisterTask(task);
        if (err == NOS_ERR_SUCCESS)
        {
            #if (NANO_OS_TASK_JOIN_ENABLED == 1u)

            /* Signal waiting tasks */
            uint32_t woke_up_task_count = 0;
            err = NANO_OS_WAIT_OBJECT_Signal(&task->join_object, NOS_ERR_SUCCESS, true, NULL, &woke_up_task_count);
            NANO_OS_ERROR_ASSERT_RET(err);

            #if (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u)
            /* Destroy the join wait object */
            err = NANO_OS_WAIT_OBJECT_Destroy(&task->join_object);
            NANO_OS_ERROR_ASSERT_RET(err);
            #endif /* (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u) */

            #else

            /* Remove the task from the global task list to enable re-use of the task context object */
            err = NANO_OS_TASK_RemoveFromGlobalList(task);
            NANO_OS_ERROR_ASSERT_RET(err);

            #endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */


            #if ((NANO_OS_TASK_SLEEP_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u))
            /* Destroy the sleep wait object */
            err = NANO_OS_WAIT_OBJECT_Destroy(&task->join_object);
            NANO_OS_ERROR_ASSERT_RET(err);
            #endif /* ((NANO_OS_TASK_SLEEP_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u)) */

            /* Update task state */
            task->state = NOS_TS_DEAD;

            /* Real time trace event */
            NANO_OS_TRACE_TASK(NOS_TRACE_OBJ_DESTROY, g_nano_os.current_task);

            /* Update number of tasks */
            g_nano_os.task_count--;

            /* Schedule another task */
            g_nano_os.schedule_needed = true;
            NANO_OS_SCHEDULER_Schedule(false);
        }
    }
}


/** \brief Remove a task from the global task list */
static bool NANO_OS_TASK_RemoveFromGlobalList(nano_os_task_t* const task)
{
    bool ret = false;

    /* Look for the task in the list */
    nano_os_task_t* previous = NULL;
    nano_os_task_t* current = g_nano_os.tasks;
    while ((current != task) && (current != NULL))
    {
        previous = current;
        current = current->global_next;
    }

    /* Check if task has been found */
    if (current != NULL)
    {
        /* Remove the task from the list */
        if (previous != NULL)
        {
            previous->global_next = current->global_next;
        }
        else
        {
            g_nano_os.tasks = current->global_next;
        }

        ret = true;
    }

    return ret;
}
