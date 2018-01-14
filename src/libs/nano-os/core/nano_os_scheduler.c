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
#include "nano_os_trace.h"
#include "nano_os_scheduler.h"
#include "nano_os_task_list.h"


/** \brief Look-up table for the bitfield tables */
static const uint8_t  ready_tasks_bitfield_lookup[256u] = {
                                                            /* Priority bit 0 */
                                                            0, 0,
                                                            /* Priority bit 1 */
                                                            1, 1,
                                                            /* Priority bit 2 */
                                                            2, 2, 2, 2,
                                                            /* Priority bit 3 */
                                                            3, 3, 3, 3, 3, 3, 3, 3,
                                                            /* Priority bit 4 */
                                                            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
                                                            /* Priority bit 5 */
                                                            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                                            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
                                                            /* Priority bit 6 */
                                                            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                                                            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                                                            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                                                            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
                                                            /* Priority bit 7 */
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
                                                            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
                                                          };


/** \brief Get the highest priority ready task */
static nano_os_task_t* NANO_OS_SCHEDULER_GetHighestReady(void);

/** \brief Add a task to the ready list */
static bool NANO_OS_SCHEDULER_AddToReadyList(nano_os_task_t* const task);

/** \brief Remove a task from the ready list */
static bool NANO_OS_SCHEDULER_RemoveFromReadyList(nano_os_task_t* const task);

/** \brief Remove a task from the suspent list */
static bool NANO_OS_SCHEDULER_RemoveFromSuspentList(nano_os_task_t* const task);



/** \brief First scheduling which will start Nano OS */
nano_os_error_t NANO_OS_SCHEDULER_FirstScheduling(void)
{
    nano_os_error_t ret = NOS_ERR_NO_TASK_READY;

    /* Get the highest priority ready task */
    nano_os_task_t* const ready_task = NANO_OS_SCHEDULER_GetHighestReady();
    if (ready_task != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_ADD(NOS_TRACE_SCHEDULER_START, 0u);

        /* Prepare a switch to current task */
        g_nano_os.next_running_task = ready_task;

        /* Update the task state */
        ready_task->state = NOS_TS_RUNNING;

        #if (NANO_OS_TASK_EXECUTION_COUNTER_ENABLED == 1u)
        /* Update the execution counter */
        ready_task->execution_counter++;
        #endif /* (NANO_OS_TASK_EXECUTION_COUNTER_ENABLED == 1u) */

        /* No more schedule needed */
        g_nano_os.schedule_needed = false;

        /* OS is started */
        g_nano_os.started = true;

        /* Real time trace event */
        NANO_OS_TRACE_TASK(NOS_TRACE_SCHEDULER_CONTEXT_SWITCH_ON, g_nano_os.next_running_task);

        /* First context switch */
        ret = NANO_OS_PORT_FirstContextSwitch();

        /* We should never reach this code unless the OS fails to start */
        g_nano_os.started = false;
    }

    return ret;
}


