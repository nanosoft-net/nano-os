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

#ifndef NANO_OS_STATS_H
#define NANO_OS_STATS_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_STATS_ENABLED == 1u)

#include "nano_os_task.h"
#include "nano_os_console.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Statistics module data */
typedef struct _nano_os_stats_module_t
{
    #if (NANO_OS_STATS_ENABLED == 1u)
    #if ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u))
    /** \brief Statistics console command group */
    nano_os_console_cmd_group_desc_t stats_cmd_group;
    #else
    /** \brief Dummy value for non empty struct */
    uint8_t dummy;
    #endif /* ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_CONSOLE_CMD_ENABLED == 1u)) */
    #else
    /** \brief Dummy value for non empty struct */
    uint8_t dummy;
    #endif /* (NANO_OS_STATS_ENABLED == 1u) */
} nano_os_stats_module_t;



/** \brief Initialize the statistics module */
nano_os_error_t NANO_OS_STATS_Init(void);



#if (NANO_OS_STATS_GETMEMORYSTATS_ENABLED == 1u)

/** \brief Nano OS memory statistics */
typedef struct _nano_os_mem_stats_t
{
    /** \brief Size of Nano OS handle in bytes */
    size_t nano_os_handle_size;
    /** \brief Size of Nano OS handle without stacks in bytes */
    size_t nano_os_handle_without_stacks_size;

    /** \brief Size of a task context in bytes */
    size_t task_context_size;

    #if (NANO_OS_SEMAPHORE_ENABLED == 1u)
    /** \brief Size of a semaphore in bytes */
    size_t semaphore_size;
    #endif /* (NANO_OS_SEMAPHORE_ENABLED == 1u) */

    #if (NANO_OS_MUTEX_ENABLED == 1u)
    /** \brief Size of a mutex in bytes */
    size_t mutex_size;
    #endif /* (NANO_OS_MUTEX_ENABLED == 1u) */

    #if (NANO_OS_COND_VAR_ENABLED == 1u)
    /** \brief Size of a condition variable in bytes */
    size_t cond_var_size;
    #endif /* (NANO_OS_COND_VAR_ENABLED == 1u) */

    #if (NANO_OS_FLAG_SET_ENABLED == 1u)
    /** \brief Size of a flag set in bytes */
    size_t flag_set_size;
    #endif /* (NANO_OS_FLAG_SET_ENABLED == 1u) */

    #if (NANO_OS_MAILBOX_ENABLED == 1u)
    /** \brief Size of a mailbox in bytes */
    size_t mailbox_size;
    #endif /* (NANO_OS_MAILBOX_ENABLED == 1u) */

    #if (NANO_OS_TIMER_ENABLED == 1u)
    /** \brief Size of a timer in bytes */
    size_t timer_size;
    #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

    #if (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)
    /** \brief Size of a waitable timer in bytes */
    size_t waitable_timer_size;
    #endif /* (NANO_OS_WAITABLE_TIMER_ENABLED == 1u) */

    /** \brief Size of all the task managed by Nano OS (including their stack) in bytes */
    size_t all_tasks_size;

    #if (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u)
    /** \brief Size of all the registered OS objects in bytes */
    size_t all_os_objects_size;
    #endif /* (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u) */

} nano_os_mem_stats_t;

/** \brief Get the memory statistics */
nano_os_error_t NANO_OS_STATS_GetMemoryStats(nano_os_mem_stats_t* const memory_stats);

#endif /* (NANO_OS_STATS_GETMEMORYSTATS_ENABLED == 1u) */

#if (NANO_OS_STATS_GETSTACKUSAGE_ENABLED == 1u)

/** \brief Get the stack usage of a task */
nano_os_error_t NANO_OS_STATS_GetStackUsage(const nano_os_task_t* const task, uint32_t* const bytes_left, uint32_t* const stack_size_in_bytes);

#endif /* (NANO_OS_STATS_GETSTACKUSAGE_ENABLED == 1u) */



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_STATS_ENABLED == 1u) */

#endif /* NANO_OS_MUTEX_H */
