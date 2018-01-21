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

#include "nano_os_console.h"

/* Check if module is enabled */
#if (NANO_OS_CONSOLE_ENABLED == 1u)

#include "nano_os.h"
#include "nano_os_data.h"
#include "nano_os_user.h"
#include "nano_os_tools.h"


/** \brief Console task */
static void* NANO_OS_CONSOLE_Task(void* const param);

#if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)
/** \brief Clear the current command line */
static void NANO_OS_CONSOLE_ClearLine(const uint32_t cmd_line_count);
#endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */

/** \brief Decode a console command */
static bool NANO_OS_CONSOLE_DecodeCmd(char* cmd, uint32_t len);

/** \brief Handle the 'help' command */
static void NANO_OS_CONSOLE_HelpCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);

/** \brief Handle the 'version' command */
static void NANO_OS_CONSOLE_VersionCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);


/** \brief Built-in console commands */
static const nano_os_console_cmd_desc_t console_builtin_commands[] = {
                                                                        {"help", "Display the list of available commands", NANO_OS_CONSOLE_HelpCmdHandler},
                                                                        {"version", "Display the version of Nano OS", NANO_OS_CONSOLE_VersionCmdHandler}
                                                                     };

/** \brief Console module data */
static nano_os_console_module_t console_module;


/** \brief Initialize the console module */
nano_os_error_t NANO_OS_CONSOLE_Init(const nano_os_port_init_data_t* const port_init_data)
{
    nano_os_task_init_data_t task_init_data;
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (port_init_data != NULL)
    {
        /* 0 init of the module */
        (void)MEMSET(&console_module, 0, sizeof(nano_os_console_module_t));

        /* Initialize the hardware drivers of the user module which will send/receive the console packets */
        ret = NANO_OS_USER_ConsoleHwInit();
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Create console task */
            (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            task_init_data.name = "Console task";
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
            task_init_data.base_priority = NANO_OS_CONSOLE_TASK_PRIORITY;
            task_init_data.stack_origin = console_module.console_task_stack;
            task_init_data.stack_size = NANO_OS_CONSOLE_TASK_STACK_SIZE;
            task_init_data.task_func = NANO_OS_CONSOLE_Task;
            task_init_data.param = NULL;
            #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
            (void)MEMCPY(&task_init_data.port_init_data, &port_init_data->console_task_init_data, sizeof(nano_os_port_task_init_data_t));
            #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */
            ret = NANO_OS_TASK_Create(&console_module.console_task, &task_init_data);
            if (ret == NOS_ERR_SUCCESS)
            {
                /* Register built-in commands */
                console_module.cmd_groups = &console_module.builtin_cmd_group;
                console_module.cmd_groups->commands = console_builtin_commands;
                console_module.cmd_groups->command_count = sizeof(console_builtin_commands) / sizeof(nano_os_console_cmd_desc_t);
                console_module.cmd_groups->user_data = NULL;
                console_module.cmd_groups->next = NULL;
            }
        }
    }

    return ret;
}


