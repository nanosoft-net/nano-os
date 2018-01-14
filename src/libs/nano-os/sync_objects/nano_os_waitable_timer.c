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

#include "nano_os_waitable_timer.h"

/* Check if module is enabled */
#if (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_trace.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"



/** \brief Generic timer callback for all waitable timers */
static void NANO_OS_WAITABLE_TIMER_Callback(nano_os_timer_t* const timer, void* const user_data);



/** \brief Create a new waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Create(nano_os_waitable_timer_t* const timer, const nano_os_queuing_type_t queuing_type)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (timer != NULL)
    {
        /* 0 init of the timer */
        (void)MEMSET(timer, 0, sizeof(nano_os_waitable_timer_t));

        /* Init underlying timer */
        ret = NANO_OS_TIMER_Create(&timer->timer, NANO_OS_WAITABLE_TIMER_Callback, timer);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Init underlying wait object */
            ret = NANO_OS_WAIT_OBJECT_Initialize(&timer->wait_object, WOT_TIMER, queuing_type);
            if (ret == NOS_ERR_SUCCESS)
            {
                /* Real time trace event */
                NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_CREATE, timer);
            }
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u)

/** \brief Destroy a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Destroy(nano_os_waitable_timer_t* const timer)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((timer != NULL) && (timer->wait_object.type == WOT_TIMER))
    {
        nano_os_timer_t* current;

        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_DESTROY, timer);

        /* Check if the timer is already started */
        current = g_nano_os.started_timers;
        while ((current != NULL) && (current != &timer->timer))
        {
            current = current->next;
        }
        if (current == NULL)
        {
            /* Destroy wait object */
            ret = NANO_OS_WAIT_OBJECT_Destroy(&timer->wait_object);
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

#endif /* (NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u) */


/** \brief Start a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Start(nano_os_waitable_timer_t* const timer, const uint32_t first_due_time, const uint32_t period)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((timer != NULL) && (timer->wait_object.type == WOT_TIMER))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_WAITABLE_TIMER_START, timer);

        /* Start the timer */
        ret = NANO_OS_TIMER_Start(&timer->timer, first_due_time, period);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

/** \brief Stop a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Stop(nano_os_waitable_timer_t* const timer)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((timer != NULL) && (timer->wait_object.type == WOT_TIMER))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_WAITABLE_TIMER_STOP, timer);

        /* Stop the timer */
        ret = NANO_OS_TIMER_Stop(&timer->timer);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Signal waiting tasks */
            uint32_t woke_up_task_count = 0u;
            ret = NANO_OS_WAIT_OBJECT_Signal(&timer->wait_object, NOS_ERR_TIMER_STOPPED, true, NULL, &woke_up_task_count);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


/** \brief Wait for a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Wait(nano_os_waitable_timer_t* const timer, const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((timer != NULL) && (timer->wait_object.type == WOT_TIMER))
    {
        nano_os_timer_t* current;

        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_WAITABLE_TIMER_WAIT, timer);

        /* Check if the timer is already started */
        current = g_nano_os.started_timers;
        while ((current != NULL) && (current != &timer->timer))
        {
            current = current->next;
        }
        if (current != NULL)
        {
            /* Check if the timer has elapsed */
            if (timer->timer.has_elapsed)
            {
                ret = NOS_ERR_SUCCESS;
            }
            else
            {
                /* Wait for the timer */
                ret = NANO_OS_WAIT_OBJECT_Wait(&timer->wait_object, tick_count);
                if (ret == NOS_ERR_SUCCESS)
                {
                    /* Reset timer state */
                    timer->timer.has_elapsed = false;
                }
            }
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


#if (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_SetName(nano_os_waitable_timer_t* const timer, const char* const name)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (timer != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_NAMING, timer);

        /* Save the name */
        timer->wait_object.name = name;

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u) */



/** \brief Generic timer callback for all waitable timers */
static void NANO_OS_WAITABLE_TIMER_Callback(nano_os_timer_t* const timer, void* const user_data)
{
    nano_os_waitable_timer_t* const waitable_timer = NANO_OS_CAST(nano_os_waitable_timer_t*, user_data);

    /* Check parameters */
    if ((timer != NULL) && (waitable_timer != NULL) && (waitable_timer->wait_object.type == WOT_TIMER))
    {
        nano_os_error_t ret;
        uint32_t woke_up_task_count = 0u;
        
        /* Raise task priviledges */
        NANO_OS_PORT_RAISE_PRIVILEDGES();

        /* Increment lock count as if it were a syscall */
        NANO_OS_PORT_ATOMIC_INC32(g_nano_os.lock_count);

        /* Signal waiting tasks */
        ret = NANO_OS_WAIT_OBJECT_Signal(&waitable_timer->wait_object, NOS_ERR_SUCCESS, true, NULL, &woke_up_task_count);
        NANO_OS_ERROR_ASSERT_RET(ret);

        /* Decrement lock count as if it were a syscall */
        NANO_OS_PORT_ATOMIC_DEC32(g_nano_os.lock_count);
    }
}


#endif /* (NANO_OS_WAITABLE_TIMER_ENABLED == 1u) */
