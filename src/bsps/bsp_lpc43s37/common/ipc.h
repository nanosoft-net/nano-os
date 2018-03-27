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

#ifndef IPC_H
#define IPC_H


#include "nano_os_types.h"



/** \brief Initialize the IPC driver */
void IPC_Init(void);

/** \brief Send message over IPC */
bool IPC_SendMessage(const uint16_t command, const void* const data, const size_t data_size);

/** \brief Receive message over IPC */
bool IPC_ReceiveMessage(uint16_t* const command, void* const data, size_t* data_size, const uint32_t ms_timeout);


#endif /* IPC_H */
