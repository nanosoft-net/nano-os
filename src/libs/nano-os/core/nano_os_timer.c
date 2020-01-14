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

#include "nano_os_timer.h"

/* Check if module is enabled */
#if (NANO_OS_TIMER_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_trace.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"



/** \brief Timer task */
static void* NANO_OS_TIMER_Task(void* const param);

/** \brief Queue a timer to the started timer list */
static void NANO_OS_TIMER_Queue(nano_os_timer_t* const timer, const uint32_t first_due_time);

/** \brief Check if time2 represents a time which is after time1 based on the current_time */
static bool NANO_OS_TIMER_IsAfter(const uint32_t current_time, const uint32_t time1, const uint32_t time2);




/** \brief Initialize the timer module */
nano_os_error_t NANO_OS_TIMER_Init(const nano_os_port_init_data_t* const port_init_data)
{
    nano_os_task_init_data_t task_init_data;
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (port_init_data != NULL)
    {
        /* Initialize started timer list */
        g_nano_os.started_timers = NULL;

        /* Create timer task */
        (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
        #if (NANO_OS_TASK_NAME_ENABLED == 1u)
        task_init_data.name = "Timer task";
        #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
        task_init_data.base_priority = NANO_OS_NUMBER_OF_PRIORITY_LEVELS - 2u;
        task_init_data.stack_origin = g_nano_os.timer_task_stack;
        task_init_data.stack_size = NANO_OS_TIMER_TASK_STACK_SIZE;
        task_init_data.task_func = NANO_OS_TIMER_Task;
        task_init_data.param = NULL;
        #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
        (void)MEMCPY(&task_init_data.port_init_data, &port_init_data->timer_task_init_data, sizeof(nano_os_port_task_init_data_t));
        #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */
        ret = NANO_OS_TASK_Create(&g_nano_os.timer_task, &task_init_data);

        /* Init underlying wait object */
        if (ret == NOS_ERR_SUCCESS)
        {
            ret = NANO_OS_WAIT_OBJECT_Initialize(&g_nano_os.timer_task_wait_object, WOT_TASK, QT_PRIORITY);
        }
    }

    return ret;
}


/** \brief Create a new timer */
nano_os_error_t NANO_OS_TIMER_Create(nano_os_timer_t* const timer, const fp_nano_os_timer_callback_func_t callback, void* const user_data)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (timer != NULL)
    {
        /* 0 init of the timer */
        (void)MEMSET(timer, 0, sizeof(nano_os_timer_t));

        /* Save callback ans user data*/
        timer->callback = callback;
        timer->user_data = user_data;

        #if (NANO_OS_TRACE_ENABLED == 1u)
        /* Global object id */
        timer->object_id = g_nano_os.next_object_id;
        g_nano_os.next_object_id++;
        #endif /* (NANO_OS_TRACE_ENABLED == 1u) */

        /* Real time trace event */
        NANO_OS_TRACE_ADD(NOS_TRACE_OBJ_CREATE, timer->object_id);

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_TIMER_DESTROY_ENABLED == 1u)

/** \brief Destroy a timer */
nano_os_error_t NANO_OS_TIMER_Destroy(nano_os_timer_t* const timer)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if (timer != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_ADD(NOS_TRACE_OBJ_DESTROY, timer->object_id);

        /* Stop the timer if needed */
        (void)NANO_OS_TIMER_Stop(timer);
        
        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_TIMER_DESTROY_ENABLED == 1u) */


/** \brief Start a timer */
nano_os_error_t NANO_OS_TIMER_Start(nano_os_timer_t* const timer, const uint32_t first_due_time, const uint32_t period)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((timer != NULL) && (first_due_time != 0u))
    {
        nano_os_timer_t* current;

        /* Real time trace event */
        NANO_OS_TRACE_ADD(NOS_TRACE_TIMER_START, timer->object_id);

        /* Check if the timer is already started */
        current = g_nano_os.started_timers;
        while ((current != NULL) && (current != timer))
        {
            current = current->next;
        }
        if (current == NULL)
        {
            /* Configure timer */
            timer->has_elapsed = false;
            timer->period = period;

            /* Add timer to the timer list */
            NANO_OS_TIMER_Queue(timer, first_due_time);

            ret = NOS_ERR_SUCCESS;
        }
        else
        {
            ret = NOS_ERR_TIMER_STARTED;
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

/** \brief Stop a timer */
nano_os_error_t NANO_OS_TIMER_Stop(nano_os_timer_t* const timer)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if (timer != NULL)
    {
        nano_os_timer_t* previous;
        nano_os_timer_t* current;

        /* Real time trace event */
        NANO_OS_TRACE_ADD(NOS_TRACE_TIMER_STOP, timer->object_id);

        /* Check if the timer is already started */
        previous = NULL;
        current = g_nano_os.started_timers;
        while ((current != NULL) && (current != timer))
        {
            previous = current;
            current = current->next;
        }
        if (current != NULL)
        {
            nano_os_int_status_reg_t int_status_reg;

            /* Remove timer from the timer list */
            NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);
            if (previous == NULL)
            {
                g_nano_os.started_timers = timer->next;
            }
            else
            {
                previous->next = timer->next;
            }
            NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);

            /* Update timer state */
            timer->period = 0u;
            timer->next = NULL;
            timer->has_elapsed = false;

            ret = NOS_ERR_SUCCESS;
        }
        else
        {
            ret = NOS_ERR_TIMER_STOPPED;
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


/** \brief Check if a timer has elapsed */
nano_os_error_t NANO_OS_TIMER_HasElapsed(nano_os_timer_t* const timer, bool* const has_elapsed)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((timer != NULL) && (has_elapsed != NULL))
    {
        /* Copy elapsed state */
        (*has_elapsed) = timer->has_elapsed;

        /* Reset elapsed state */
        timer->has_elapsed = false;

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


/** \brief Timer tick handler */
nano_os_error_t NANO_OS_TIMER_TickHandler(void)
{

    /* Check if at least one timer is started */
    if (g_nano_os.started_timers != NULL)
    {
        /* Check if at least one timer has elapsed */
        if (g_nano_os.started_timers->timeout == g_nano_os.tick_count)
        {
            nano_os_error_t ret;
            uint32_t woke_up_task_count = 0;

            /* Lock scheduling */
            NANO_OS_PORT_ATOMIC_INC32(g_nano_os.lock_count);

            /* Wake up the timer task */
            ret = NANO_OS_WAIT_OBJECT_Signal(&g_nano_os.timer_task_wait_object, NOS_ERR_SUCCESS, false, NULL, &woke_up_task_count);
            NANO_OS_ERROR_ASSERT_RET(ret);
            NANO_OS_ERROR_ASSERT((woke_up_task_count != 0u), NOS_ERR_TIMER_TASK_OVERRUN);

            /* Unlock scheduling */
            NANO_OS_PORT_ATOMIC_DEC32(g_nano_os.lock_count);
        }
    }

    return NOS_ERR_SUCCESS;
}


/** \brief Timer task */
static void* NANO_OS_TIMER_Task(void* const param)
{
    nano_os_error_t ret;
    NANO_OS_UNUSED(param);

    /* Task loop */
    while(true)
    {
        /* Increment lock count as if it were a syscall */
        NANO_OS_PORT_ATOMIC_INC32(g_nano_os.lock_count);

        /* Wait for a timer to elapse */
        ret = NANO_OS_WAIT_OBJECT_Wait(&g_nano_os.timer_task_wait_object, 0xFFFFFFFFu);

        /* Decrement lock count as if it were a syscall */
        NANO_OS_PORT_ATOMIC_DEC32(g_nano_os.lock_count);

        /* Check wait status */
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Handle elapsed timers */
            uint32_t timer_timeout;
            nano_os_timer_t* current;

            /* Disable interrupts */
            NANO_OS_PORT_DISABLE_INTERRUPTS();

            /* Go through started timer to find elapsed timers */
            current = g_nano_os.started_timers;
            timer_timeout = current->timeout;
            while ((current != NULL) && (current->timeout == g_nano_os.tick_count))
            {
                /* Remove timer from timer list */
                g_nano_os.started_timers = current->next;

                /* Update timer state */
                current->has_elapsed = true;

                /* Call user callback function if any */
                if (current->callback != NULL)
                {
                    /* Enable interrupts */
                    NANO_OS_PORT_ENABLE_INTERRUPTS();

                    /* Lower task priviledges */
                    NANO_OS_PORT_LOWER_PRIVILEDGES();

                    /* Call user callback function */
                    current->callback(current, current->user_data);

                    /* Raise task priviledges */
                    NANO_OS_PORT_RAISE_PRIVILEDGES();

                    /* Disable interrupts */
                    NANO_OS_PORT_DISABLE_INTERRUPTS();
                }

                /* Requeue timer if periodic */
                if (current->period != 0u)
                {
                    NANO_OS_TIMER_Queue(current, 0u);
                }

                /* Next timer */
                current = g_nano_os.started_timers;
            }

            /* Check that timer handling did not take more than 1 tick */
            NANO_OS_ERROR_ASSERT((timer_timeout == g_nano_os.tick_count), NOS_ERR_TIMER_TASK_OVERRUN);

            /* Enable interrupts */
            NANO_OS_PORT_ENABLE_INTERRUPTS();
        }
        else if (ret == NOS_ERR_TIMEOUT)
        {
            /* No timer to handle, return in wait state */
        }
        else
        {
            /* Critical error */
            NANO_OS_ERROR_Handler(ret);
        }

    }

    return NULL;
}



/** \brief Queue a timer to the started timer list */
static void NANO_OS_TIMER_Queue(nano_os_timer_t* const timer, const uint32_t first_due_time)
{
    nano_os_int_status_reg_t int_status_reg;
    const uint32_t current_time = NANO_OS_PORT_ATOMIC_READ32(g_nano_os.tick_count);

    /* Configure timeout */
    if (first_due_time != 0u)
    {
        timer->timeout = current_time + first_due_time;
    }
    else
    {
        timer->timeout = current_time + timer->period;
    }

    /* Add timer to the timer list */
    if (g_nano_os.started_timers == NULL)
    {
        NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);
        timer->next = g_nano_os.started_timers;
        g_nano_os.started_timers = timer;
        NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);
    }
    else
    {
        bool is_after;
        nano_os_timer_t* previous = NULL;
        nano_os_timer_t* current = g_nano_os.started_timers;
        do
        {
            is_after = NANO_OS_TIMER_IsAfter(current_time, timer->timeout, current->timeout);
            if (!is_after)
            {
                previous = current;
                current = current->next;
            }
        }
        while ((current != NULL) && (!is_after));

        NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);
        if (previous != NULL)
        {
            previous->next = timer;
            timer->next = current;
        }
        else
        {
            timer->next = g_nano_os.started_timers;
            g_nano_os.started_timers = timer;
        }
        NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);
    }
}


/** \brief Check if time2 represents a time which is after time1 based on the current_time */
static bool NANO_OS_TIMER_IsAfter(const uint32_t current_time, const uint32_t time1, const uint32_t time2)
{
    bool ret = false;

    if (time1 > current_time)
    {
        if ((time2 > time1) || (time2 < current_time))
        {
            ret = true;
        }
    }
    else
    {
        if ((time2 > time1) && (time2 < current_time))
        {
            ret = true;
        }
    }

    return ret;
}

#endif /* (NANO_OS_TIMER_ENABLED == 1u) */
