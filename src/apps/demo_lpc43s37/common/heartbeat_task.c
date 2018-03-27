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

#include "heartbeat_task.h"
#include "ipc.h"
#include "ipc_task.h"
#include "defect.h"
#include "nano_os_tools.h"

/** \brief Heartbeat period in milliseconds */
#define HEARTBEAT_PERIOD    500u


/** \brief Stack size in number of elements */
#define TASK_STACK_SIZE     128u



/** \brief Heartbeat task stack */
static nano_os_stack_t heartbeat_task_stack[TASK_STACK_SIZE];

/** \brief Heartbeat task context */
static nano_os_task_t heartbeat_task;

/** \brief Heartbeat task mutex */
static nano_os_mutex_t heartbeat_task_mutex;

/** \brief Last received heartbeat message */
static ipc_msg_heartbeat_t last_heartbeat_msg;

/** \brief Last received heartbeat message timestamp */
static uint32_t last_heartbeat_msg_timestamp;


/** \brief Heartbeat task */
static void* HEARTBEAT_TASK_Task(void* param);

/** \brief IPC heartbeat message handler */
static void HEARTBEAT_TASK_IPCMessageHandler(const uint16_t cmd, const ipc_msg_t* const ipc_msg);

/** \brief Handle the 'heartbeat' console command */
static void HEARTBEAT_TASK_HeartbeatCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);


/** \brief Heartbeat task console commands */
static const nano_os_console_cmd_desc_t heartbeat_task_commands[] = {
                                                                    {"heartbeat", "Display informations about heartbeat exchange with other Cortex-M core", HEARTBEAT_TASK_HeartbeatCmdHandler}
                                                              };
/** \brief Console command group for the heartbeat task */
static nano_os_console_cmd_group_desc_t heartbeat_task_commands_group;








/** \brief Create the heartbeat task */
nano_os_error_t HEARTBEAT_TASK_Init(void)
{
    nano_os_error_t ret;
    nano_os_task_init_data_t task_init_data;
    (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));

    /* Create the task */
    task_init_data.name = "Heartbeat task";
    task_init_data.base_priority = 2u;
    task_init_data.stack_origin = heartbeat_task_stack;
    task_init_data.stack_size = TASK_STACK_SIZE;
    task_init_data.task_func = HEARTBEAT_TASK_Task;
    task_init_data.param = NULL;
    #if CORE_M4
    task_init_data.port_init_data.is_priviledged = true;
    #endif /* CORE_M4 */
    ret = NANO_OS_TASK_Create(&heartbeat_task, &task_init_data);

    /* Register commands to the console module */
    if (ret == NOS_ERR_SUCCESS)
    {
        heartbeat_task_commands_group.command_count = sizeof(heartbeat_task_commands) / sizeof(nano_os_console_cmd_desc_t);
        heartbeat_task_commands_group.commands = heartbeat_task_commands;
        heartbeat_task_commands_group.user_data = NULL;
        heartbeat_task_commands_group.next = NULL;
        ret = NANO_OS_CONSOLE_RegisterCommands(&heartbeat_task_commands_group);
    }

    return ret;
}


