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


/* 
    This file defines the common interface to all ports.
    Any modification on this port interface must be reported on ALL the port implementations.
*/

#ifndef NANO_OS_PORT_H
#define NANO_OS_PORT_H

#include "nano_os_types.h"
#include "nano_os_task.h"
#include "nano_os_port_macros.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Port specific data which must be initialized by the port */
typedef struct _nano_os_port_init_data_t
{
    #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)

    /** \brief Port specific data for the idle task */
    nano_os_port_task_init_data_t idle_task_init_data;
    /** \brief Port specific data for the ISR request task */
    nano_os_port_task_init_data_t isr_request_task_init_data;

    #if (NANO_OS_TIMER_ENABLED == 1)
    /** \brief Port specific data for the timer task */
    nano_os_port_task_init_data_t timer_task_init_data;
    #endif /* (NANO_OS_TIMER_ENABLED == 1) */

    #if (NANO_OS_CONSOLE_ENABLED == 1)
    /** \brief Port specific data for the console task */
    nano_os_port_task_init_data_t console_task_init_data;
    #endif /* (NANO_OS_CONSOLE_ENABLED == 1) */

    #if (NANO_OS_CONSOLE_ENABLED == 1)
    /** \brief Port specific data for the debug task */
    nano_os_port_task_init_data_t debug_task_init_data;
    #endif /* (NANO_OS_CONSOLE_ENABLED == 1) */


    #else

    /** \brief Dummy variable to not have an empty structure */
    uint8_t dummy;

    #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */

} nano_os_port_init_data_t;




/** \brief Port specific initialization */
nano_os_error_t NANO_OS_PORT_Init(nano_os_port_init_data_t* const port_init_data);

/** \brief Get the port version */
nano_os_error_t NANO_OS_PORT_GetVersion(nano_os_version_t* const port_version);


/** \brief Port specific initialization of the task context */
nano_os_error_t NANO_OS_PORT_InitTask(nano_os_task_t* const task, const nano_os_task_init_data_t* const task_init_data);


/** \brief Port specific first task context switch */
nano_os_error_t NANO_OS_PORT_FirstContextSwitch(void);

/** \brief Port specific task level context switch */
void NANO_OS_PORT_ContextSwitch(void);

/** \brief Port specific interrupt level context switch */
void NANO_OS_PORT_ContextSwitchFromIsr(void);

#if ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u))
#if (NANO_OS_PORT_PROVIDES_SYSTEM_TIMER == 1u)

/** \brief Get the current timestamp in µs */
uint32_t NANO_OS_PORT_GetTimestampInUs(void);

#else

/** \brief Get the current timestamp in µs */
#define NANO_OS_PORT_GetTimestampInUs() NANO_OS_USER_SystemTimerGetTimestampInUs()

#endif /* NANO_OS_PORT_PROVIDES_SYSTEM_TIMER */
#endif /* ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_PORT_H */
