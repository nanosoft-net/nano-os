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

#ifndef NANO_OS_TIMER_H
#define NANO_OS_TIMER_H

#include "nano_os_types.h"
#include "nano_os_port.h"

/* Check if module is enabled */
#if (NANO_OS_TIMER_ENABLED == 1u)


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Nano OS timer pre-declaration */
struct _nano_os_timer_t;


/** \brief Nano OS timer callback function */
typedef void (*fp_nano_os_timer_callback_func_t)(struct _nano_os_timer_t* const timer, void* const user_data);


/** \brief Nano OS timer */
typedef struct _nano_os_timer_t
{
    /** \brief Period */
    uint32_t period;
    /** \brief Timeout */
    uint32_t timeout;
    /** \brief Callback */
    fp_nano_os_timer_callback_func_t callback;
    /** \brief User data */
    void* user_data;
    /** \brief Next timer */
    struct _nano_os_timer_t* next;
    /** \brief Indicate if the timer has elapsed */
    bool has_elapsed;

    #if (NANO_OS_TRACE_ENABLED == 1u)
    /** \brief Global object id affected by Nano-OS */
    uint16_t object_id;
    #endif /* (NANO_OS_TRACE_ENABLED == 1u) */
} nano_os_timer_t;



/** \brief Initialize the timer module */
nano_os_error_t NANO_OS_TIMER_Init(const nano_os_port_init_data_t* const port_init_data);

/** \brief Create a new timer */
nano_os_error_t NANO_OS_TIMER_Create(nano_os_timer_t* const timer, const fp_nano_os_timer_callback_func_t callback, void* const user_data);

#if (NANO_OS_TIMER_DESTROY_ENABLED == 1u)

/** \brief Destroy a timer */
nano_os_error_t NANO_OS_TIMER_Destroy(nano_os_timer_t* const timer);

#endif /* (NANO_OS_TIMER_DESTROY_ENABLED == 1u) */

/** \brief Start a timer */
nano_os_error_t NANO_OS_TIMER_Start(nano_os_timer_t* const timer, const uint32_t first_due_time, const uint32_t period);

/** \brief Stop a timer */
nano_os_error_t NANO_OS_TIMER_Stop(nano_os_timer_t* const timer);

/** \brief Check if a timer has elapsed */
nano_os_error_t NANO_OS_TIMER_HasElapsed(nano_os_timer_t* const timer, bool* const has_elapsed);


/** \brief Timer tick handler */
nano_os_error_t NANO_OS_TIMER_TickHandler(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_TIMER_ENABLED == 1u) */

#endif /* NANO_OS_TIMER_H */
