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

#ifndef NANO_OS_INTERRUPT_H
#define NANO_OS_INTERRUPT_H

#include "nano_os_types.h"
#include "nano_os_port.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Prototype for functions to execute in the interrupt service thread */
typedef nano_os_error_t (*fp_nano_os_isr_func_t)(void* const object, const uint32_t param1, const uint32_t param2);

/** \brief Prototype for error callbacks */
typedef void (*fp_nano_os_isr_error_func_t)(const nano_os_error_t error);


/** \brief Interrupt service request */
typedef struct _nano_os_isr_service_request_t
{
    /** \brief Function to execute */
    fp_nano_os_isr_func_t service_func;
    /** \brief Nano OS objects*/
    void* object;
    /** \brief First parameter */
    uint32_t param1;
    /** \brief Second parameter */
    uint32_t param2;
} nano_os_isr_service_request_t;



/** \brief Initialize the interrupt service module */
nano_os_error_t NANO_OS_INTERRUPT_Init(const nano_os_port_init_data_t* const port_init_data);

/** \brief Queue an interrupt service request */
nano_os_error_t NANO_OS_INTERRUPT_QueueRequest(nano_os_isr_service_request_t* const isr_request);


/** \brief Manage the entry into an interrupt handler */
void NANO_OS_INTERRUPT_Enter(void);

/** \brief Manage the exit of an interrupt handler */
void NANO_OS_INTERRUPT_Exit(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_INTERRUPT_H */
