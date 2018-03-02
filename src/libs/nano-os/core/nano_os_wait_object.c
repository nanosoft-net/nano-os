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
#include "nano_os_task.h"
#include "nano_os_port.h"
#include "nano_os_tools.h"
#include "nano_os_syscall.h"
#include "nano_os_task_list.h"
#include "nano_os_scheduler.h"
#include "nano_os_wait_object.h"


/** \brief Queuing function prototype */
typedef nano_os_error_t (*nano_os_queuing_func_t)(nano_os_task_t** task_list, nano_os_task_t* const task);


/** \brief Available queuing functions */
static const nano_os_queuing_func_t queuing_functions[] = {
                                                            NANO_OS_TASK_LIST_AddToEnd,
                                                            NANO_OS_TASK_LIST_AddHighPriority
                                                           };


/** \brief Wake up the waiting tasks */
static nano_os_error_t NANO_OS_WAIT_OBJECT_WakeUpTasks(nano_os_wait_object_t* const wait_object, const nano_os_error_t wait_status,
                                                       const bool signal_all_tasks, const fp_nano_os_wait_object_wakeup_cond_func_t wakeup_cond_func,
                                                       uint32_t* const woke_up_task_count);




/** \brief Initialize a wait object */
nano_os_error_t NANO_OS_WAIT_OBJECT_Initialize(nano_os_wait_object_t* const wait_object, const nano_os_wait_object_type_t type,
                                               const nano_os_queuing_type_t queuing_type)
{
    nano_os_error_t ret = NOS_ERR_INVALID_OBJECT;

    /* Check parameters */
    if ((wait_object != NULL) && (wait_object->type == WOT_NOT_INIT) &&
        (type != WOT_NOT_INIT))
    {
        /* Check queuing type */
        switch(queuing_type)
        {
            case QT_FIFO:
                /* Intended fallthrough */

            case QT_PRIORITY:
                /* Queuing type is valid */
                ret = NOS_ERR_SUCCESS;
                break;

            default:
                /* Invalid value */
                break;
        }
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Initialize wait object */
            wait_object->type = type;
            wait_object->queuing = queuing_type;
            wait_object->waiting_tasks = NULL;

            #if (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u)
            /* Add object to the global list */
            wait_object->next = g_nano_os.wait_objects;
            g_nano_os.wait_objects = wait_object;

            /* Update number of wait objects */
            g_nano_os.wait_object_count++;
            #endif /* (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u) */

            #if (NANO_OS_TRACE_ENABLED == 1u)
            /* Global object id */
            wait_object->object_id = g_nano_os.next_object_id;
            g_nano_os.next_object_id++;
            #endif /* (NANO_OS_TRACE_ENABLED == 1u) */

            #if (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u)
            /* Name */
            wait_object->name = NULL;
            #endif /* (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u) */
        }
    }

    return ret;
}

/** \brief Wait for the wait object to be signaled */
nano_os_error_t NANO_OS_WAIT_OBJECT_Wait(nano_os_wait_object_t* const wait_object, const uint32_t timeout)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((wait_object != NULL) && (wait_object->type != WOT_NOT_INIT))
    {
        /* Case of NULL timeout */
        if (timeout == 0u)
        {
            ret = NOS_ERR_TIMEOUT;
        }
        else
        {
            /* Get the current task */
            nano_os_task_t* const task = g_nano_os.current_task;

            /* Update task wait data */
            task->waiting_object = wait_object;

            /* Set task pending */
            ret = NANO_OS_SCHEDULER_SetTaskPending(task, timeout);

            /* Add the task to the waiting list */
            if (ret == NOS_ERR_SUCCESS)
            {
                ret = queuing_functions[wait_object->queuing](&wait_object->waiting_tasks, task);
                if (ret == NOS_ERR_SUCCESS)
                {
                    nano_os_int_status_reg_t int_status_reg;

                    /* Disable interrupts */
                    NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

                    /* Allow operations on this wait object */
                    WAIT_OBJECT_ISR_UNLOCK(*wait_object);

                    /* Decrement the lock count */
                    g_nano_os.lock_count--;

                    /* Start a new scheduling */
                    NANO_OS_SCHEDULER_Schedule(false);

                    /* Increment the lock count */
                    g_nano_os.lock_count++;

                    /* Restore the wait object locked state */
                    WAIT_OBJECT_ISR_LOCK(*wait_object);

                    /* Enable interrupts */
                    NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);

                    /* Get wait return value */
                    ret = task->wait_status;
                }
            }
        }
    }

    return ret;
}

