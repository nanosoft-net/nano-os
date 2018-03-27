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


#ifndef IPC_MESSAGES_H
#define IPC_MESSAGES_H

#include "nano_os_api.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief IPC messages commands */
typedef enum _ipc_msg_cmd_t
{
    /** \brief Heartbeat */
    IPC_MSG_HEARTBEAT = 0u,


    /** \brief Number of IPC message commands */
    IPC_MSG_MAX
} ipc_msg_cmd_t;



/** \brief IPC heartbeat message data */
typedef struct _ipc_msg_heartbeat_t
{
    /** \brief Up time */
    uint32_t up_time;
    /** \brief Nano OS version */
    nano_os_version_t version;
    /** \brief Nano OS port version */
    nano_os_version_t port_version;
    /** \brief Port name */
    char port_name[20u];
} ipc_msg_heartbeat_t;





/** \brief IPC messages */
typedef union _ipc_msg_t
{
    /** \brief Heartbeat message */
    ipc_msg_heartbeat_t heartbeat;
} ipc_msg_t;




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IPC_MESSAGES_H */
