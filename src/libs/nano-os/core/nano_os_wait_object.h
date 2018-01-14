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

#ifndef NANO_OS_WAIT_OBJECT_H
#define NANO_OS_WAIT_OBJECT_H

#include "nano_os_types.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Lock a wait object against interrupt handler simultaneous access */
#define WAIT_OBJECT_ISR_LOCK(wait_object)   g_nano_os.isr_locked_wait_object = &(wait_object)

/** \brief Unlock a wait object against interrupt handler simultaneous access */
#define WAIT_OBJECT_ISR_UNLOCK(wait_object)   g_nano_os.isr_locked_wait_object = NULL

/** \brief Check if a wait object is locked against interrupt handler simultaneous access */
#define WAIT_OBJECT_ISR_ISLOCKED(wait_object)   (g_nano_os.isr_locked_wait_object == &(wait_object))


/** \brief Wait object type */
typedef enum _nano_os_wait_object_type_t
{
    /** \brief Invalid */
    WOT_INVALID = -1,
    /** \brief Not initialized */
    WOT_NOT_INIT = 0u,
    /** \brief Task */
    WOT_TASK = 1u,
    /** \brief Semaphore */
    WOT_SEMAPHORE = 2u,
    /** \brief Mutex */
    WOT_MUTEX = 3u,
    /** \brief Condition variable */
    WOT_COND_VAR = 4u,
    /** \brief Timer */
    WOT_TIMER = 5u,
    /** \brief Mailbox */
    WOT_MAILBOX = 6u,
    /** \brief Flag set */
    WOT_FLAG_SET = 7u
} nano_os_wait_object_type_t;


/** \brief Queuing type */
typedef enum _nano_os_queuing_type_t
{
    /** \brief FIFO => first task which has been waiting on the object will be the first to be signaled */
    QT_FIFO = 0u,
    /** \brief Priority => the highest priority task waiting on the object will be the first signaled */
    QT_PRIORITY = 1u
} nano_os_queuing_type_t;



/** \brief Task context pre-declaration */
struct _nano_os_task_t;


/** \brief Wait object */
typedef struct _nano_os_wait_object_t
{
    /** \brief Type */
    nano_os_wait_object_type_t type;
    /** \brief Queuing type */
    nano_os_queuing_type_t queuing;
    /** \brief Waiting tasks */
    struct _nano_os_task_t* waiting_tasks;

    #if (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u)
    /** \brief Next wait object in the global list */
    struct _nano_os_wait_object_t* next;
    #endif /* (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u) */

    #if (NANO_OS_TRACE_ENABLED == 1u)
    /** \brief Global object id affected by Nano-OS */
    uint16_t object_id;
    #endif /* (NANO_OS_TRACE_ENABLED == 1u) */

    #if (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u)
    const char* name;
    #endif /* (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u) */
} nano_os_wait_object_t;


/** \brief Prototype for the OS object specific action on wait object signal */
typedef bool (*fp_nano_os_wait_object_wakeup_cond_func_t)(nano_os_wait_object_t* const wait_object, struct _nano_os_task_t* task);




/** \brief Initialize a wait object */
nano_os_error_t NANO_OS_WAIT_OBJECT_Initialize(nano_os_wait_object_t* const wait_object, const nano_os_wait_object_type_t type,
                                               const nano_os_queuing_type_t queuing_type);

/** \brief Wait for the wait object to be signaled */
nano_os_error_t NANO_OS_WAIT_OBJECT_Wait(nano_os_wait_object_t* const wait_object, const uint32_t timeout);

/** \brief Signal the wait object */
nano_os_error_t NANO_OS_WAIT_OBJECT_Signal(nano_os_wait_object_t* const wait_object, const nano_os_error_t wait_status,
                                           const bool signal_all_tasks, const fp_nano_os_wait_object_wakeup_cond_func_t wakeup_cond_func,
                                           uint32_t* const woke_up_task_count);

#if (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u)

/** \brief Destroy a wait object */
nano_os_error_t NANO_OS_WAIT_OBJECT_Destroy(nano_os_wait_object_t* const wait_object);

#endif /* (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED == 1u) */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_WAIT_OBJECT_H */