/** \brief Schedule the next running task and do a context switch if needed */
void NANO_OS_SCHEDULER_Schedule(const bool from_isr)
{
    /* Check if a schedule is needed */
    if (g_nano_os.schedule_needed && g_nano_os.started)
    {
        /* Get the highest priority ready task */
        nano_os_task_t* const ready_task = NANO_OS_SCHEDULER_GetHighestReady();

        /* Check if the task is the current task */
        if (ready_task != g_nano_os.current_task)
        {
            #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
            /* Update data for CPU usage measurement */
            const uint32_t now_in_us = NANO_OS_PORT_GetTimestampInUs();
            g_nano_os.current_task->execution_time += now_in_us - g_nano_os.context_switch_timestamp;
            g_nano_os.context_switch_timestamp = now_in_us;
            #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */


            /* This is not the current task, a context switch needed */
            g_nano_os.next_running_task = ready_task;

            /* Update the task state */
            ready_task->state = NOS_TS_RUNNING;
            if (g_nano_os.current_task->state == NOS_TS_RUNNING)
            {
                g_nano_os.current_task->state = NOS_TS_READY;
            }

            #if (NANO_OS_TASK_EXECUTION_COUNTER_ENABLED == 1u)
            /* Update the execution counter */
            g_nano_os.next_running_task->execution_counter++;
            #endif /* (NANO_OS_TASK_EXECUTION_COUNTER_ENABLED == 1u) */

            /* No more schedule needed */
            g_nano_os.schedule_needed = false;

            /* Real time trace event */
            NANO_OS_TRACE_TASK(NOS_TRACE_SCHEDULER_CONTEXT_SWITCH_OFF, g_nano_os.current_task);
            NANO_OS_TRACE_TASK(NOS_TRACE_SCHEDULER_CONTEXT_SWITCH_ON, g_nano_os.next_running_task);


            #if (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u)

            /* Check current task stack pointer range */
            {
                nano_os_task_t* const current_task = g_nano_os.current_task;
                const uint32_t lower_limit = NANO_OS_CAST(uint32_t, current_task->stack_origin);
                const uint32_t upper_limit = NANO_OS_CAST(uint32_t, &current_task->stack_origin[current_task->stack_size]);
                uint32_t task_sp = NANO_OS_PORT_GET_TASK_SP();

                /* Adjust stack pointer with the task context frame which will be stored on the
                   stack during context switch */
                #if (NANO_OS_PORT_DESCENDING_STACK == 1u)
                task_sp -= (NANO_OS_PORT_TASK_CONTEXT_STACK_FRAME_SIZE(current_task) * sizeof(nano_os_stack_t));
                #else
                task_sp += (NANO_OS_PORT_TASK_CONTEXT_STACK_FRAME_SIZE(current_task) * sizeof(nano_os_stack_t));
                #endif /* (NANO_OS_PORT_DESCENDING_STACK == 1u) */

                NANO_OS_ERROR_ASSERT(((task_sp >= lower_limit) && (task_sp < upper_limit)), NOS_ERR_INVALID_SP);
            }

            /* Check if current task stack pointer has gone outside of stack range during task execution */
            NANO_OS_ERROR_ASSERT( ((g_nano_os.stack_marker == g_nano_os.current_task->stack_origin[0u]) ||
                                   (g_nano_os.stack_marker == g_nano_os.current_task->stack_origin[g_nano_os.current_task->stack_size - 1u])),
                                  NOS_ERR_CORRUPTED_STACK);

            #endif /* (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u) */

            /* Context switch */
            if (from_isr)
            {
                /* Interrupt level context switch */
                NANO_OS_PORT_ContextSwitchFromIsr();
            }
            else
            {
                /* Task level context switch */
                NANO_OS_PORT_ContextSwitch();
            }
        }
    }
}



/** \brief Register a task to the scheduler */
nano_os_error_t NANO_OS_SCHEDULER_RegisterTask(nano_os_task_t* const task)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((task != NULL) && (task->base_priority < NANO_OS_NUMBER_OF_PRIORITY_LEVELS))
    {
        #if (NANO_OS_ROUND_ROBIN_ENABLED == 0u)
        if (g_nano_os.ready_tasks[task->base_priority] != NULL)
        {
            ret = NOS_ERR_INVALID_PRIORITY_LEVEL;
        }
        else
        {
        #endif /* (NANO_OS_ROUND_ROBIN_ENABLED == 0u) */

        /* Set the task in ready state */
        ret = NANO_OS_SCHEDULER_SetTaskReady(task);

        #if (NANO_OS_ROUND_ROBIN_ENABLED == 0u)
        }
        #endif /* (NANO_OS_ROUND_ROBIN_ENABLED == 0u) */
    }

    return ret;
}


/** \brief Unregister a task from the scheduler */
nano_os_error_t NANO_OS_SCHEDULER_UnregisterTask(nano_os_task_t* const task)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (task != NULL)
    {
        /* Try to remove the task from the ready list */
        bool remove_ret = NANO_OS_SCHEDULER_RemoveFromReadyList(task);
        if (!remove_ret)
        {
            /* Remove the task from the pending list */
            remove_ret = NANO_OS_SCHEDULER_RemoveFromSuspentList(task);
        }
        if (remove_ret)
        {
            ret = NOS_ERR_SUCCESS;
        }
    }

    return ret;
}


