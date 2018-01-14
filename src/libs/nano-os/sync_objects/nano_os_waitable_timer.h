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

#ifndef NANO_OS_WAITABLETIMER_H
#define NANO_OS_WAITABLETIMER_H

#include "nano_os_timer.h"

/* Check if module is enabled */
#if (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Nano OS waitable timer */
typedef struct _nano_os_waitable_timer_t
{
    /** \brief Underlying wait object */
    nano_os_wait_object_t wait_object;
    /** \brief Underlying timer */
    nano_os_timer_t timer;
} nano_os_waitable_timer_t;



/** \brief Create a new waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Create(nano_os_waitable_timer_t* const timer, const nano_os_queuing_type_t queuing_type);

#if (NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u)

/** \brief Destroy a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Destroy(nano_os_waitable_timer_t* const timer);

#endif /* (NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u) */

/** \brief Start a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Start(nano_os_waitable_timer_t* const timer, const uint32_t first_due_time, const uint32_t period);

/** \brief Stop a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Stop(nano_os_waitable_timer_t* const timer);

/** \brief Wait for a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_Wait(nano_os_waitable_timer_t* const timer, const uint32_t tick_count);

#if (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a waitable timer */
nano_os_error_t NANO_OS_WAITABLE_TIMER_SetName(nano_os_waitable_timer_t* const timer, const char* const name);

#endif /* (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u) */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_WAITABLE_TIMER_ENABLED == 1u) */

#endif /* NANO_OS_WAITABLETIMER_H */
