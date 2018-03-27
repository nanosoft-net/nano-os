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


#ifndef IPC_TASK_H
#define IPC_TASK_H

#include "nano_os_api.h"
#include "ipc_messages.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** \brief IPC message handler function */
typedef void (*fp_ipc_msg_handler_func_t)(const uint16_t cmd, const ipc_msg_t* const ipc_msg);





/** \brief Create the IPC task */
nano_os_error_t IPC_TASK_Init(void);

/** \brief Register a handler for an IPC message */
bool IPC_TASK_RegisterMessageHandler(const uint16_t cmd, fp_ipc_msg_handler_func_t handler);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HEARTBEAT_TASK_H */
