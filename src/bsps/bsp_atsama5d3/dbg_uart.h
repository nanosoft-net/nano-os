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


#ifndef DBG_UART_H
#define DBG_UART_H

#include "nano_os_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */




/** \brief Initialize the UART driver */
void DBG_UART_Init(void);

/** \brief Send data over the UART */
void DBG_UART_Send(const uint8_t* data, uint32_t data_len);

/** \brief Receive data on the UART */
void DBG_UART_Receive(uint8_t* data, uint32_t data_len);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DBG_UART_H */
