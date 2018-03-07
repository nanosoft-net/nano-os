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

#include "ipc_task.h"
#include "ipc.h"
#include "defect.h"


/** \brief Stack size in number of elements */
#define TASK_STACK_SIZE     256u



/** \brief IPC task stack */
static nano_os_stack_t ipc_task_stack[TASK_STACK_SIZE];

/** \brief IPC task context */
static nano_os_task_t ipc_task;

/** \brief IPC message handlers */
static fp_ipc_msg_handler_func_t ipc_msg_handlers[IPC_MSG_MAX];


/** \brief IPC task */
static void* IPC_TASK_Task(void* param);



/** \brief Create the IPC task */
nano_os_error_t IPC_TASK_Init(void)
{
    nano_os_error_t ret;
    nano_os_task_init_data_t task_init_data;
    (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));

    /* Create the task */
    task_init_data.name = "IPC task";
    task_init_data.base_priority = 7u;
    task_init_data.stack_origin = ipc_task_stack;
    task_init_data.stack_size = TASK_STACK_SIZE;
    task_init_data.task_func = IPC_TASK_Task;
    task_init_data.param = NULL;
    #ifdef CORE_M4
    task_init_data.port_init_data.is_priviledged = true;
    #endif /* CORE_M4 */
    ret = NANO_OS_TASK_Create(&ipc_task, &task_init_data);

    return ret;
}

/** \brief Register a handler for an IPC message */
bool IPC_TASK_RegisterMessageHandler(const uint16_t cmd, fp_ipc_msg_handler_func_t handler)
{
    bool ret = false;

    /* Check parameters */
    if ((cmd < IPC_MSG_MAX) && (ipc_msg_handlers[cmd] == NULL))
    {
        ipc_msg_handlers[cmd] = handler;
        ret = true;
    }

    return ret;
}



/** \brief IPC task */
static void* IPC_TASK_Task(void* param)
{
    uint16_t command;
    ipc_msg_t ipc_msg;
    NANO_OS_UNUSED(param);


    /* Init */
    IPC_Init();

    /* Task loop */
    while(true)
    {
        /* Wait for a message */
        size_t msg_size = sizeof(ipc_msg);
        if (IPC_ReceiveMessage(&command, &ipc_msg, &msg_size, 4000000000u))
        {
            /* Handle message */
            if ((command < IPC_MSG_MAX) && (ipc_msg_handlers[command] != NULL))
            {
                ipc_msg_handlers[command](command, &ipc_msg);
                DEFECT_Clear(DEF_IPC_RX);
            }
            else
            {
                DEFECT_Raise(DEF_IPC_RX);
            }
        }
    }

    return NULL;
}

