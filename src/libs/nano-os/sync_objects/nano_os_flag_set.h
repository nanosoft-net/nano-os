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

#ifndef NANO_OS_FLAG_SET_H
#define NANO_OS_FLAG_SET_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_FLAG_SET_ENABLED == 1u)


#include "nano_os_wait_object.h"
#include "nano_os_interrupt.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Nano OS flag set */
typedef struct _nano_os_flag_set_t
{
    /** \brief Underlying wait object */
    nano_os_wait_object_t wait_object;

    /** \brief Current flag mask */
    uint32_t mask;

} nano_os_flag_set_t;




/** \brief Create a new flag set */
nano_os_error_t NANO_OS_FLAG_SET_Create(nano_os_flag_set_t* const flag_set, const uint32_t initial_mask,
                                        const nano_os_queuing_type_t queuing_type);

#if (NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u)

/** \brief Destroy a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Destroy(nano_os_flag_set_t* const flag_set);

#endif /* (NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u) */

/** \brief Wait for a specific flag mask in a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Wait(nano_os_flag_set_t* const flag_set, const uint32_t wait_mask,
                                      const bool wait_all, const bool reset_flags,
                                      uint32_t* const wakeup_flags, const uint32_t tick_count);

/** \brief Set a flag mask in a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Set(nano_os_flag_set_t* const flag_set, const uint32_t mask);

#if (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u)

/** \brief Set a flag mask in a flag set from an interrupt handler */
nano_os_error_t NANO_OS_FLAG_SET_SetFromIsr(nano_os_flag_set_t* const flag_set, const uint32_t mask);

#endif /* (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u) */

#if (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u)

/** \brief Clear a flag mask in a flag set */
nano_os_error_t NANO_OS_FLAG_SET_Clear(nano_os_flag_set_t* const flag_set, const uint32_t mask);

#if (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u)

/** \brief Clear a flag mask in a flag set from an interrupt handler */
nano_os_error_t NANO_OS_FLAG_SET_ClearFromIsr(nano_os_flag_set_t* const flag_set, const uint32_t mask);

#endif /* (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u) */

#endif /* (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u) */

#if (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a flag set */
nano_os_error_t NANO_OS_FLAG_SET_SetName(nano_os_flag_set_t* const flag_set, const char* const name);

#endif /* (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u) */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_FLAG_SET_ENABLED == 1u) */

#endif /* NANO_OS_FLAG_SET_H */
