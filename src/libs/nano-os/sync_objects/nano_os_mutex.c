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

#include "nano_os_mutex.h"

/* Check if module is enabled */
#if (NANO_OS_MUTEX_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_trace.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"
#include "nano_os_port.h"



/** \brief Prototype for the OS object specific action on wait object signal */
static bool NANO_OS_MUTEX_WaitObjectWakeupCond(nano_os_wait_object_t* const wait_object, nano_os_task_t* task);






/** \brief Create a new mutex */
nano_os_error_t NANO_OS_MUTEX_Create(nano_os_mutex_t* const mutex, const nano_os_queuing_type_t queuing_type)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (mutex != NULL)
    {
        /* 0 init of mutex */
        (void)MEMSET(mutex, 0, sizeof(nano_os_mutex_t));

        /* Init underlying wait object */
        ret = NANO_OS_WAIT_OBJECT_Initialize(&mutex->wait_object, WOT_MUTEX, queuing_type);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Real time trace event */
            NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_CREATE, mutex);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_MUTEX_DESTROY_ENABLED == 1u)

/** \brief Destroy a mutex */
nano_os_error_t NANO_OS_MUTEX_Destroy(nano_os_mutex_t* const mutex)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((mutex != NULL) && (mutex->wait_object.type == WOT_MUTEX))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_DESTROY, mutex);

        /* Destroy wait object */
        ret = NANO_OS_WAIT_OBJECT_Destroy(&mutex->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_MUTEX_DESTROY_ENABLED == 1u) */


/** \brief Lock a mutex */
nano_os_error_t NANO_OS_MUTEX_Lock(nano_os_mutex_t* const mutex, const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* mutex parameters */
    if ((mutex != NULL) && (mutex->wait_object.type == WOT_MUTEX))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_MUTEX_LOCK, mutex);

        /* Check if the mutex is own by another task */
        if (mutex->owner == NULL)
        {
            /* Mutex is free */
            mutex->owner = g_nano_os.current_task;
            mutex->lock_count++;
            ret = NOS_ERR_SUCCESS;
        }
        else
        {
            /* Check if we already own the mutex */
            if (mutex->owner == g_nano_os.current_task)
            {
                /* Increment the lock count */
                mutex->lock_count++;
                ret = NOS_ERR_SUCCESS;
            }
            else
            {
                /* Wait for the mutex to be free */
                ret = NANO_OS_WAIT_OBJECT_Wait(&mutex->wait_object, tick_count);
            }
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u)

/** \brief Try to lock a mutex */
nano_os_error_t NANO_OS_MUTEX_TryLock(nano_os_mutex_t* const mutex)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Try to lock the mutex without timeout */
    ret = NANO_OS_MUTEX_Lock(mutex, 0u);
    if (ret == NOS_ERR_TIMEOUT)
    {
        /* Mutex is locked by another task, convert return value */
        ret = NOS_ERR_MUTEX_LOCKED;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u) */


/** \brief Unlock a mutex */
nano_os_error_t NANO_OS_MUTEX_Unlock(nano_os_mutex_t* const mutex)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((mutex != NULL) && (mutex->wait_object.type == WOT_MUTEX))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_MUTEX_UNLOCK, mutex);

        /* Check if we are the mutex owner */
        if (mutex->owner != g_nano_os.current_task)
        {
            /* Error, a mutex can only be unlocked by its owning task */
            ret = NOS_ERR_MUTEX_NOT_OWNED;
        }
        else
        {
            /* Check the lock count */
            mutex->lock_count--;
            if (mutex->lock_count == 0u)
            {
                uint32_t woke_up_task_count = 0u;

                /* Mutex is free */
                mutex->owner = NULL;

                /* Signal waiting tasks */
                ret = NANO_OS_WAIT_OBJECT_Signal(&mutex->wait_object, NOS_ERR_SUCCESS, false, NANO_OS_MUTEX_WaitObjectWakeupCond, &woke_up_task_count);
            }
            else
            {
                /* Mutex is still locked by the task */
                ret = NOS_ERR_SUCCESS;
            }
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_MUTEX_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a mutex */
nano_os_error_t NANO_OS_MUTEX_SetName(nano_os_mutex_t* const mutex, const char* const name)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (mutex != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_NAMING, mutex);

        /* Save the name */
        mutex->wait_object.name = name;

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_MUTEX_SETNAME_ENABLED == 1u) */



/** \brief Prototype for the OS object specific action on wait object signal */
static bool NANO_OS_MUTEX_WaitObjectWakeupCond(nano_os_wait_object_t* const wait_object, nano_os_task_t* task)
{
    bool ret = false;
    nano_os_mutex_t* const mutex = NANO_OS_CAST(nano_os_mutex_t*, wait_object);

    /* Check parameters */
    if ((wait_object != NULL) && (task != NULL) && (mutex->wait_object.type == WOT_MUTEX))
    {
        /* Lock the mutex to the woke up task */
        mutex->owner = task;
        mutex->lock_count++;
        ret = true;
    }

    return ret;
}


#endif /* (NANO_OS_MUTEX_ENABLED == 1u) */
