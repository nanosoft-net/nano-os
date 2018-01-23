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

#include "nano_os_api.h"

/* Check if module is enabled */
#if (NANO_OS_CONSOLE_ENABLED == 1u)

#include "mpu_faults.h"


/** \brief Handle the 'mpu_fault_io' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultIoHandler(void* const user_data, const uint32_t command_id, const char* const params);

/** \brief Handle the 'mpu_fault_os_main' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultOsMainHandler(void* const user_data, const uint32_t command_id, const char* const params);

/** \brief Handle the 'mpu_fault_os_led' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultOsLedHandler(void* const user_data, const uint32_t command_id, const char* const params);

/** \brief Handle the 'mpu_fault_task_main' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultTaskMainHandler(void* const user_data, const uint32_t command_id, const char* const params);

/** \brief Handle the 'mpu_fault_task_led' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultTaskLedHandler(void* const user_data, const uint32_t command_id, const char* const params);


/** \brief Console commands for the demo application */
static const nano_os_console_cmd_desc_t demo_app_commands[] = {
                                                                    {"mpu_fault_io", "Triggers an MPU fault by driving an I/O from the main task", NANO_OS_CONSOLE_CMDS_MpuFaultIoHandler},
                                                                    {"mpu_fault_os_main", "Triggers an MPU fault by modifying an internal variable of Nano OS from the main task", NANO_OS_CONSOLE_CMDS_MpuFaultOsMainHandler},
                                                                    {"mpu_fault_os_led", "Triggers an MPU fault by modifying an internal variable of Nano OS from the led task", NANO_OS_CONSOLE_CMDS_MpuFaultOsLedHandler},
                                                                    {"mpu_fault_task_main", "Triggers an MPU fault by modifying an internal variable of the main task from the led task", NANO_OS_CONSOLE_CMDS_MpuFaultTaskMainHandler},
                                                                    {"mpu_fault_task_led", "Triggers an MPU fault by modifying an internal variable of the led task from the main task", NANO_OS_CONSOLE_CMDS_MpuFaultTaskLedHandler}
                                                              };
/** \brief Console command group for the demo application */
static nano_os_console_cmd_group_desc_t demo_app_commands_group;


/** \brief Initialize demo application console commands */
nano_os_error_t NANO_OS_CONSOLE_CMDS_Init(void)
{
    /* Register commands to the console module */
    demo_app_commands_group.command_count = sizeof(demo_app_commands) / sizeof(nano_os_console_cmd_desc_t);
    demo_app_commands_group.commands = demo_app_commands;
    demo_app_commands_group.user_data = NULL;
    demo_app_commands_group.next = NULL;
    return NANO_OS_CONSOLE_RegisterCommands(&demo_app_commands_group);
}


/** \brief Handle the 'mpu_fault_io' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultIoHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    g_mpu_fault_io = true;
}

/** \brief Handle the 'mpu_fault_os_main' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultOsMainHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    g_mpu_fault_os_main = true;
}

/** \brief Handle the 'mpu_fault_os_led' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultOsLedHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    g_mpu_fault_os_led = true;
}

/** \brief Handle the 'mpu_fault_task_main' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultTaskMainHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    g_mpu_fault_task_main = true;
}

/** \brief Handle the 'mpu_fault_task_led' console command */
static void NANO_OS_CONSOLE_CMDS_MpuFaultTaskLedHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    g_mpu_fault_task_led = true;
}








#endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */
