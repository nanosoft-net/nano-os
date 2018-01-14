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

#ifndef UART_H
#define UART_H


#include "nano_os_types.h"



/** \brief Initialize the UART driver */
void UART_Init(void);

/** \brief Send data over the UART */
void UART_Send(const uint8_t* data, uint32_t data_len);

/** \brief Receive data on the UART */
void UART_Receive(uint8_t* data, uint32_t data_len);


#endif /* UART_H */
