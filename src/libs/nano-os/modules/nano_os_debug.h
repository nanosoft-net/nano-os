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

#ifndef NANO_OS_DEBUG_H
#define NANO_OS_DEBUG_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_DEBUG_ENABLED == 1u)

#include "nano_os_port.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Debug module data */
typedef struct _nano_os_debug_module_t
{
    /** \brief Debug task stack */
    nano_os_stack_t debug_task_stack[NANO_OS_DEBUG_TASK_STACK_SIZE];
    /** \brief Debug task context */
    nano_os_task_t debug_task;

} nano_os_debug_module_t;



/** \brief Initialize the debug module */
nano_os_error_t NANO_OS_DEBUG_Init(const nano_os_port_init_data_t* const port_init_data);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_DEBUG_ENABLED == 1u) */

#endif /* NANO_OS_DEBUG_H */
