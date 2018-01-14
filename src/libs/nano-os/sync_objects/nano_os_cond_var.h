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

#ifndef NANO_OS_COND_VAR_H
#define NANO_OS_COND_VAR_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_COND_VAR_ENABLED == 1u)

#include "nano_os_wait_object.h"
#include "nano_os_mutex.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Nano OS condition variable */
typedef struct _nano_os_cond_var_t
{
    /** \brief Underlying wait object */
    nano_os_wait_object_t wait_object;

} nano_os_cond_var_t;




/** \brief Create a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Create(nano_os_cond_var_t* const cond_var, const nano_os_queuing_type_t queuing_type);

#if (NANO_OS_COND_VAR_DESTROY_ENABLED == 1u)

/** \brief Destroy a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Destroy(nano_os_cond_var_t* const cond_var);

#endif /* (NANO_OS_COND_VAR_DESTROY_ENABLED == 1u) */

/** \brief Wait for a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Wait(nano_os_cond_var_t* const cond_var, nano_os_mutex_t* const mutex, const uint32_t tick_count);

/** \brief Signal a condition variable */
nano_os_error_t NANO_OS_COND_VAR_Signal(nano_os_cond_var_t* const cond_var, const bool broadcast);

#if (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u)

/** \brief Signal a condition variable from an interrupt handler */
nano_os_error_t NANO_OS_COND_VAR_SignalFromIsr(nano_os_cond_var_t* const cond_var, const bool broadcast);

#endif /* (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u) */

#if (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a condition variable */
nano_os_error_t NANO_OS_COND_VAR_SetName(nano_os_cond_var_t* const cond_var, const char* const name);

#endif /* (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u) */





#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_COND_VAR_ENABLED == 1u) */

#endif /* NANO_OS_COND_VAR_H */
