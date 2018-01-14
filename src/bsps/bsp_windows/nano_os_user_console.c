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


/** \brief Enable Nano OS console on stdin/stdout */
#define BSP_WINDOWS_STDINOUT_CONSOLE_ENABLED   1u


/* Check if module is enabled */
#if (BSP_WINDOWS_STDINOUT_CONSOLE_ENABLED == 1u)

#include "nano_os_api.h"

#include <stdio.h>
#include <conio.h>


/** \brief Handle the 'exit' console command */
static void NANO_OS_USER_ConsoleExitHandler(void* const user_data, const uint32_t command_id, const char* const params);

/** \brief Console commands */
static const nano_os_console_cmd_desc_t console_module_commands[] = {
                                                                        {"exit", "Stop the application", NANO_OS_USER_ConsoleExitHandler},
                                                                    };

/** \brief Console commands group */
static nano_os_console_cmd_group_desc_t console_module_commands_group;


/** \brief Initialize the hardware drivers of the user module which will send/receive the console packets */
nano_os_error_t NANO_OS_USER_ConsoleHwInit(void)
{
    // Nothing to do
    return NOS_ERR_SUCCESS;
}

/** \brief Initialize the user module which will send/receive the console packets */
nano_os_error_t NANO_OS_USER_ConsoleInit(void)
{
    nano_os_error_t ret;

    // Register specific console commands
    console_module_commands_group.commands = console_module_commands;
    console_module_commands_group.command_count = sizeof(console_module_commands) / sizeof(nano_os_console_cmd_desc_t);
    console_module_commands_group.user_data = NULL;
    ret = NANO_OS_CONSOLE_RegisterCommands(&console_module_commands_group);

    return ret;
}

/** \brief Wait forever for an incoming char on the console link (this function must be blocking) */
nano_os_error_t NANO_OS_USER_ConsoleReadChar(char* const c)
{
    static uint8_t escape_sequence = 0u;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Wait for a char on stdin */
    if (escape_sequence == 1u)
    {
        (*c) = '[';
        escape_sequence++;
    }
    else
    {
        /* Read char without echo */
        (*c) = NANO_OS_CAST(char, getch());

        /* Conversion to ASCII escape codes */
        switch((*c))
        {
            case -32:
            {
                (*c) = 0x1B; /* ESC */
                escape_sequence++;
            }
            break;

            case 'H':
            {
                if (escape_sequence == 2u)
                {
                    (*c) = 'A';
                    escape_sequence = 0u;
                }
            }
            break;

            case 'P':
            {
                if (escape_sequence == 2u)
                {
                    (*c) = 'B';
                    escape_sequence = 0u;
                }
            }
            break;

            default:
            {
                escape_sequence = 0u;
            }
            break;
        }
    }

    return ret;
}

/** \brief Write a string on the console link */
nano_os_error_t NANO_OS_USER_ConsoleWriteString(const char* const string)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Write the string on stdout */
    printf(string);

    return ret;
}



/** \brief Handle the 'exit' console command */
static void NANO_OS_USER_ConsoleExitHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    /* Exit application */
    _exit(0);
}


#endif /* (BSP_WINDOWS_STDINOUT_CONSOLE_ENABLED == 1u) */