/** \brief Register commands to the console module */
nano_os_error_t NANO_OS_CONSOLE_RegisterCommands(nano_os_console_cmd_group_desc_t* const commands)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (commands != NULL)
    {
        /* Add command group to the command list */
        commands->next = console_module.cmd_groups;
        console_module.cmd_groups = commands;
        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/** \brief Get the next parameter in the command line */
const char* NANO_OS_CONSOLE_GetNextParam(const char* const current_param)
{
    const char* next_param = NULL;

    /* Check parameters */
    if (current_param != NULL)
    {
        /* Look for the end of the current param */
        next_param = current_param;
        while ((*next_param) != 0)
        {
            next_param++;
        }

        /* Jump to next char = start of next param */
        next_param++;

        /* Check if a parameter is present */
        if ((*next_param) == 0)
        {
            /* No parameter available */
            next_param = NULL;
        }
    }

    return next_param;
}

/** \brief Console task */
static void* NANO_OS_CONSOLE_Task(void* const param)
{
    bool echo;
    bool new_cmd;
    char echo_str[2];
    nano_os_error_t ret;
    char* current_cmd_char;
    uint32_t cmd_line_count;
    bool previous_was_cr = false;

    #if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)
    uint8_t escape_sequence;
    #endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */

    NANO_OS_UNUSED(param);

    /* Initialize the echo string */
    echo_str[1u] = 0;

    /* Initialize the user module which will send/receive the console packets */
    ret = NANO_OS_USER_ConsoleInit();
    NANO_OS_ERROR_ASSERT_RET(ret);

    /* Print banner */
    (void)NANO_OS_USER_ConsoleWriteString("###################################\r\n");
    (void)NANO_OS_USER_ConsoleWriteString("          Nano OS Console\r\n");
    (void)NANO_OS_USER_ConsoleWriteString("###################################\r\n");

    /* Task loop */
    while(true)
    {
        /* Reset command buffer */
        new_cmd = false;
        cmd_line_count = 0u;
        current_cmd_char = &console_module.console_cmd_buffer[0u];
        (*current_cmd_char) = 0;
        #if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)
        escape_sequence = 0;
        #endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */

        /* Print command prompt */
        ret = NANO_OS_USER_ConsoleWriteString(">");
        NANO_OS_ERROR_ASSERT_RET(ret);

        /* Command receive loop */
        do
        {

            /* Wait for an incoming char */
            ret = NANO_OS_USER_ConsoleReadChar(&echo_str[0u]);
            NANO_OS_ERROR_ASSERT_RET(ret);

            #if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)
            /* Check if an escape sequence is in progress */
            if (escape_sequence == 1u)
            {
                if (echo_str[0u] == '[')
                {
                    /* Next step of escape sequence */
                    escape_sequence++;
                }
                else
                {
                    /* End of escape sequence */
                    escape_sequence = 0u;
                }
            }
            else if (escape_sequence == 2u)
            {
                switch (echo_str[0u])
                {
                    case 'A':
                    {
                        /* Up key = previous command in history */
                        if (console_module.console_history_entry_count != 0u)
                        {
                            nano_os_error_t ret;

                            /* Update current history entry cursor */
                            if (console_module.console_current_history_entry == 0u)
                            {
                                if (console_module.console_history_entry_count == NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT)
                                {
                                    console_module.console_current_history_entry = (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT - 1u);
                                }
                            }
                            else
                            {
                                console_module.console_current_history_entry--;
                            }

                            /* Clear line */
                            NANO_OS_CONSOLE_ClearLine(cmd_line_count);

                            /* Copy current history entry to command line */
                            (void)MEMCPY(console_module.console_cmd_buffer,
                                         console_module.console_history_cmd_entries[console_module.console_current_history_entry],
                                         NANO_OS_CONSOLE_CMD_BUFFER_SIZE);
                            cmd_line_count = STRNLEN(console_module.console_cmd_buffer, NANO_OS_CONSOLE_CMD_BUFFER_SIZE);
                            current_cmd_char = &console_module.console_cmd_buffer[cmd_line_count];

                            /* Send new command entry */
                            ret = NANO_OS_USER_ConsoleWriteString(console_module.console_cmd_buffer);
                            NANO_OS_ERROR_ASSERT_RET(ret);
                        }
                    }
                    break;

                    case 'B':
                    {
                        /* Down key = next command in history */
                        if (console_module.console_history_entry_count != 0u)
                        {
                            nano_os_error_t ret;

                            /* Update current history entry cursor */
                            if (console_module.console_current_history_entry == (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT - 1u))
                            {
                                if (console_module.console_history_entry_count == NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT)
                                {
                                    console_module.console_current_history_entry = 0u;
                                }
                            }
                            else
                            {
                                if (console_module.console_current_history_entry < (console_module.console_history_entry_count - 1u))
                                {
                                    console_module.console_current_history_entry++;
                                }
                            }

                            /* Clear line */
                            NANO_OS_CONSOLE_ClearLine(cmd_line_count);

                            /* Copy current history entry to command line */
                            (void)MEMCPY(console_module.console_cmd_buffer,
                                         console_module.console_history_cmd_entries[console_module.console_current_history_entry],
                                         NANO_OS_CONSOLE_CMD_BUFFER_SIZE);
                            cmd_line_count = STRNLEN(console_module.console_cmd_buffer, NANO_OS_CONSOLE_CMD_BUFFER_SIZE);
                            current_cmd_char = &console_module.console_cmd_buffer[cmd_line_count];

                            /* Send new command entry */
                            ret = NANO_OS_USER_ConsoleWriteString(console_module.console_cmd_buffer);
                            NANO_OS_ERROR_ASSERT_RET(ret);
                        }
                    }
                    break;

                    default:
                    {
                        /* Ignore */
                    }
                    break;
                }

                /* End of escape sequence */
                escape_sequence = 0u;
            }
            else
            {
            #endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */

                /* Check char type */
                echo = true;
                switch (echo_str[0u])
                {
                    case '\r':
                    {
                        /* End of command */
                        new_cmd = true;
                        previous_was_cr = true;
                    }
                    break;

                    case '\n':
                    {
                        if (!previous_was_cr)
                        {
                            /* End of command */
                            new_cmd = true;
                        }
                        else
                        {
                            previous_was_cr = false;
                        }
                        echo = false;
                    }
                    break;

                    case '\b':
                    case 0x7F: /* DEL */
                    {
                        /* Back space, remove last char */
                        if (cmd_line_count != 0u)
                        {
                            current_cmd_char--;
                            cmd_line_count--;
                            (*current_cmd_char) = 0;
                        }
                        else
                        {
                            echo = false;
                        }
                        previous_was_cr = false;
                    }
                    break;

                    #if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)
                    case 0x1B: /* ESC */
                    {
                        /* Escape sequence */
                        escape_sequence++;
                        echo = false;
                    }
                    break;
                    #endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */

                    default:
                    {
                        /* Check if there is space left in command buffer */
                        if (cmd_line_count != (NANO_OS_CONSOLE_CMD_BUFFER_SIZE - 1u))
                        {
                            /* Store char into command buffer */
                            (*current_cmd_char) = echo_str[0u];
                            current_cmd_char++;
                            cmd_line_count++;
                        }
                        else
                        {
                            echo = false;
                        }
                        previous_was_cr = false;
                    }
                    break;
                }

                /* Echo the received char */
                if (echo)
                {
                    ret = NANO_OS_USER_ConsoleWriteString(echo_str);
                    NANO_OS_ERROR_ASSERT_RET(ret);
                    if (previous_was_cr)
                    {
                        ret = NANO_OS_USER_ConsoleWriteString("\n");
                        NANO_OS_ERROR_ASSERT_RET(ret);
                    }
                }

                /* Check if the command is ready to interpret */
                if (new_cmd && (cmd_line_count != 0u))
                {
                    bool success;

                    #if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)
                    /* Save command line to history */
                    (void)MEMCPY(console_module.console_history_cmd_entries[console_module.console_next_history_entry],
                                 console_module.console_cmd_buffer,
                                 NANO_OS_CONSOLE_CMD_BUFFER_SIZE);

                    console_module.console_next_history_entry++;
                    if (console_module.console_next_history_entry == NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT)
                    {
                        console_module.console_next_history_entry = 0u;
                    }
                    console_module.console_current_history_entry = console_module.console_next_history_entry;
                    if (console_module.console_history_entry_count != NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT)
                    {
                        console_module.console_history_entry_count++;
                    }
                    #endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */

                    /* Decode command */
                    success = NANO_OS_CONSOLE_DecodeCmd(console_module.console_cmd_buffer, cmd_line_count);
                    if (!success)
                    {
                        ret = NANO_OS_USER_ConsoleWriteString("Invalid command\r\n");
                        NANO_OS_ERROR_ASSERT_RET(ret);
                    }

                    /* Reset command line */
                    (void)MEMSET(console_module.console_cmd_buffer, 0, cmd_line_count);
                }

            #if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)
            }
            #endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */
        }
        while (!new_cmd);
    }

    return NULL;
}


#if (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u)

/** \brief Clear the current command line */
static void NANO_OS_CONSOLE_ClearLine(const uint32_t cmd_line_count)
{
    uint32_t i;
    nano_os_error_t ret;
    for(i = 0; i < cmd_line_count; i++)
    {
        console_module.console_cmd_buffer[i] = 0x7F;
    }
    ret = NANO_OS_USER_ConsoleWriteString(console_module.console_cmd_buffer);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

#endif /* (NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT > 0u) */


/** \brief Decode a console command */
static bool NANO_OS_CONSOLE_DecodeCmd(char* cmd, uint32_t len)
{
    bool found = false;
    uint32_t i = 0;
    nano_os_console_cmd_group_desc_t* current_group;

    /* Split the command and its parameters to simplify parameters management */
    while (i != len)
    {
        if (cmd[i] == ' ')
        {
            cmd[i] = 0;
        }
        i++;
    }

    /* Look for the command in all the commands groups */
    current_group = console_module.cmd_groups;
    while ((current_group != NULL) && (!found))
    {
        /* Look for the command in the command group */
        uint32_t i;
        for (i = 0; ((i < current_group->command_count) && (!found)); i++)
        {
            const nano_os_console_cmd_desc_t* current_cmd = &current_group->commands[i];
            if (STRNCMP(cmd, current_cmd->name, len) == 0)
            {
                /* Execute command specific handler */
                const char* param = NANO_OS_CONSOLE_GetNextParam(cmd);
                current_cmd->handler(current_group->user_data, i, param);
                found = true;
            }
        }
        /* Next group */
        current_group = current_group->next;
    }

    return found;
}


/** \brief Handler the 'help' command */
static void NANO_OS_CONSOLE_HelpCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    /* Check if the help concerns a specific command */
    if (params != NULL)
    {
        /* Look for the command in all the commands groups */
        bool found = false;
        nano_os_console_cmd_group_desc_t* current_group = console_module.cmd_groups;
        while ((current_group != NULL) && (!found))
        {
            /* Look for the command in the command group */
            uint32_t i;
            for (i = 0; ((i < current_group->command_count) && (!found)); i++)
            {
                const nano_os_console_cmd_desc_t* current_cmd = &current_group->commands[i];
                if (STRNCMP(params, current_cmd->name, NANO_OS_CONSOLE_CMD_BUFFER_SIZE) == 0)
                {
                    /* Display the specific command help message */
                    (void)NANO_OS_USER_ConsoleWriteString(current_cmd->help);
                    (void)NANO_OS_USER_ConsoleWriteString("\r\n");
                    found = true;
                }
            }
            /* Next group */
            current_group = current_group->next;
        }
        if (!found)
        {
            (void)NANO_OS_USER_ConsoleWriteString("Unknown command\r\n");
        }
    }
    else
    {
        /* Go through all the commands groups */
        nano_os_console_cmd_group_desc_t* current_group = console_module.cmd_groups;
        while (current_group != NULL)
        {
            /* Go through all the commands in the command group */
            uint32_t i;
            for (i = 0; i < current_group->command_count; i++)
            {
                /* Display the specific command help message */
                const nano_os_console_cmd_desc_t* current_cmd = &current_group->commands[i];
                (void)NANO_OS_USER_ConsoleWriteString(current_cmd->name);
                (void)NANO_OS_USER_ConsoleWriteString(" : ");
                (void)NANO_OS_USER_ConsoleWriteString(current_cmd->help);
                (void)NANO_OS_USER_ConsoleWriteString("\r\n");
            }
            /* Next group */
            current_group = current_group->next;
        }
    }
}

/** \brief Handler the 'version' command */
static void NANO_OS_CONSOLE_VersionCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    char version_str[4u];
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    /* Get the current version */
    nano_os_version_t nano_os_version;
    nano_os_version_t nano_os_port_version;
    nano_os_error_t ret = NANO_OS_GetVersion(&nano_os_version, &nano_os_port_version);
    NANO_OS_ERROR_ASSERT_RET(ret);

    /* Display versions */
    (void)NANO_OS_USER_ConsoleWriteString("Nano OS version : ");
    (void)ITOA(nano_os_version.major, version_str, 10u);
    (void)NANO_OS_USER_ConsoleWriteString(version_str);
    (void)NANO_OS_USER_ConsoleWriteString(".");
    (void)ITOA(nano_os_version.minor, version_str, 10u);
    (void)NANO_OS_USER_ConsoleWriteString(version_str);
    (void)NANO_OS_USER_ConsoleWriteString("\r\nNano OS port name : ");
    (void)NANO_OS_USER_ConsoleWriteString(NANO_OS_PORT_NAME);
    (void)NANO_OS_USER_ConsoleWriteString("\r\nNano OS port version : ");
    (void)ITOA(nano_os_port_version.major, version_str, 10u);
    (void)NANO_OS_USER_ConsoleWriteString(version_str);
    (void)NANO_OS_USER_ConsoleWriteString(".");
    (void)ITOA(nano_os_port_version.minor, version_str, 10u);
    (void)NANO_OS_USER_ConsoleWriteString(version_str);
    (void)NANO_OS_USER_ConsoleWriteString("\r\n");
}



#endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */
