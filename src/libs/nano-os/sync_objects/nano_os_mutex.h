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

#ifndef NANO_OS_MUTEX_H
#define NANO_OS_MUTEX_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_MUTEX_ENABLED == 1u)

#include "nano_os_task.h"
#include "nano_os_wait_object.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Nano OS mutex */
typedef struct _nano_os_mutex_t
{
    /** \brief Underlying wait object */
    nano_os_wait_object_t wait_object;

    /** \brief Current owner */
    nano_os_task_t* owner;
    /** \brief Lock count */
    uint32_t lock_count;
} nano_os_mutex_t;




/** \brief Create a new mutex */
nano_os_error_t NANO_OS_MUTEX_Create(nano_os_mutex_t* const mutex, const nano_os_queuing_type_t queuing_type);

#if (NANO_OS_MUTEX_DESTROY_ENABLED == 1u)

/** \brief Destroy a mutex */
nano_os_error_t NANO_OS_MUTEX_Destroy(nano_os_mutex_t* const mutex);

#endif /* (NANO_OS_MUTEX_DESTROY_ENABLED == 1u) */

/** \brief Lock a mutex */
nano_os_error_t NANO_OS_MUTEX_Lock(nano_os_mutex_t* const mutex, const uint32_t tick_count);

#if (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u)

/** \brief Try to lock a mutex */
nano_os_error_t NANO_OS_MUTEX_TryLock(nano_os_mutex_t* const mutex);

#endif /* (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u) */

/** \brief Unlock a mutex */
nano_os_error_t NANO_OS_MUTEX_Unlock(nano_os_mutex_t* const mutex);

#if (NANO_OS_MUTEX_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a mutex */
nano_os_error_t NANO_OS_MUTEX_SetName(nano_os_mutex_t* const mutex, const char* const name);

#endif /* (NANO_OS_MUTEX_SETNAME_ENABLED == 1u) */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_MUTEX_ENABLED == 1u) */

#endif /* NANO_OS_MUTEX_H */
