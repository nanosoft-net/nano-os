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

#include "nano_os_flag_set.h"

/* Check if module is enabled */
#if (NANO_OS_FLAG_SET_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_trace.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"


/** \brief Prototype for the OS object specific action on wait object signal */
static bool NANO_OS_FLAG_SET_WaitObjectWakeupCond(nano_os_wait_object_t* const wait_object, nano_os_task_t* task);




/** \brief Create a new flag set */
nano_os_error_t NANO_OS_FLAG_SET_Create(nano_os_flag_set_t* const flag_set, const uint32_t initial_mask,
                                        const nano_os_queuing_type_t queuing_type)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (flag_set != NULL)
    {
        /* 0 init of flag set */
        (void)MEMSET(flag_set, 0, sizeof(nano_os_flag_set_t));

        /* Init underlying wait object */
        ret = NANO_OS_WAIT_OBJECT_Initialize(&flag_set->wait_object, WOT_FLAG_SET, queuing_type);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Initialize flag set data */
            flag_set->mask = initial_mask;

            /* Real time trace event */
            NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_CREATE, flag_set);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u)

/** \brief Destroy a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Destroy(nano_os_flag_set_t* const flag_set)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((flag_set != NULL) && (flag_set->wait_object.type == WOT_FLAG_SET))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_DESTROY, flag_set);

        /* Destroy wait object */
        ret = NANO_OS_WAIT_OBJECT_Destroy(&flag_set->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u) */


/** \brief Wait for a specific flag mask in a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Wait(nano_os_flag_set_t* const flag_set, const uint32_t wait_mask,
                                      const bool wait_all, const bool reset_flags,
                                      uint32_t* const wakeup_flags, const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((flag_set != NULL) && (flag_set->wait_object.type == WOT_FLAG_SET) &&
        (wakeup_flags != NULL))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_FLAG_SET_WAIT, flag_set);

        /* Lock flag set against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(flag_set->wait_object);

        /* Check if the flags are matching */
        if ((wait_all && ((flag_set->mask & wait_mask) == wait_mask)) ||
            (!wait_all && ((flag_set->mask & wait_mask) != 0u)))
        {
            /* Flags are matching */
            g_nano_os.current_task->flag_set_wait_mask = flag_set->mask & wait_mask;
            ret = NOS_ERR_SUCCESS;
        }
        else
        {
            /* Save the flag mask and the wait mode in the task context */
            g_nano_os.current_task->flag_set_wait_mask = wait_mask;
            g_nano_os.current_task->flag_set_wait_all = wait_all;

            /* Wait for the specified flags */
            ret = NANO_OS_WAIT_OBJECT_Wait(&flag_set->wait_object, tick_count);
        }
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Get the flags which did trigger the task wakeup */
            (*wakeup_flags) = g_nano_os.current_task->flag_set_wait_mask;

            /* Reset flags if needed */
            if (reset_flags)
            {
                flag_set->mask &= ~g_nano_os.current_task->flag_set_wait_mask;
            }
        }

        /* Unlock flag set against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(flag_set->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


/** \brief Set a flag mask in a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Set(nano_os_flag_set_t* const flag_set, const uint32_t mask)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((flag_set != NULL) && (flag_set->wait_object.type == WOT_FLAG_SET))
    {
        uint32_t woke_up_task_count = 0u;

        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_FLAG_SET_SET, flag_set);

        /* Lock flag set against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(flag_set->wait_object);

        /* Update wait mask */
        flag_set->mask |= mask;

        /* Signal waiting tasks */
        ret = NANO_OS_WAIT_OBJECT_Signal(&flag_set->wait_object, NOS_ERR_SUCCESS, true, NANO_OS_FLAG_SET_WaitObjectWakeupCond, &woke_up_task_count);

        /* Unlock flag set against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(flag_set->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u)

/** \brief Set a flag mask in a flag set from an interrupt handler */
nano_os_error_t NANO_OS_FLAG_SET_SetFromIsr(nano_os_flag_set_t* const flag_set, const uint32_t mask)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((flag_set != NULL) && (flag_set->wait_object.type == WOT_FLAG_SET))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_FLAG_SET_SET_ISR, flag_set);

        /* Check if operation can be done */
        if (WAIT_OBJECT_ISR_ISLOCKED(flag_set->wait_object))
        {
            /* Fill interrupt service request */
            nano_os_isr_service_request_t request;
            request.service_func = NANO_OS_CAST(fp_nano_os_isr_func_t, NANO_OS_FLAG_SET_Set);
            request.object = flag_set;
            request.param1 = mask;

            /* Queue request */
            ret = NANO_OS_INTERRUPT_QueueRequest(&request);
        }
        else
        {
            /* No syscall in progress */
            ret = NANO_OS_FLAG_SET_Set(flag_set, mask);
        }
    }

    return ret;
}

