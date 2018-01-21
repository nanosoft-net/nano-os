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

#include "nano_os_stats.h"

/* Check if module is enabled */
#if (NANO_OS_STATS_ENABLED == 1u)

#include "nano_os_api.h"
#include "nano_os_data.h"


#if ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u))

/** \brief Handle the 'tasks' console command */
static void NANO_OS_STATS_ConsoleTasksCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);

#if (NANO_OS_STATS_STACK_CMD_ENABLED == 1u)
/** \brief Handle the 'stack' console command */
static void NANO_OS_STATS_ConsoleStackCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);
#endif /* (NANO_OS_STATS_STACK_CMD_ENABLED == 1u) */

#if (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u)
/** \brief Handle the 'memstats' console command */
static void NANO_OS_STATS_ConsoleMemstatsCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);
#endif /* (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u) */

/** \brief Console commands for the statistics module */
static const nano_os_console_cmd_desc_t stats_module_commands[] = {
                                                                        {"tasks", "Display the list of the tasks", NANO_OS_STATS_ConsoleTasksCmdHandler},

                                                                        #if (NANO_OS_STATS_STACK_CMD_ENABLED == 1u)
                                                                        {"stack", "[task_id] Display the stack usage of the selected task", NANO_OS_STATS_ConsoleStackCmdHandler},
                                                                        #endif /* (NANO_OS_STATS_STACK_CMD_ENABLED == 1u) */

                                                                        #if (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u)
                                                                        {"memstats", "Display the memory statistics", NANO_OS_STATS_ConsoleMemstatsCmdHandler},
                                                                        #endif /* (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u) */
                                                                  };

#endif /* ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u)) */


/** \brief Statistics module data */
static nano_os_stats_module_t stats_module;


/** \brief Initialize the statistics module */
nano_os_error_t NANO_OS_STATS_Init(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* 0 init of the module */
    (void)MEMSET(&stats_module, 0, sizeof(nano_os_stats_module_t));

    #if ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u))
    /* Register heap console commands */
    if (ret == NOS_ERR_SUCCESS)
    {
        stats_module.stats_cmd_group.commands = stats_module_commands;
        stats_module.stats_cmd_group.command_count = sizeof(stats_module_commands) / sizeof(nano_os_console_cmd_desc_t);
        ret = NANO_OS_CONSOLE_RegisterCommands(&stats_module.stats_cmd_group);
    }
    #endif /* ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u)) */

    return ret;
}





#if (NANO_OS_STATS_GETMEMORYSTATS_ENABLED == 1u)

