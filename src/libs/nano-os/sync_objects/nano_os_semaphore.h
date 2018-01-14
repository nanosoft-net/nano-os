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

#ifndef NANO_OS_SEMAPHORE_H
#define NANO_OS_SEMAPHORE_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_SEMAPHORE_ENABLED == 1u)


#include "nano_os_wait_object.h"
#include "nano_os_interrupt.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Nano OS semaphore */
typedef struct _nano_os_semaphore_t
{
    /** \brief Underlying wait object */
    nano_os_wait_object_t wait_object;

    /** \brief Current count */
    uint32_t count;
    /** \brief Max count */
    uint32_t max_count;
} nano_os_semaphore_t;




/** \brief Create a new semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Create(nano_os_semaphore_t* const semaphore, const uint32_t initial_count,
                                         const uint32_t max_count, const nano_os_queuing_type_t queuing_type);

#if (NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u)

/** \brief Destroy a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Destroy(nano_os_semaphore_t* const semaphore);

#endif /* (NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u) */

/** \brief Wait for a token in a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Wait(nano_os_semaphore_t* const semaphore, const uint32_t tick_count);

/** \brief Post a token to a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_Post(nano_os_semaphore_t* const semaphore);

#if (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u)

/** \brief Post a token to a semaphore from an interrupt handler */
nano_os_error_t NANO_OS_SEMAPHORE_PostFromIsr(nano_os_semaphore_t* const semaphore);

#endif /* (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u) */

#if (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u)

/** \brief Get the current count of a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_GetCount(nano_os_semaphore_t* const semaphore, uint32_t* const count);

#endif /* (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u) */

#if (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a semaphore */
nano_os_error_t NANO_OS_SEMAPHORE_SetName(nano_os_semaphore_t* const semaphore, const char* const name);

#endif /* (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u) */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_SEMAPHORE_ENABLED == 1u) */

#endif /* NANO_OS_SEMAPHORE_H */
