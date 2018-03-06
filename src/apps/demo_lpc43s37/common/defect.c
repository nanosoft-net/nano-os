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

#include "defect.h"
#include "nano_os_tools.h"


/** \brief Data associated to a defect */
typedef struct _defect_t
{
    /** \brief Indicates if the defect is active */
    bool is_active;
    /** \brief Occurence counter */
    uint32_t count;
    /** \brief Last timestamp */
    uint32_t timestamp;
} defect_t;


/** \brief List of defects */
static defect_t defect_list[DEF_MAX];

/** \brief Defect mutex */
static nano_os_mutex_t defect_mutex;



/** \brief Handle the 'defect' console command */
static void DEFECT_DefectCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);


/** \brief Defect console commands */
static const nano_os_console_cmd_desc_t defect_commands[] = {
                                                                    {"defect", "Defect management", DEFECT_DefectCmdHandler}
                                                              };
/** \brief Console command group for the defects */
static nano_os_console_cmd_group_desc_t dectect_commands_group;



/** \brief Initialize the defect management  */
nano_os_error_t DEFECT_Init(void)
{
    nano_os_error_t ret;

    /* Init mutex */
    ret = NANO_OS_MUTEX_Create(&defect_mutex, QT_PRIORITY);

    /* Register commands to the console module */
    if (ret == NOS_ERR_SUCCESS)
    {
        dectect_commands_group.command_count = sizeof(defect_commands) / sizeof(nano_os_console_cmd_desc_t);
        dectect_commands_group.commands = defect_commands;
        dectect_commands_group.user_data = NULL;
        dectect_commands_group.next = NULL;
        ret = NANO_OS_CONSOLE_RegisterCommands(&dectect_commands_group);
    }

    return ret;
}

/** \brief Raise a defect */
bool DEFECT_Raise(const defect_src_t defect_src)
{
    bool ret = false;

    if ((defect_src >= 0u) && (defect_src < DEF_MAX))
    {
        defect_t* const defect = &defect_list[defect_src];

        NANO_OS_MUTEX_Lock(&defect_mutex, 0xFFFFFFFFu);
        defect->is_active = true;
        defect->count++;
        NANO_OS_GetTickCount(&defect->timestamp);
        NANO_OS_MUTEX_Unlock(&defect_mutex);

        ret = true;
    }

    return ret;
}

/** \brief Clear a defect */
bool DEFECT_Clear(const defect_src_t defect_src)
{
    bool ret = false;

    if ((defect_src >= 0u) && (defect_src < DEF_MAX))
    {
        defect_t* const defect = &defect_list[defect_src];

        NANO_OS_MUTEX_Lock(&defect_mutex, 0xFFFFFFFFu);
        defect->is_active = false;
        NANO_OS_MUTEX_Unlock(&defect_mutex);

        ret = true;
    }

    return ret;
}

/** \brief Handle the 'defect' console command */
static void DEFECT_DefectCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    static const char* const defect_src[] = { "HEARTBEAT", "IPC_RX", "IPC_TX" };

    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    if (params == NULL)
    {
        NANO_OS_USER_ConsoleWriteString("Usage: defect show | clear\r\n");
    }
    else if (STRNCMP(params, "show", STRNLEN(params, 255u)) == 0)
    {
        char temp_str[10u];

        NANO_OS_USER_ConsoleWriteString("Defect | Active | Timestamp | Count\r\n");
        for (uint8_t i = 0u; i < DEF_MAX; i++)
        {
            NANO_OS_USER_ConsoleWriteString(defect_src[i]);
            NANO_OS_USER_ConsoleWriteString(" | ");

            if (defect_list[i].is_active)
            {
                NANO_OS_USER_ConsoleWriteString("TRUE");
            }
            else
            {
                NANO_OS_USER_ConsoleWriteString("FALSE");
            }
            NANO_OS_USER_ConsoleWriteString(" | ");

            (void)ITOA(defect_list[i].timestamp, temp_str, 10u);
            (void)NANO_OS_USER_ConsoleWriteString(temp_str);
            NANO_OS_USER_ConsoleWriteString(" | ");

            (void)ITOA(defect_list[i].count, temp_str, 10u);
            (void)NANO_OS_USER_ConsoleWriteString(temp_str);
            NANO_OS_USER_ConsoleWriteString("\r\n");
        }
    }
    else if (STRNCMP(params, "clear", STRNLEN(params, 255u)) == 0)
    {
        (void)MEMSET(defect_list, 0, sizeof(defect_list));
        NANO_OS_USER_ConsoleWriteString("Defects cleared!\r\n");
    }
    else
    {
        NANO_OS_USER_ConsoleWriteString("Invalid param\r\n");
    }
}