#endif /* (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u) */


#if (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u)

/** \brief Clear a flag mask in a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Clear(nano_os_flag_set_t* const flag_set, const uint32_t mask)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((flag_set != NULL) && (flag_set->wait_object.type == WOT_FLAG_SET))
    {
        /* Real time trace event */
       NANO_OS_TRACE_OBJ(NOS_TRACE_FLAG_SET_CLEAR, flag_set);

       /* Lock flag set against interrupt handler access */
       WAIT_OBJECT_ISR_LOCK(flag_set->wait_object);

        /* Update wait mask */
        flag_set->mask &= ~mask;

        /* Unlock flag set against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(flag_set->wait_object);

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u)

/** \brief Clear a flag mask in a flag set from an interrupt handler */
nano_os_error_t NANO_OS_FLAG_SET_ClearFromIsr(nano_os_flag_set_t* const flag_set, const uint32_t mask)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((flag_set != NULL) && (flag_set->wait_object.type == WOT_FLAG_SET))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_FLAG_SET_CLEAR_ISR, flag_set);

        /* Check if operation can be done */
        if (WAIT_OBJECT_ISR_ISLOCKED(flag_set->wait_object))
        {
            /* Fill interrupt service request */
            nano_os_isr_service_request_t request;
            request.service_func = NANO_OS_CAST(fp_nano_os_isr_func_t, NANO_OS_FLAG_SET_Clear);
            request.object = flag_set;
            request.param1 = mask;

            /* Queue request */
            ret = NANO_OS_INTERRUPT_QueueRequest(&request);
        }
        else
        {
            /* No syscall in progress */
            ret = NANO_OS_FLAG_SET_Clear(flag_set, mask);
        }
    }

    return ret;
}

#endif /* (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u) */

#endif /* (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u) */


#if (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a flag set */
nano_os_error_t NANO_OS_FLAG_SET_SetName(nano_os_flag_set_t* const flag_set, const char* const name)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (flag_set != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_NAMING, flag_set);

        /* Save the name */
        flag_set->wait_object.name = name;

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u) */



/** \brief Prototype for the OS object specific action on wait object signal */
static bool NANO_OS_FLAG_SET_WaitObjectWakeupCond(nano_os_wait_object_t* const wait_object, nano_os_task_t* task)
{
    bool ret = false;
    nano_os_flag_set_t* const flag_set = NANO_OS_CAST(nano_os_flag_set_t*, wait_object);

    /* Check parameters */
    if ((flag_set != NULL) && (task != NULL) && (flag_set->wait_object.type == WOT_FLAG_SET))
    {
        const bool wait_all = task->flag_set_wait_all;
        const uint32_t wait_mask = task->flag_set_wait_mask;

        /* Check if the flags are matching */
        if ((wait_all && ((flag_set->mask & wait_mask) == wait_mask)) ||
            (!wait_all && ((flag_set->mask & wait_mask) != 0u)))
        {
            /* Save flags which did trigger the task wakeup */
            task->flag_set_wait_mask = flag_set->mask & wait_mask;
            ret = true;
        }
    }

    return ret;
}




#endif /* (NANO_OS_FLAG_SET_ENABLED == 1u) */
