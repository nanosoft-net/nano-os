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

#include "nano_os_cond_var.h"

/* Check if module is enabled */
#if (NANO_OS_COND_VAR_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_trace.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"
#include "nano_os_port.h"



/** \brief Create a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Create(nano_os_cond_var_t* const cond_var, const nano_os_queuing_type_t queuing_type)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (cond_var != NULL)
    {
        /* 0 init of condition variable */
        (void)MEMSET(cond_var, 0, sizeof(nano_os_cond_var_t));

        /* Init underlying wait object */
        ret = NANO_OS_WAIT_OBJECT_Initialize(&cond_var->wait_object, WOT_COND_VAR, queuing_type);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Real time trace event */
            NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_CREATE, cond_var);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_COND_VAR_DESTROY_ENABLED == 1u)

/** \brief Destroy a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Destroy(nano_os_cond_var_t* const cond_var)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((cond_var != NULL) && (cond_var->wait_object.type == WOT_COND_VAR))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_DESTROY, cond_var);

        /* Destroy wait object */
        ret = NANO_OS_WAIT_OBJECT_Destroy(&cond_var->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_COND_VAR_DESTROY_ENABLED == 1u) */


/** \brief Wait for a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Wait(nano_os_cond_var_t* const cond_var, nano_os_mutex_t* const mutex, const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* mutex parameters */
    if ((cond_var != NULL) && (mutex != NULL) && (cond_var->wait_object.type == WOT_COND_VAR))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_COND_VAR_WAIT, cond_var);

        /* Check if the mutex is own by the current task */
        if (mutex->owner == g_nano_os.current_task)
        {
            /* Check the lock count of the mutex */
            if (mutex->lock_count == 1u)
            {
                /* Unlock the mutex */
                ret = NANO_OS_MUTEX_Unlock(mutex);
                if (ret == NOS_ERR_SUCCESS)
                {
                    uint32_t timeout;

                    /* Lock condition variable against interrupt handler access */
                    WAIT_OBJECT_ISR_LOCK(cond_var->wait_object);

                    /* Wait for the condition variable to be signaled */
                    timeout = NANO_OS_PORT_ATOMIC_READ32(g_nano_os.tick_count) + tick_count;
                    ret = NANO_OS_WAIT_OBJECT_Wait(&cond_var->wait_object, timeout - NANO_OS_PORT_ATOMIC_READ32(g_nano_os.tick_count));

                    /* Unlock condition variable against interrupt handler access */
                    WAIT_OBJECT_ISR_UNLOCK(cond_var->wait_object);

                    if (ret == NOS_ERR_SUCCESS)
                    {
                        /* Lock the mutex */
                        ret = NANO_OS_MUTEX_Lock(mutex, timeout - NANO_OS_PORT_ATOMIC_READ32(g_nano_os.tick_count));
                    }
                }
            }
            else
            {
                /* Error => the mutex must be ready to unlock */
                ret = NOS_ERR_MUTEX_LOCK_COUNT;
            }
        }
        else
        {
            /* Error => the mutex must be locked by the current task */
            ret = NOS_ERR_MUTEX_NOT_OWNED;
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


/** \brief Signal a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Signal(nano_os_cond_var_t* const cond_var, const bool broadcast)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((cond_var != NULL) && (cond_var->wait_object.type == WOT_COND_VAR))
    {
        uint32_t woke_up_task_count = 0u;

        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_COND_VAR_SIGNAL, cond_var);

        /* Lock condition variable against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(cond_var->wait_object);

        /* Signal waiting tasks */
        ret = NANO_OS_WAIT_OBJECT_Signal(&cond_var->wait_object, NOS_ERR_SUCCESS, broadcast, NULL, &woke_up_task_count);

        /* Unlock condition variable against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(cond_var->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u)

/** \brief Signal a condition variable from an interrupt handler */
nano_os_error_t NANO_OS_COND_VAR_SignalFromIsr(nano_os_cond_var_t* const cond_var, const bool broadcast)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((cond_var != NULL) && (cond_var->wait_object.type == WOT_COND_VAR))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_COND_VAR_SIGNAL_ISR, cond_var);

        /* Check if operation can be done */
        if (WAIT_OBJECT_ISR_ISLOCKED(cond_var->wait_object))
        {
            /* Fill interrupt service request */
            nano_os_isr_service_request_t request;
            request.service_func = NANO_OS_CAST(fp_nano_os_isr_func_t, NANO_OS_COND_VAR_Signal);
            request.object = cond_var;
            request.param1 = NANO_OS_CAST(uint32_t, broadcast);

            /* Queue request */
            ret = NANO_OS_INTERRUPT_QueueRequest(&request);
        }
        else
        {
            /* No syscall in progress */
            ret = NANO_OS_COND_VAR_Signal(cond_var, broadcast);
        }
    }

    return ret;
}

#endif /* (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u) */


#if (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a condition variable */
nano_os_error_t NANO_OS_COND_VAR_SetName(nano_os_cond_var_t* const cond_var, const char* const name)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (cond_var != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_NAMING, cond_var);

        /* Save the name */
        cond_var->wait_object.name = name;

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u) */



#endif /* (NANO_OS_COND_VAR_ENABLED == 1u) */
