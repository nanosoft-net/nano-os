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

#ifndef NANO_OS_SAFE_CFG_H
#define NANO_OS_SAFE_CFG_H

#include "nano_os_cfg.h"

/*********************************************************/
/*  Check of Nano OS core functionalities configuration  */
/*********************************************************/


/** \brief Number of priority levels */
#if (NANO_OS_NUMBER_OF_PRIORITY_LEVELS < 3u)
    #error "Number of priority levels must be > 3"
#endif /* (NANO_OS_NUMBER_OF_PRIORITY_LEVELS < 3u) */

/** \brief Enable the round-robin scheduling */
#define NANO_OS_ROUND_ROBIN_ENABLED             1u

/** \brief Timeslice in number of ticks for the tasks in round robin scheduling */
#if (NANO_OS_ROUND_ROBIN_TIMESLICE <= 0u)
    #error "Round robin time slice must be > 0"
#endif /* (NANO_OS_ROUND_ROBIN_TIMESLICE <= 0u) */

/** \brief Nano OS tick rate in Hz */
#if ((NANO_OS_TICK_RATE_HZ <= 0u) || (NANO_OS_TICK_RATE_HZ > 1000u))
    #error "Tick rate must be in the following interval ]0;1000u]"
#endif /* ((NANO_OS_TICK_RATE_HZ <= 0u) || (NANO_OS_TICK_RATE_HZ > 1000u)) */

/** \brief Maximum number of interrupt service requests */
#if (NANO_OS_MAX_ISR_REQUEST_COUNT <= 0u)
    #error "Number of interrupt service requests must be > 0"
#endif /* (NANO_OS_MAX_ISR_REQUEST_COUNT <= 0u) */

/** \brief Period in number of ticks for CPU usage measurement */
#if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
#define CPU_USAGE_MEASUREMENT_PERIOD ((NANO_OS_CPU_USAGE_MEASUREMENT_PERIOD) * 1000u / NANO_OS_TICK_RATE_HZ)
#if ((NANO_OS_CPU_USAGE_MEASUREMENT_PERIOD <= 0u) || (CPU_USAGE_MEASUREMENT_PERIOD > 4000u))
    #error "CPU usage measurement period must be in the following interval ]0;4000u] milliseconds"
#endif /* ((NANO_OS_CPU_USAGE_MEASUREMENT_PERIOD <= 0u) || (CPU_USAGE_MEASUREMENT_PERIOD > 4000u)) */
#endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */


/** \brief Enable the task id field */
#if ((NANO_OS_TASK_GET_ENABLED == 1u) || (NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u))
    #define NANO_OS_TASK_ID_ENABLED     1u
#endif /* ((NANO_OS_TASK_GET_ENABLED == 1u) || (NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u)) */



/*************************** Trace options *************************/


/*************************** OS objects features *************************/



/*************************** Task system calls *************************/


/*************************** Semaphore system calls *************************/


/** \brief Enable NANO_OS_WAITABLE_TIMER_Destroy() system call */
#if ((NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_DESTROY_ENABLED must be enabled to use NANO_OS_SEMAPHORE_Destroy() system call"
#endif /* ((NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u)) */

/** \brief Enable NANO_OS_SEMAPHORE_SetName() system call */
#if ((NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_NAME_ENABLED must be enabled to use NANO_OS_SEMAPHORE_SetName() system call"
#endif /* ((NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u)) */


/*************************** Mutex system calls *************************/


/** \brief Enable NANO_OS_MUTEX_Destroy() system call */
#if ((NANO_OS_MUTEX_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_DESTROY_ENABLED must be enabled to use NANO_OS_MUTEX_Destroy() system call"
#endif /* ((NANO_OS_MUTEX_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u)) */

/** \brief Enable NANO_OS_MUTEX_SetName() system call */
#if ((NANO_OS_MUTEX_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_NAME_ENABLED must be enabled to use NANO_OS_MUTEX_SetName() system call"
#endif /* ((NANO_OS_MUTEX_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u)) */


/*********************** Condition variable system calls *********************/


/** \brief Enable the condition variable objects */
#if ((NANO_OS_COND_VAR_ENABLED == 1u) && (NANO_OS_MUTEX_ENABLED != 1u))
#error "Mutexes must be enabled to use condition variables"
#endif /* ((NANO_OS_COND_VAR_ENABLED == 1u) && (NANO_OS_MUTEX_ENABLED != 1u)) */

/** \brief Enable NANO_OS_COND_VAR_Destroy() system call */
#if ((NANO_OS_COND_VAR_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_DESTROY_ENABLED must be enabled to use NANO_OS_COND_VAR_Destroy() system call"
#endif /* ((NANO_OS_COND_VAR_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u)) */

/** \brief Enable NANO_OS_COND_VAR_SetName() system call */
#if ((NANO_OS_COND_VAR_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_NAME_ENABLED must be enabled to use NANO_OS_COND_VAR_SetName() system call"
#endif /* ((NANO_OS_COND_VAR_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u)) */


/*********************** Flag set system calls *********************/


/** \brief Enable NANO_OS_FLAG_SET_Destroy() system call */
#if ((NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_DESTROY_ENABLED must be enabled to use NANO_OS_FLAG_SET_Destroy() system call"
#endif /* ((NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u)) */