/** \brief Signal the wait object */
nano_os_error_t NANO_OS_WAIT_OBJECT_Signal(nano_os_wait_object_t* const wait_object, const nano_os_error_t wait_status,
                                           const bool signal_all_tasks, const fp_nano_os_wait_object_wakeup_cond_func_t wakeup_cond_func,
                                           uint32_t* const woke_up_task_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((wait_object != NULL) &&
        (wait_object->type != WOT_NOT_INIT) &&
        (woke_up_task_count != NULL))
    {
        /* Signal the waiting task(s) */
        ret = NANO_OS_WAIT_OBJECT_WakeUpTasks(wait_object, wait_status, signal_all_tasks, wakeup_cond_func, woke_up_task_count);
    }

    return ret;
}

#if (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u)

/** \brief Destroy a wait object */
nano_os_error_t NANO_OS_WAIT_OBJECT_Destroy(nano_os_wait_object_t* const wait_object)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((wait_object != NULL) &&
        (wait_object->type != WOT_NOT_INIT))
    {
        uint32_t woke_up_task_count = 0u;

        /* Signal the waiting task(s) */
        ret = NANO_OS_WAIT_OBJECT_WakeUpTasks(wait_object, NOS_ERR_OBJECT_DESTROYED, true, NULL, &woke_up_task_count);
        if (ret == NOS_ERR_SUCCESS)
        {
            #if (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u)
            /* Remove wait object from global list */
            nano_os_wait_object_t* previous = NULL;
            nano_os_wait_object_t* current = g_nano_os.wait_objects;
            while ((current != NULL) && (current != wait_object))
            {
                previous = current;
                current = current->next;
            }

            NANO_OS_ERROR_ASSERT(current != NULL, NOS_ERR_INVALID_OBJECT);

            if (previous == NULL)
            {
                g_nano_os.wait_objects = wait_object->next;
            }
            else
            {
                previous->next = wait_object->next;
            }

            /* Update number of wait objects */
            g_nano_os.wait_object_count--;

            #endif /* (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u) */

            /* De-init wait object */
            (void)MEMSET(wait_object, 0, sizeof(nano_os_wait_object_t));
        }
    }

    return ret;
}

#endif /* (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u) */


/** \brief Wake up the waiting tasks */
static nano_os_error_t NANO_OS_WAIT_OBJECT_WakeUpTasks(nano_os_wait_object_t* const wait_object, const nano_os_error_t wait_status,
                                                       const bool signal_all_tasks, const fp_nano_os_wait_object_wakeup_cond_func_t wakeup_cond_func,
                                                       uint32_t* const woke_up_task_count)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Signal the waiting task(s) */
    nano_os_task_t* task = NANO_OS_TASK_LIST_PopTask(&wait_object->waiting_tasks);
    if (task != NULL)
    {
        nano_os_task_t* last_requeued_task = NULL;
        nano_os_task_t* requeued_tasks = NULL;
        (*woke_up_task_count) = 0;
        do
        {
            /* Call OS object specific wakeup condition function */
            bool wakeup_task = true;
            if (wakeup_cond_func != NULL)
            {
                wakeup_task = wakeup_cond_func(wait_object, task);
            }

            if (wakeup_task)
            {
                /* Update task wait data */
                task->waiting_object = NULL;
                task->wait_timeout = 0u;
                task->wait_status = wait_status;

                /* Change task to ready state */
                ret = NANO_OS_SCHEDULER_SetTaskReady(task);
                if (ret != NOS_ERR_SUCCESS)
                {
                    /* Critical error */
                    NANO_OS_ERROR_Handler(ret);
                }

                /* Increase task wake up count */
                (*woke_up_task_count)++;
            }
            else
            {
                /* Requeue task */
                (void)NANO_OS_TASK_LIST_AddToEnd(&requeued_tasks, task);
                last_requeued_task = task;
            }

            /* Get next task */
            task = NANO_OS_TASK_LIST_PopTask(&wait_object->waiting_tasks);
        }
        while ((task != NULL) && signal_all_tasks);

        /* Put the tasks which have not been woken back into the task queue */
        if (last_requeued_task != NULL)
        {
            last_requeued_task->next = wait_object->waiting_tasks;
            wait_object->waiting_tasks = requeued_tasks;
        }
    }

    return ret;
}
