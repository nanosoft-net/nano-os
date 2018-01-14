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

#ifndef NANO_OS_TRACE_H
#define NANO_OS_TRACE_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_TRACE_ENABLED == 1u)

/** \brief Macro to add a trace entry */
#define NANO_OS_TRACE_ADD(event_type, event_id)         NANO_OS_TRACE_Add((event_type), (event_id))

/** \brief Macro to add a trace entry for a Nano-OS object */
#define NANO_OS_TRACE_OBJ(event_type, object)           NANO_OS_TRACE_ADD((event_type), (object)->wait_object.object_id)

/** \brief Macro to add a trace entry for a Nano-OS task */
#define NANO_OS_TRACE_TASK(event_type, task)            NANO_OS_TRACE_ADD((event_type), (task)->object_id)

/** \brief Macro to trace an interrupt entry */
#define NANO_OS_TRACE_INTERRUPT_ENTRY(interrupt_id)    NANO_OS_TRACE_ADD(NOS_TRACE_INTERRUPT_ENTRY, (interrupt_id))

/** \brief Macro to trace an interrupt exit */
#define NANO_OS_TRACE_INTERRUPT_EXIT(interrupt_id)    NANO_OS_TRACE_ADD(NOS_TRACE_INTERRUPT_EXIT, (interrupt_id))

/** \brief Macro to trace user event */
#define NANO_OS_TRACE_USER(user_event_type, user_event_id)    NANO_OS_TRACE_ADD(NANO_OS_CAST(uint8_t, (NOS_TRACE_USER_EVENT_0 + (user_event_type & 0x1Fu))), \
                                                                                (user_event_id))


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Trace event types */
typedef enum _nano_os_trace_event_type_t
{
    /* Generic events */

    /** \brief Trace start */
    NOS_TRACE_START = 0u,
    /** \brief Object creation */
    NOS_TRACE_OBJ_CREATE = 1u,
    /** \brief Object destruction */
    NOS_TRACE_OBJ_DESTROY = 2u,
    /** \brief Object naming */
    NOS_TRACE_OBJ_NAMING = 3u,


    /* Task events */
    NOS_TRACE_TASK_START = 10u,
    NOS_TRACE_TASK_SLEEP = 11u,
    NOS_TRACE_TASK_JOIN = 12u,

    /* Semaphore events */
    NOS_TRACE_SEMAPHORE_WAIT = 20u,
    NOS_TRACE_SEMAPHORE_POST = 21u,
    NOS_TRACE_SEMAPHORE_POST_ISR = 22u,
    NOS_TRACE_SEMAPHORE_GETCOUNT = 23u,

    /* Mutex events */
    NOS_TRACE_MUTEX_LOCK = 30u,
    NOS_TRACE_MUTEX_TRYLOCK = 31u,
    NOS_TRACE_MUTEX_UNLOCK = 32u,

    /* Flag set events */
    NOS_TRACE_FLAG_SET_WAIT = 40u,
    NOS_TRACE_FLAG_SET_SET = 41u,
    NOS_TRACE_FLAG_SET_SET_ISR = 42u,
    NOS_TRACE_FLAG_SET_CLEAR = 43u,
    NOS_TRACE_FLAG_SET_CLEAR_ISR = 44u,

    /* Mailbox events */
    NOS_TRACE_MAILBOX_WAIT = 50u,
    NOS_TRACE_MAILBOX_POST = 51u,
    NOS_TRACE_MAILBOX_POST_ISR = 52u,
    NOS_TRACE_MAILBOX_GETCOUNT = 53u,

    /* Condition variable events */
    NOS_TRACE_COND_VAR_WAIT = 60u,
    NOS_TRACE_COND_VAR_SIGNAL = 61u,
    NOS_TRACE_COND_VAR_SIGNAL_ISR = 62u,

    /* Timer events */
    NOS_TRACE_TIMER_START = 70u,
    NOS_TRACE_TIMER_STOP = 71u,

    /* Waitable timer events */
    NOS_TRACE_WAITABLE_TIMER_START = 80u,
    NOS_TRACE_WAITABLE_TIMER_STOP = 81u,
    NOS_TRACE_WAITABLE_TIMER_WAIT = 82u,


    /* Scheduler events */
    NOS_TRACE_SCHEDULER_START = 100u,
    NOS_TRACE_SCHEDULER_CONTEXT_SWITCH_ON = 101u,
    NOS_TRACE_SCHEDULER_CONTEXT_SWITCH_OFF = 102u,

    /* Interrupt events */
    NOS_TRACE_INTERRUPT_ENTRY = 110u,
    NOS_TRACE_INTERRUPT_EXIT = 111u,

    /* User events */
    NOS_TRACE_USER_EVENT_0 = 200u,
    NOS_TRACE_USER_EVENT_31 = 231u

} nano_os_trace_event_type_t;





/** \brief Initialize the trace module */
nano_os_error_t NANO_OS_TRACE_Init(void);

/** \brief Add a trace entry */
void NANO_OS_TRACE_Add(const uint8_t event_type, const uint16_t event_id);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#else

/** \brief Macro to add a trace entry */
#define NANO_OS_TRACE_ADD(event_type, event_id)

/** \brief Macro to add a trace entry for a Nano-OS object */
#define NANO_OS_TRACE_OBJ(event_type, object)

/** \brief Macro to add a trace entry for a Nano-OS task */
#define NANO_OS_TRACE_TASK(event_type, task)

/** \brief Macro to trace an interrupt entry */
#define NANO_OS_TRACE_INTERRUPT_ENTRY(interrupt_id)

/** \brief Macro to trace an interrupt exit */
#define NANO_OS_TRACE_INTERRUPT_EXIT(interrupt_id)


#endif /* (NANO_OS_TRACE_ENABLED == 1u) */

#endif /* NANO_OS_TRACE_H */
