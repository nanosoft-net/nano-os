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

#ifndef NANO_OS_PORT_USER_H
#define NANO_OS_PORT_USER_H


#include "nano_os_types.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void);


/** \brief Get port specific global MPU configuration */
nano_os_error_t NANO_OS_PORT_USER_GetGlobalMpuConfig(nano_os_port_mpu_region_t* const mpu_regions);

/** \brief Get idle task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetIdleTaskConfig(nano_os_port_task_init_data_t* const port_init_data, nano_os_stack_t** stack_origin);

/** \brief Get interrupt service request task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetInterruptServiceRequestTaskConfig(nano_os_port_task_init_data_t* const port_init_data, nano_os_stack_t** stack_origin);

#if (NANO_OS_TIMER_ENABLED == 1u)
/** \brief Get timer task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetTimerTaskConfig(nano_os_port_task_init_data_t* const port_init_data, nano_os_stack_t** stack_origin);
#endif /* (NANO_OS_TIMER_ENABLED == 1u) */

#if (NANO_OS_CONSOLE_ENABLED == 1u)
/** \brief Get console task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetConsoleTaskConfig(nano_os_port_task_init_data_t* const port_init_data);
#endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */

#if (NANO_OS_DEBUG_ENABLED == 1u)
/** \brief Get debug task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetDebugTaskConfig(nano_os_port_task_init_data_t* const port_init_data);
#endif /* (NANO_OS_DEBUG_ENABLED == 1u) */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_PORT_USER_H */
