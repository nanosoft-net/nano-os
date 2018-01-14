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

#include "nano_os_semaphore.h"

/* Check if module is enabled */
#if (NANO_OS_SEMAPHORE_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_trace.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"




/** \brief Create a new semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Create(nano_os_semaphore_t* const semaphore, const uint32_t initial_count,
                                         const uint32_t max_count, const nano_os_queuing_type_t queuing_type)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if ((semaphore != NULL) && (initial_count <= max_count) &&
        (max_count != 0u))
    {
        /* 0 init of semaphore */
        (void)MEMSET(semaphore, 0, sizeof(nano_os_semaphore_t));

        /* Init underlying wait object */
        ret = NANO_OS_WAIT_OBJECT_Initialize(&semaphore->wait_object, WOT_SEMAPHORE, queuing_type);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Initialize semaphore data */
            semaphore->count = initial_count;
            semaphore->max_count = max_count;

            /* Real time trace event */
            NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_CREATE, semaphore);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u)

/** \brief Destroy a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Destroy(nano_os_semaphore_t* const semaphore)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((semaphore != NULL) && (semaphore->wait_object.type == WOT_SEMAPHORE))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_DESTROY, semaphore);

        /* Destroy wait object */
        ret = NANO_OS_WAIT_OBJECT_Destroy(&semaphore->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u) */


/** \brief Post a token to a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Wait(nano_os_semaphore_t* const semaphore, const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((semaphore != NULL) && (semaphore->wait_object.type == WOT_SEMAPHORE))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_SEMAPHORE_WAIT, semaphore);

        /* Lock semaphore against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(semaphore->wait_object);

        /* Check if a token is present */
        if (semaphore->count != 0u)
        {
            /* Update semaphore count */
            semaphore->count--;
            ret = NOS_ERR_SUCCESS;
        }
        else
        {
            /* Wait for a token */
            ret = NANO_OS_WAIT_OBJECT_Wait(&semaphore->wait_object, tick_count);
        }

        /* Unlock semaphore against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(semaphore->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


/** \brief Wait post a token in a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Post(nano_os_semaphore_t* const semaphore)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((semaphore != NULL) && (semaphore->wait_object.type == WOT_SEMAPHORE))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_SEMAPHORE_POST, semaphore);

        /* Lock semaphore against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(semaphore->wait_object);

        /* Check if the semaphore is full */
        if (semaphore->count == semaphore->max_count)
        {
            /* Full */
            ret = NOS_ERR_SEMAPHORE_FULL;
        }
        else
        {
            /* Signal waiting tasks */
            uint32_t woke_up_task_count = 0u;
            ret = NANO_OS_WAIT_OBJECT_Signal(&semaphore->wait_object, NOS_ERR_SUCCESS, false, NULL, &woke_up_task_count);
            if (ret == NOS_ERR_SUCCESS)
            {
                /* Check if the token has been consumed by a task */
                if (woke_up_task_count == 0u)
                {
                    /* No task was waiting on the semaphore, update count */
                    semaphore->count++;
                }
            }
        }

        /* Unlock semaphore against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(semaphore->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u)

/** \brief Post a token to a semaphore from an interrupt handler */
nano_os_error_t NANO_OS_SEMAPHORE_PostFromIsr(nano_os_semaphore_t* const semaphore)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((semaphore != NULL) && (semaphore->wait_object.type == WOT_SEMAPHORE))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_SEMAPHORE_POST_ISR, semaphore);

        /* Check if operation can be done */
        if (WAIT_OBJECT_ISR_ISLOCKED(semaphore->wait_object))
        {
            /* Fill interrupt service request */
            nano_os_isr_service_request_t request;
            request.service_func = NANO_OS_CAST(fp_nano_os_isr_func_t, NANO_OS_SEMAPHORE_Post);
            request.object = semaphore;

            /* Queue request */
            ret = NANO_OS_INTERRUPT_QueueRequest(&request);
        }
        else
        {
            /* No syscall in progress */
            ret = NANO_OS_SEMAPHORE_Post(semaphore);
        }
    }

    return ret;
}

#endif /* (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u) */


#if (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u)

/** \brief Get the current count of a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_GetCount(nano_os_semaphore_t* const semaphore, uint32_t* const count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((semaphore != NULL) && (count != NULL) && (semaphore->wait_object.type == WOT_SEMAPHORE))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_SEMAPHORE_GETCOUNT, semaphore);

        /* Lock semaphore against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(semaphore->wait_object);

        /* Copy current count */
        (*count) = semaphore->count;

        /* Unlock semaphore against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(semaphore->wait_object);

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u) */


#if (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_SetName(nano_os_semaphore_t* const semaphore, const char* const name)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (semaphore != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_NAMING, semaphore);

        /* Save the name */
        semaphore->wait_object.name = name;

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u) */


#endif /* (NANO_OS_SEMAPHORE_ENABLED == 1u) */
