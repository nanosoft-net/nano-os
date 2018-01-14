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

#ifndef NANO_OS_TYPES_H
#define NANO_OS_TYPES_H

#include "nano_os_safe_cfg.h"
#include "nano_os_error.h"

/* Standard integer types*/
#if (NANO_OS_USE_STD_INT == 1u)
#include <stdint.h>
#endif /* (NANO_OS_USE_STD_INT == 1u) */

/* Standard bool type definition */
#if (NANO_OS_USE_STD_BOOL == 1u)
#include <stdbool.h>
#else
#define false   0
#define true    1
#endif /* (NANO_OS_USE_STD_BOOL == 1u) */


/* Port specific types */
#include "nano_os_port_types.h"


/* Standard size_t type definition */
#if (NANO_OS_USE_STD_SIZE_T == 1u)
#include <stddef.h>
#else
typedef uint32_t size_t;
#endif /* (NANO_OS_USE_STD_SIZE_T == 1u) */

/* NULL pointer */
#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif /* __cplusplus */
#endif /* NULL */


/** \brief Version */
typedef struct _nano_os_version_t
{
    /** \brief Major revision number */
    uint8_t major;
    /** \brief Minor revision number */
    uint8_t minor;
} nano_os_version_t;


/** \brief Macro to cast a value or an expression to another type */
#define NANO_OS_CAST(type, expr) ((type)(expr))

/** \brief Macro for unused parameters in functions */
#define NANO_OS_UNUSED(param) ((void)(param))




#endif /* NANO_OS_TYPES_H */