/** \brief Set a task into ready state */
nano_os_error_t NANO_OS_SCHEDULER_SetTaskReady(nano_os_task_t* const task)
{
    nano_os_int_status_reg_t int_status_reg;
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Critical section */
    NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

    /* Check parameters */
    if (task != NULL)
    {
        bool list_ret = true;

        /* Remove the task from the pending list */
        if (task->state == NOS_TS_PENDING)
        {
            list_ret = NANO_OS_SCHEDULER_RemoveFromSuspentList(task);
        }
        if (list_ret)
        {
            /* Add the task to the ready list */
            list_ret = NANO_OS_SCHEDULER_AddToReadyList(task);
            if (list_ret)
            {
                /* Update the task state */
                task->state = NOS_TS_READY;
                #if (NANO_OS_ROUND_ROBIN_ENABLED == 1u)
                task->timeslice = NANO_OS_ROUND_ROBIN_TIMESLICE;
                #endif /* (NANO_OS_ROUND_ROBIN_ENABLED == 1u) */

                /* A schedule is needed as the task may be the highest priority task ready */
                g_nano_os.schedule_needed = true;

                ret = NOS_ERR_SUCCESS;
            }
        }
    }

    /* End of critical section */
    NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);

    return ret;
}


/** \brief Set a task into pending state */
nano_os_error_t NANO_OS_SCHEDULER_SetTaskPending(nano_os_task_t* const task, const uint32_t tick_count)
{
    nano_os_int_status_reg_t int_status_reg;
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Critical section */
    NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

    /* Check parameters */
    if (task != NULL)
    {
        /* Remove task from ready list */
        const bool remove_ret = NANO_OS_SCHEDULER_RemoveFromReadyList(task);
        if (remove_ret)
        {
            /* Update the task state */
            task->state = NOS_TS_PENDING;
            task->wait_timeout = NANO_OS_PORT_ATOMIC_READ32(g_nano_os.tick_count) + tick_count;

            /* Add the task at the start of the suspent list */
            task->next = NULL;
            task->suspent_next = g_nano_os.suspent_tasks;
            g_nano_os.suspent_tasks = task;

            /* A schedule is needed as the task may be the highest priority task ready */
            g_nano_os.schedule_needed = true;

            ret = NOS_ERR_SUCCESS;
        }
    }

    /* End of critical section */
    NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);

    return ret;
}


/** \brief Go through all the suspent tasks to check their wake-up timeout */
nano_os_error_t NANO_OS_SCHEDULER_HandleSuspentTasks(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Look for tasks to wake up */
    nano_os_task_t* current = g_nano_os.suspent_tasks;
    while (current != NULL)
    {
        /* Check wake up time */
        if (current->wait_timeout == g_nano_os.tick_count)
        {
            nano_os_task_t* next = current->suspent_next;

            /* Update the task wait status */
            current->wait_status = NOS_ERR_TIMEOUT;

            /* Set the task in ready state */
            ret = NANO_OS_SCHEDULER_SetTaskReady(current);
            NANO_OS_ERROR_ASSERT_RET(ret);

            /* Next task */
            current = next;
        }
        else
        {
            /* Next task */
            current = current->suspent_next;
        }
    }

    #if (NANO_OS_ROUND_ROBIN_ENABLED == 1u)
    /* Check if round robin is needed for this task */
    current = g_nano_os.current_task;
    if (current->next != NULL)
    {
        /* Decrease time slice */
        current->timeslice--;
        if (current->timeslice == 0u)
        {
            nano_os_error_t err;

            /* Change first ready task for this priority level */
            g_nano_os.ready_tasks[current->priority] = current->next;
            current->timeslice = NANO_OS_ROUND_ROBIN_TIMESLICE;
            current->state = NOS_TS_READY;

            /* Requeue current task at the end of the ready task list */
            err  = NANO_OS_TASK_LIST_AddToEnd(&g_nano_os.ready_tasks[current->priority], current);
            NANO_OS_ERROR_ASSERT_RET(err);

            /* A scheduling is needed */
            g_nano_os.schedule_needed = true;
        }

    }
    #endif /* (NANO_OS_ROUND_ROBIN_ENABLED == 1u) */

    return ret;
}