/** \brief Enable NANO_OS_FLAG_SET_SetName() system call */
#if ((NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_NAME_ENABLED must be enabled to use NANO_OS_FLAG_SET_SetName() system call"
#endif /* ((NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u)) */


/*********************** Mailbox system calls *********************/


/** \brief Enable NANO_OS_MAILBOX_Destroy() system call */
#if ((NANO_OS_MAILBOX_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_DESTROY_ENABLED must be enabled to use NANO_OS_MAILBOX_Destroy() system call"
#endif /* ((NANO_OS_MAILBOX_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u)) */

/** \brief Enable NANO_OS_MAILBOX_SetName() system call */
#if ((NANO_OS_MAILBOX_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_NAME_ENABLED must be enabled to use NANO_OS_MAILBOX_SetName() system call"
#endif /* ((NANO_OS_MAILBOX_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u)) */


/*************************** Timer system calls *************************/


/*************************** Waitable timer system calls  *************************/


/** \brief Enable the waitable timer objects */
#if ((NANO_OS_WAITABLE_TIMER_ENABLED == 1u) && (NANO_OS_TIMER_ENABLED != 1u))
#error "Timer module must be enabled to use waitable timers"
#endif /* ((NANO_OS_COND_VAR_ENABLED == 1u) && (NANO_OS_MUTEX_ENABLED != 1u)) */

/** \brief Enable NANO_OS_WAITABLE_TIMER_Destroy() system call */
#if ((NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_DESTROY_ENABLED must be enabled to use NANO_OS_WAITABLE_TIMER_Destroy() system call"
#endif /* ((NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_DESTROY_ENABLED != 1u)) */

/** \brief Enable NANO_OS_WAITABLE_TIMER_SetName() system call */
#if ((NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u))
#error "NANO_OS_WAIT_OBJECT_NAME_ENABLED must be enabled to use NANO_OS_WAITABLE_TIMER_SetName() system call"
#endif /* ((NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u) && (NANO_OS_WAIT_OBJECT_NAME_ENABLED != 1u)) */


/*********************************************************/
/*     Check of Nano OS modules configuration            */
/*********************************************************/




/*************************** Heap module *************************/

/** \brief Enable the heap module */
#if ((NANO_OS_HEAP_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u))
#error "Nano OS modules must be enabled to use heap module"
#endif /* ((NANO_OS_HEAP_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u)) */

#if ((NANO_OS_HEAP_ENABLED == 1u) && (NANO_OS_MUTEX_ENABLED != 1u))
#error "Mutexes must be enabled to use heap module"
#endif /* ((NANO_OS_HEAP_ENABLED == 1u) && (NANO_OS_MUTEX_ENABLED != 1u)) */

/** \brief Enable the heap commands in Nano OS console module */
#if ((NANO_OS_HEAP_ENABLED == 1u) && (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u) && (NANO_OS_CONSOLE_ENABLED != 1u))
#error "Console module must be enabled to use heap console commands"
#endif /* ((NANO_OS_HEAP_ENABLED == 1u) && (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u) && (NANO_OS_CONSOLE_ENABLED != 1u)) */


/*********************** Statistics module *********************/

/** \brief Enable the statistics module */
#if ((NANO_OS_STATS_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u))
#error "Nano OS modules must be enabled to use statistics module"
#endif /* ((NANO_OS_STATS_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u)) */

#if ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_STACK_CMD_ENABLED == 1u) && (NANO_OS_STATS_GETSTACKUSAGE_ENABLED != 1u))
#error "NANO_OS_STATS_GetStackUsage() system call must be enabled to use 'stack' console commands"
#endif /* ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_STACK_CMD_ENABLED == 1u) && (NANO_OS_STATS_GETSTACKUSAGE_ENABLED != 1u)) */

/** \brief Enable the 'memstats' statistics command in Nano OS console module */
#if ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u) && (NANO_OS_STATS_GETMEMORYSTATS_ENABLED != 1u))
#error "NANO_OS_STATS_GetMemoryStats() system call must be enabled to use 'memstats' console commands"
#endif /* ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_STATS_MEMSTATS_CMD_ENABLED == 1u) && (NANO_OS_STATS_GETMEMORYSTATS_ENABLED != 1u)) */


/*********************** Console module *********************/

/** \brief Enable the console module */
#if ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u))
#error "Nano OS modules must be enabled to use console module"
#endif /* ((NANO_OS_CONSOLE_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u)) */

/** \brief Size of the console command buffer in number of characters
           (must be big enough to store a whole command line) */
#if (NANO_OS_CONSOLE_CMD_BUFFER_SIZE <= 4u)
    #error "Console command buffer size must be > 4"
#endif /* (NANO_OS_CONSOLE_CMD_BUFFER_SIZE <= 0u) */


/*********************** Debug module *********************/


/** \brief Enable the debug module */
#if ((NANO_OS_DEBUG_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u))
#error "Nano OS modules must be enabled to use debug module"
#endif /* ((NANO_OS_DEBUG_ENABLED == 1u) && (NANO_OS_MODULES_ENABLED != 1u)) */



#endif /* NANO_OS_SAFE_CFG_H */