/** \brief Heartbeat task */
static void* HEARTBEAT_TASK_Task(void* param)
{
    bool ret;
    ipc_msg_heartbeat_t heartbeat_msg;
    nano_os_waitable_timer_t wait_timer;
    NANO_OS_UNUSED(param);


    /* Init */
    NANO_OS_MUTEX_Create(&heartbeat_task_mutex, QT_PRIORITY);
    NANO_OS_WAITABLE_TIMER_Create(&wait_timer, QT_PRIORITY);

    NANO_OS_GetVersion(&heartbeat_msg.version, &heartbeat_msg.port_version);
    (void)STRNCPY(heartbeat_msg.port_name, NANO_OS_PORT_NAME, sizeof(heartbeat_msg.port_name));

    IPC_TASK_RegisterMessageHandler(IPC_MSG_HEARTBEAT, HEARTBEAT_TASK_IPCMessageHandler);

    /* Task loop */
    NANO_OS_WAITABLE_TIMER_Start(&wait_timer, 1u, NANO_OS_MS_TO_TICKS(HEARTBEAT_PERIOD));
    while(true)
    {
        /* Wait for the next heartbeat */
        NANO_OS_WAITABLE_TIMER_Wait(&wait_timer, 0xFFFFFFFFu);

        /* Send heartbeat message */
        NANO_OS_GetTickCount(&heartbeat_msg.up_time);
        ret = IPC_SendMessage(IPC_MSG_HEARTBEAT, &heartbeat_msg, sizeof(ipc_msg_heartbeat_t));
        if (!ret)
        {
            /* IPC Tx error */
            DEFECT_Raise(DEF_IPC_TX);
        }
        else
        {
            DEFECT_Clear(DEF_IPC_TX);
        }

        /* Check other CPU heartbeat */
        if ((heartbeat_msg.up_time - last_heartbeat_msg_timestamp) > (2u * NANO_OS_MS_TO_TICKS(HEARTBEAT_PERIOD)))
        {
            /* Distant heartbeat error */
            DEFECT_Raise(DEF_HEARTBEAT);
        }
        else
        {
            /* Heartbeat received */
            DEFECT_Clear(DEF_HEARTBEAT);
        }
    }

    return NULL;
}

/** \brief IPC heartbeat message handler */
static void HEARTBEAT_TASK_IPCMessageHandler(const uint16_t cmd, const ipc_msg_t* const ipc_msg)
{
    NANO_OS_UNUSED(cmd);

    /* Save received heartbeat message */
    NANO_OS_MUTEX_Lock(&heartbeat_task_mutex, 0xFFFFFFFFu);

    NANO_OS_GetTickCount(&last_heartbeat_msg_timestamp);
    (void)MEMCPY(&last_heartbeat_msg, &ipc_msg->heartbeat, sizeof(ipc_msg_heartbeat_t));

    NANO_OS_MUTEX_Unlock(&heartbeat_task_mutex);
}

/** \brief Handle the 'heartbeat' console command */
static void HEARTBEAT_TASK_HeartbeatCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);
    NANO_OS_UNUSED(params);

    if (last_heartbeat_msg_timestamp == 0u)
    {
        NANO_OS_USER_ConsoleWriteString("Heartbeat infos: none (no heartbeat received from distant cpu)\r\n");
    }
    else
    {
        uint32_t now;
        ipc_msg_heartbeat_t heartbeat_msg;
        static char tmp[128u];

        NANO_OS_MUTEX_Lock(&heartbeat_task_mutex, 0xFFFFFFFFu);
        MEMCPY(&heartbeat_msg, &last_heartbeat_msg, sizeof(ipc_msg_heartbeat_t));
        NANO_OS_MUTEX_Unlock(&heartbeat_task_mutex);

        NANO_OS_USER_ConsoleWriteString("Heartbeat infos:\r\n");

        NANO_OS_GetTickCount(&now);
        SNPRINTF(tmp, sizeof(tmp), "- last heartbeat received: %lums\r\n", (now - last_heartbeat_msg_timestamp));
        NANO_OS_USER_ConsoleWriteString(tmp);

        SNPRINTF(tmp, sizeof(tmp), "- NanoOs version on distant cpu: %u.%u\r\n", heartbeat_msg.version.major, heartbeat_msg.version.minor);
        NANO_OS_USER_ConsoleWriteString(tmp);
        SNPRINTF(tmp, sizeof(tmp), "- NanoOs port version on distant cpu: %u.%u\r\n", heartbeat_msg.port_version.major, heartbeat_msg.port_version.minor);
        NANO_OS_USER_ConsoleWriteString(tmp);
        NANO_OS_USER_ConsoleWriteString("- NanoOS port name on distant cpu:");
        NANO_OS_USER_ConsoleWriteString(heartbeat_msg.port_name);
        NANO_OS_USER_ConsoleWriteString("\r\n");
    }
}