/** \brief Get the memory statistics */
nano_os_error_t NANO_OS_STATS_GetMemoryStats(nano_os_mem_stats_t* const memory_stats)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (memory_stats != NULL)
    {
        nano_os_task_t* task;

        #if (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u)
        nano_os_wait_object_t* wait_object;
        #endif /* (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u) */

        /* Get the static memory statistics */
        memory_stats->nano_os_handle_size = sizeof(nano_os_t);
        memory_stats->nano_os_handle_without_stacks_size = sizeof(nano_os_t) -
                                                           NANO_OS_IDLE_TASK_STACK_SIZE -
                                                           NANO_OS_ISR_REQUEST_TASK_STACK_SIZE;
        #if (NANO_OS_TIMER_ENABLED == 1u)
        memory_stats->nano_os_handle_without_stacks_size -= NANO_OS_TIMER_TASK_STACK_SIZE;
        #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

        memory_stats->task_context_size = sizeof(nano_os_task_t);

        #if (NANO_OS_SEMAPHORE_ENABLED == 1u)
        memory_stats->semaphore_size = sizeof(nano_os_semaphore_t);
        #endif /* (NANO_OS_SEMAPHORE_ENABLED == 1u) */

        #if (NANO_OS_MUTEX_ENABLED == 1u)
        memory_stats->mutex_size = sizeof(nano_os_mutex_t);
        #endif /* (NANO_OS_MUTEX_ENABLED == 1u) */

        #if (NANO_OS_COND_VAR_ENABLED == 1u)
        memory_stats->cond_var_size = sizeof(nano_os_cond_var_t);
        #endif /* (NANO_OS_COND_VAR_ENABLED == 1u) */

        #if (NANO_OS_FLAG_SET_ENABLED == 1u)
        memory_stats->flag_set_size = sizeof(nano_os_flag_set_t);
        #endif /* (NANO_OS_FLAG_SET_ENABLED == 1u) */

        #if (NANO_OS_MAILBOX_ENABLED == 1u)
        memory_stats->mailbox_size = sizeof(nano_os_mailbox_t);
        #endif /* (NANO_OS_MAILBOX_ENABLED == 1u) */

        #if (NANO_OS_TIMER_ENABLED == 1u)
        memory_stats->timer_size = sizeof(nano_os_timer_t);
        #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

        #if (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)
        memory_stats->waitable_timer_size = sizeof(nano_os_waitable_timer_t);
        #endif /* (NANO_OS_WAITABLE_TIMER_ENABLED == 1u) */

        /* Compute the memory occupied by all the tasks */
        task = g_nano_os.tasks;
        memory_stats->all_tasks_size = 0u;
        while (task != NULL)
        {
            memory_stats->all_tasks_size += sizeof(nano_os_task_t) + task->stack_size * sizeof(nano_os_stack_t);
            task = task->global_next;
        }


        #if (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u)
        /* Compute the memory occupied by all the tasks */
        wait_object = g_nano_os.wait_objects;
        memory_stats->all_os_objects_size = 0u;
        while (wait_object != NULL)
        {
            switch (wait_object->type)
            {
                case WOT_TASK:
                {
                    /* Not taken into account because part of a task context */
                    break;
                }

                #if (NANO_OS_SEMAPHORE_ENABLED == 1u)
                case WOT_SEMAPHORE:
                {
                    memory_stats->all_os_objects_size += sizeof(nano_os_semaphore_t);
                    break;
                }
                #endif /* (NANO_OS_SEMAPHORE_ENABLED == 1u) */

                #if (NANO_OS_MUTEX_ENABLED == 1u)
                case WOT_MUTEX:
                {
                    memory_stats->all_os_objects_size += sizeof(nano_os_mutex_t);
                    break;
                }
                #endif /* (NANO_OS_MUTEX_ENABLED == 1u) */

                #if (NANO_OS_COND_VAR_ENABLED == 1u)
                case WOT_COND_VAR:
                {
                    memory_stats->all_os_objects_size += sizeof(nano_os_cond_var_t);
                    break;
                }
                #endif /* (NANO_OS_COND_VAR_ENABLED == 1u) */

                #if (NANO_OS_FLAG_SET_ENABLED == 1u)
                case WOT_FLAG_SET:
                {
                    memory_stats->all_os_objects_size += sizeof(nano_os_flag_set_t);
                    break;
                }
                #endif /* (NANO_OS_FLAG_SET_ENABLED == 1u) */

                #if (NANO_OS_MAILBOX_ENABLED == 1u)
                case WOT_MAILBOX:
                {
                    memory_stats->all_os_objects_size += sizeof(nano_os_mailbox_t);
                    break;
                }
                #endif /* (NANO_OS_MAILBOX_ENABLED == 1u) */

                #if (NANO_OS_TIMER_ENABLED == 1u)
                case WOT_TIMER:
                {
                    memory_stats->all_os_objects_size += sizeof(nano_os_timer_t);
                    break;
                }
                #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

                default:
                {
                    /* Invalid object */
                    ret = NOS_ERR_INVALID_OBJECT;
                    break;
                }
            }
            wait_object = wait_object->next;
        }
        #endif /* (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u) */

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

#endif /* (NANO_OS_STATS_GETMEMORYSTATS_ENABLED == 1u) */


#if (NANO_OS_STATS_GETSTACKUSAGE_ENABLED == 1u)

/** \brief Get the stack usage of a task */
nano_os_error_t NANO_OS_STATS_GetStackUsage(const nano_os_task_t* const task, uint32_t* const size_left_in_bytes, uint32_t* const stack_size_in_bytes)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((task != NULL) &&
        (size_left_in_bytes != NULL) &&
        (stack_size_in_bytes != NULL))
    {
        uint32_t left = 0u;
        uint8_t* bottom_of_stack;
        uint8_t* top_of_stack;

        /* Stack size in bytes */
        (*stack_size_in_bytes) = NANO_OS_CAST(uint32_t, task->stack_size * sizeof(nano_os_stack_t));

        /* Compute size left */
        #if (NANO_OS_PORT_DESCENDING_STACK == 1u)
        bottom_of_stack = NANO_OS_CAST(uint8_t*, task->stack_origin);
        top_of_stack = NANO_OS_CAST(uint8_t*, &task->stack_origin[task->stack_size]);
        while (((*bottom_of_stack) == NANO_OS_TASK_STACK_USAGE_MARKER) && (bottom_of_stack != top_of_stack))
        {
            left++;
            bottom_of_stack++;
        }
        #else
        bottom_of_stack = NANO_OS_CAST(uint8_t*, &task->stack_origin[task->stack_size]);
        top_of_stack = NANO_OS_CAST(uint8_t*, task->stack_origin);
        while (((*bottom_of_stack) == NANO_OS_STATS_STACK_USAGE_MARKER) && (bottom_of_stack != top_of_stack))
        {
            left++;
            bottom_of_stack--;
        }
        #endif /* (NANO_OS_PORT_DESCENDING_STACK == 1u) */

        (*size_left_in_bytes) = left;

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

#endif /* (NANO_OS_STATS_GETSTACKUSAGE_ENABLED == 1u) */


#if ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u))

/** \brief Strings corresponding to task states */
static const char* task_states[] = { "FREE", "READY", "PENDING", "RUNNING", "DEAD" };
/** \brief Strings corresponding to wait object types */
static const char* wait_objects[] = { "NOT_INIT", "TASK", "SEMAPHORE", "MUTEX", "COND_VAR", "TIMER", "MAILBOX", "FLAG_SET" };

/** \brief Handle the 'tasks' console command */
static void NANO_OS_STATS_ConsoleTasksCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    nano_os_task_t* current;
    char temp_str[10u];
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    /* Write task table header */
    #if (NANO_OS_TASK_ID_ENABLED == 1u)
    (void)NANO_OS_USER_ConsoleWriteString("  id  |");
    #endif /* (NANO_OS_TASK_ID_ENABLED == 1u)*/
    #if (NANO_OS_TASK_NAME_ENABLED == 1u)
    (void)NANO_OS_USER_ConsoleWriteString("     name    |    state  ");
    #else
    (void)NANO_OS_USER_ConsoleWriteString("    state  ");
    #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
    #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
    (void)NANO_OS_USER_ConsoleWriteString("|    CPU  ");
    #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */
    (void)NANO_OS_USER_ConsoleWriteString("\r\n");

    /* Go through the global tasks list */
    current = g_nano_os.tasks;
    while (current != NULL)
    {
        /* Display task informations */
        #if (NANO_OS_TASK_ID_ENABLED == 1u)
        (void)ITOA(current->object_id, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        (void)NANO_OS_USER_ConsoleWriteString(" | ");
        #endif /* (NANO_OS_TASK_ID_ENABLED == 1u) */
        #if (NANO_OS_TASK_NAME_ENABLED == 1u)
        if (current->name != NULL)
        {
            (void)NANO_OS_USER_ConsoleWriteString(current->name);
        }
        else
        {
            (void)NANO_OS_USER_ConsoleWriteString("INVALID");
        }
        (void)NANO_OS_USER_ConsoleWriteString(" | ");
        #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
        if ((current->state >= NOS_TS_FREE) &&
            (current->state <= NOS_TS_DEAD))
        {
            (void)NANO_OS_USER_ConsoleWriteString(task_states[current->state]);
            if (current->state == NOS_TS_PENDING)
            {
                (void)NANO_OS_USER_ConsoleWriteString(" (");
                if ((current->waiting_object != NULL) &&
                    (current->waiting_object->type >= WOT_NOT_INIT) && (current->waiting_object->type <= WOT_FLAG_SET))
                {
                    (void)NANO_OS_USER_ConsoleWriteString(wait_objects[current->waiting_object->type]);
                }
                else
                {
                    (void)NANO_OS_USER_ConsoleWriteString("INVALID");
                }

                #if (NANO_OS_TRACE_ENABLED == 1u)
                (void)NANO_OS_USER_ConsoleWriteString(" : ");
                (void)ITOA(current->waiting_object->object_id, temp_str, 10u);
                (void)NANO_OS_USER_ConsoleWriteString(temp_str);
                #endif /* (NANO_OS_TRACE_ENABLED == 1u) */

                (void)NANO_OS_USER_ConsoleWriteString(")");
            }
        }
        else
        {
            (void)NANO_OS_USER_ConsoleWriteString("INVALID");
        }
        #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString(" | ");
        (void)ITOA(current->cpu_usage, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */
        (void)NANO_OS_USER_ConsoleWriteString("\r\n");

        /* Next task */
        current = current->global_next;
    }
}


#if (NANO_OS_STATS_STACK_CMD_ENABLED == 1u)

/** \brief Handle the 'stack' console command */
static void NANO_OS_STATS_ConsoleStackCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    /* Check parameters */
    if (params != NULL)
    {
        uint16_t task_id;
        nano_os_task_t* task;
        nano_os_error_t ret;

        /* Extract task id */
        task_id = ATOI(params);

        /* Look for the task */
        ret = NANO_OS_TASK_Get(task_id, &task);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Compute stack usage */
            uint32_t size_left = 0u;
            uint32_t stack_size = 0u;
            ret = NANO_OS_STATS_GetStackUsage(task, &size_left, &stack_size);
            if (ret == NOS_ERR_SUCCESS)
            {
                char temp_str[10u];
                const uint32_t size_used = stack_size - size_left;

                /* Display stack informations */
                (void)NANO_OS_USER_ConsoleWriteString("Stack usage : ");
                (void)ITOA(size_used, temp_str, 10u);
                (void)NANO_OS_USER_ConsoleWriteString(temp_str);
                (void)NANO_OS_USER_ConsoleWriteString("/");
                (void)ITOA(stack_size, temp_str, 10u);
                (void)NANO_OS_USER_ConsoleWriteString(temp_str);
                (void)NANO_OS_USER_ConsoleWriteString(" (");
                (void)ITOA(size_left, temp_str, 10u);
                (void)NANO_OS_USER_ConsoleWriteString(temp_str);
                (void)NANO_OS_USER_ConsoleWriteString(" bytes left)\r\n");
            }
            else
            {
                (void)NANO_OS_USER_ConsoleWriteString("Unable to compute stack usage\r\n");
            }
        }
        else
        {
            (void)NANO_OS_USER_ConsoleWriteString("Invalid task_id\r\n");
        }
    }
    else
    {
        (void)NANO_OS_USER_ConsoleWriteString("Missing parameter task_id\r\n");
    }
}

#endif /* (NANO_OS_STATS_STACK_CMD_ENABLED == 1u) */


#if (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u)

/** \brief Handle the 'memstats' console command */
static void NANO_OS_STATS_ConsoleMemstatsCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    nano_os_error_t ret;
    nano_os_mem_stats_t memory_stats;
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    /* Get the statistics */
    ret = NANO_OS_STATS_GetMemoryStats(&memory_stats);
    if (ret == NOS_ERR_SUCCESS)
    {
        char temp_str[10u];

        /* Display statistics */
        (void)NANO_OS_USER_ConsoleWriteString("Memory statistics :\r\n");

        (void)NANO_OS_USER_ConsoleWriteString(" - Nano OS handle size : ");
        (void)ITOA(memory_stats.nano_os_handle_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);

        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Nano OS handle without stacks size : ");
        (void)ITOA(memory_stats.nano_os_handle_without_stacks_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);

        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Task context size : ");
        (void)ITOA(memory_stats.task_context_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);

        #if (NANO_OS_SEMAPHORE_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Semaphore size : ");
        (void)ITOA(memory_stats.semaphore_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_SEMAPHORE_ENABLED == 1u) */

        #if (NANO_OS_MUTEX_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Mutex size : ");
        (void)ITOA(memory_stats.mutex_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_MUTEX_ENABLED == 1u) */

        #if (NANO_OS_COND_VAR_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Condition variable size : ");
        (void)ITOA(memory_stats.cond_var_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_COND_VAR_ENABLED == 1u) */

        #if (NANO_OS_FLAG_SET_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Flag set size : ");
        (void)ITOA(memory_stats.flag_set_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_FLAG_SET_ENABLED == 1u) */

        #if (NANO_OS_MAILBOX_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Mailbox size : ");
        (void)ITOA(memory_stats.mailbox_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_MAILBOX_ENABLED == 1u) */

        #if (NANO_OS_TIMER_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Timer size : ");
        (void)ITOA(memory_stats.timer_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

        #if (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Waitable timer size : ");
        (void)ITOA(memory_stats.waitable_timer_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        #endif /* (NANO_OS_WAITABLE_TIMER_ENABLED == 1u) */

        (void)NANO_OS_USER_ConsoleWriteString("\r\n - All tasks size : ");
        (void)ITOA(memory_stats.all_tasks_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);

        (void)NANO_OS_USER_ConsoleWriteString("\r\n - All OS objects size : ");
        (void)ITOA(memory_stats.all_os_objects_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);

        (void)NANO_OS_USER_ConsoleWriteString("\r\n");
    }
    else
    {
        (void)NANO_OS_USER_ConsoleWriteString("Unable to get memory statistics\r\n");
    }
}

#endif /* (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u) */

#endif /* ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u)) */



#endif /* (NANO_OS_STATS_ENABLED == 1u) */
