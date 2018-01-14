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

#ifndef NANO_OS_PORT_VIRTUAL_INTERRUPT_H
#define NANO_OS_PORT_VIRTUAL_INTERRUPT_H

#include "nano_os_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Initialize the virtual interrupt module */
nano_os_error_t NANO_OS_PORT_VIRTUAL_INTERRUPT_Init(void);

/** \brief Triggers a virtual interrupt */
nano_os_error_t NANO_OS_PORT_VIRTUAL_INTERRUPT_Trigger(const fp_nano_os_virtual_interrupt_handler_t handler);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_PORT_VIRTUAL_INTERRUPT_H */