/** \brief Get the highest priority ready task */
static nano_os_task_t* NANO_OS_SCHEDULER_GetHighestReady(void)
{
    nano_os_task_t* task = NULL;
    int8_t bit_index = NANO_OS_CAST(int8_t, sizeof(g_nano_os.ready_tasks_bitfield)) - 1;

    do
    {
        if (g_nano_os.ready_tasks_bitfield[bit_index] != 0u)
        {
            const uint8_t bit_number = ready_tasks_bitfield_lookup[g_nano_os.ready_tasks_bitfield[bit_index]];
            const uint8_t task_index = bit_index * 8u + bit_number;
            if (task_index < NANO_OS_NUMBER_OF_PRIORITY_LEVELS)
            {
                task = g_nano_os.ready_tasks[task_index];
            }
        }
        else
        {
            bit_index--;
        }
    }
    while ((bit_index >= 0) && (task == NULL));
    if (task == NULL)
    {
        /* Critical error => no more task in ready state */
        NANO_OS_ERROR_Handler(NOS_ERR_NO_TASK_READY);
    }

    return task;
}


/** \brief Add a task to the ready list */
static bool NANO_OS_SCHEDULER_AddToReadyList(nano_os_task_t* const task)
{
    bool ret = false;

    /* Check priority level */
    if (task->priority < NANO_OS_NUMBER_OF_PRIORITY_LEVELS)
    {
        /* Add the task at the end of the ready list of its priority level */
        const nano_os_error_t err = NANO_OS_TASK_LIST_AddToEnd(&g_nano_os.ready_tasks[task->priority], task);
        if (err == NOS_ERR_SUCCESS)
        {
            /* Update the ready list bitfield */
            const uint8_t bit_index = task->priority / 8u;
            const uint8_t bit_number = task->priority % 8u;
            g_nano_os.ready_tasks_bitfield[bit_index] |= NANO_OS_CAST(uint8_t, (1u << bit_number));

            ret = true;
        }
        else
        {
            /* Critical error */
            NANO_OS_ERROR_Handler(err);
        }
    }

    return ret;
}

/** \brief Remove a task from the ready list */
static bool NANO_OS_SCHEDULER_RemoveFromReadyList(nano_os_task_t* const task)
{
    bool ret = false;

    /* Remove the task from the ready list */
    const nano_os_error_t err = NANO_OS_TASK_LIST_Remove(&g_nano_os.ready_tasks[task->priority], task);
    if (err == NOS_ERR_SUCCESS)
    {
        /* Update the ready list bitfield */
        if (g_nano_os.ready_tasks[task->priority] == NULL)
        {
            const uint8_t bit_index = task->priority / 8u;
            const uint8_t bit_number = task->priority % 8u;
            g_nano_os.ready_tasks_bitfield[bit_index] &= NANO_OS_CAST(uint8_t, ~(1u << bit_number));
        }

        ret = true;
    }

    return ret;
}

/** \brief Remove a task from the suspent list */
static bool NANO_OS_SCHEDULER_RemoveFromSuspentList(nano_os_task_t* const task)
{
    bool ret = false;

    /* Look for the task in the list */
    nano_os_task_t* previous = NULL;
    nano_os_task_t* current = g_nano_os.suspent_tasks;
    while ((current != task) && (current != NULL))
    {
        previous = current;
        current = current->suspent_next;
    }

    /* Check if task has been found */
    if (current != NULL)
    {
        /* Remove the task from the list */
        if (previous != NULL)
        {
            previous->suspent_next = current->suspent_next;
        }
        else
        {
            g_nano_os.suspent_tasks = current->suspent_next;
        }
        current->suspent_next = NULL;

        ret = true;
    }

    return ret;
}
