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

#ifndef NANO_OS_CFG_H
#define NANO_OS_CFG_H


/*********************************************************/
/*  Configuration of Nano OS use of standard libraries    */
/*********************************************************/


/** \brief Indicate if the standard integer datatypes must be defined using <stdint.h> */
#define NANO_OS_USE_STD_INT             1u

/** \brief Indicate if the standard boolean datatype must be defined using <stdbool.h> */
#define NANO_OS_USE_STD_BOOL            1u

/** \brief Indicate if the standard size_t datatype must be defined using <stddef.h> */
#define NANO_OS_USE_STD_SIZE_T          1u

/** \brief Indicate if memset function from <string.h> must be used */
#define NANO_OS_USE_STD_MEMSET          0u

/** \brief Indicate if memset function from <string.h> must be used */
#define NANO_OS_USE_STD_MEMCPY          0u

/** \brief Indicate if memcmp function from <string.h> must be used */
#define NANO_OS_USE_STD_MEMCMP          0u

/** \brief Indicate if strncmp function from <string.h> must be used */
#define NANO_OS_USE_STD_STRNCMP         0u

/** \brief Indicate if strnlen function from <string.h> must be used */
#define NANO_OS_USE_STD_STRNLEN         0u

/** \brief Indicate if strncat function from <string.h> must be used */
#define NANO_OS_USE_STD_STRNCAT         0u

/** \brief Indicate if vsnprintf function from <stdio.h> must be used */
#define NANO_OS_USE_STD_VSNPRINTF       1u

/** \brief Indicate if snprintf function from <stdio.h> must be used */
#define NANO_OS_USE_STD_SNPRINTF        1u

/** \brief Indicate if atoi function from <stdlib.h> must be used */
#define NANO_OS_USE_STD_ATOI            0u

/** \brief Indicate if itoa function from <stdlib.h> must be used */
#define NANO_OS_USE_STD_ITOA            0u


/*********************************************************/
/*Configuration of Nano OS port specific functionalities */
/*********************************************************/

#include "nano_os_port_cfg.h"

/*********************************************************/
/*   Configuration of Nano OS core functionalities       */
/*********************************************************/


/** \brief Number of priority levels */
#define NANO_OS_NUMBER_OF_PRIORITY_LEVELS       10u

/** \brief Enable the round-robin scheduling */
#define NANO_OS_ROUND_ROBIN_ENABLED             1u

/** \brief Timeslice in number of ticks for the tasks in round robin scheduling */
#define NANO_OS_ROUND_ROBIN_TIMESLICE           5u



/** \brief Size of the idle task stack in number of elements */
#define NANO_OS_IDLE_TASK_STACK_SIZE            NANO_OS_PORT_MIN_STACK_SIZE

/** \brief Nano OS tick rate in Hz */
#define NANO_OS_TICK_RATE_HZ                    100u


/** \brief Maximum number of interrupt service requests
 *         Shall be equal to :
 *         maximum number of interrupt handler which share the same wait object + 1
 * */
#define NANO_OS_MAX_ISR_REQUEST_COUNT           2u

/** \brief Size of the interrupt service request task stack in number of elements */
#define NANO_OS_ISR_REQUEST_TASK_STACK_SIZE     (NANO_OS_PORT_MIN_STACK_SIZE + 64u)




/** \brief Task stack pointer check at runtime */
#define NANO_OS_RUNTIME_SP_CHECK_ENABLED        1u

/** \brief Enable per task execution counter */
#define NANO_OS_TASK_EXECUTION_COUNTER_ENABLED  1u

/** \brief Enable CPU usage measurement */
#define NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED   1u

/** \brief Period in number of ticks for CPU usage measurement */
#define NANO_OS_CPU_USAGE_MEASUREMENT_PERIOD    25u



/** \brief Enable error handler callback */
#define NANO_OS_ERROR_HANDLER_CALLBACK_ENABLED          0u

/** \brief Enable Idle task callback */
#define NANO_OS_IDLE_TASK_CALLBACK_ENABLED              0u

/** \brief Enable tick interrupt callback */
#define NANO_OS_TICK_CALLBACK_ENABLED                   0u

/** \brief Enable interrupt service error callback */
#define NANO_OS_ISR_REQUEST_ERROR_CALLBACK_ENABLED      0u




/** \brief Enable debug information for Segger GDB RTOS plugin */
#define NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED  0u



/*************************** Trace options *************************/

/** \brief Enable the trace module */
#define NANO_OS_TRACE_ENABLED                   1u


/*************************** Synchronization objects features *************************/

/** \brief Enable destruction of synchronization objects */
#define NANO_OS_WAIT_OBJECT_DESTROY_ENABLED     1u

/** \brief Keep a list of all created synchronization objects */
#define NANO_OS_WAIT_OBJECT_LIST_ENABLED        1u

/** \brief Enable to associate a name to a synchronization wait object */
#define NANO_OS_WAIT_OBJECT_NAME_ENABLED        1u


/*************************** Task system calls *************************/

/** \brief Enable to associate a name to a task object */
#define NANO_OS_TASK_NAME_ENABLED               1u

/** \brief Enable NANO_OS_TASK_Sleep() system call */
#define NANO_OS_TASK_SLEEP_ENABLED              1u

/** \brief Enable NANO_OS_TASK_Join() system call */
#define NANO_OS_TASK_JOIN_ENABLED               1u

/** \brief Enable NANO_OS_TASK_Get() system call */
#define NANO_OS_TASK_GET_ENABLED                1u


/*************************** Semaphore system calls *************************/

/** \brief Enable the semaphore objects */
#define NANO_OS_SEMAPHORE_ENABLED               1u

/** \brief Enable NANO_OS_SEMAPHORE_PostFromIsr() system call */
#define NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED           1u

/** \brief Enable NANO_OS_SEMAPHORE_GetCount() system call */
#define NANO_OS_SEMAPHORE_GETCOUNT_ENABLED              1u

/** \brief Enable NANO_OS_SEMAPHORE_Destroy() system call */
#define NANO_OS_SEMAPHORE_DESTROY_ENABLED               1u

/** \brief Enable NANO_OS_SEMAPHORE_SetName() system call */
#define NANO_OS_SEMAPHORE_SETNAME_ENABLED       1u


/*************************** Mutex system calls *************************/

/** \brief Enable the mutex objects */
#define NANO_OS_MUTEX_ENABLED                   1u

/** \brief Enable NANO_OS_MUTEX_TryLock() system call */
#define NANO_OS_MUTEX_TRYLOCK_ENABLED                   1u

/** \brief Enable NANO_OS_MUTEX_Destroy() system call */
#define NANO_OS_MUTEX_DESTROY_ENABLED                   1u

/** \brief Enable NANO_OS_MUTEX_SetName() system call */
#define NANO_OS_MUTEX_SETNAME_ENABLED           1u


/*********************** Condition variable system calls *********************/

/** \brief Enable the condition variable objects */
#define NANO_OS_COND_VAR_ENABLED                1u

/** \brief Enable NANO_OS_COND_VAR_Destroy() system call */
#define NANO_OS_COND_VAR_DESTROY_ENABLED                1u

/** \brief Enable NANO_OS_COND_VAR_SignalFromIsr() system call */
#define NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED          1u

/** \brief Enable NANO_OS_COND_VAR_SetName() system call */
#define NANO_OS_COND_VAR_SETNAME_ENABLED        1u


/*********************** Flag set system calls *********************/

/** \brief Enable the flag set objects */
#define NANO_OS_FLAG_SET_ENABLED                1u

/** \brief Enable NANO_OS_FLAG_SET_SetFromIsr() system call */
#define NANO_OS_FLAG_SET_SETFROMISR_ENABLED             1u

/** \brief Enable NANO_OS_FLAG_SET_Clear() system call */
#define NANO_OS_FLAG_SET_CLEAR_ENABLED                  1u

/** \brief Enable NANO_OS_FLAG_SET_ClearFromIsr() system call */
#define NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED           1u

/** \brief Enable NANO_OS_FLAG_SET_Destroy() system call */
#define NANO_OS_FLAG_SET_DESTROY_ENABLED                1u

/** \brief Enable NANO_OS_FLAG_SET_SetName() system call */
#define NANO_OS_FLAG_SET_SETNAME_ENABLED        1u


/*********************** Mailbox system calls *********************/

/** \brief Enable the mailbox objects */
#define NANO_OS_MAILBOX_ENABLED					1u

/** \brief Enable NANO_OS_MAILBOX_PostFromIsr() system call */
#define NANO_OS_MAILBOX_POSTFROMISR_ENABLED				1u

/** \brief Enable NANO_OS_MAILBOX_GetCount() system call */
#define NANO_OS_MAILBOX_GETCOUNT_ENABLED				1u

/** \brief Enable NANO_OS_MAILBOX_Destroy() system call */
#define NANO_OS_MAILBOX_DESTROY_ENABLED                 1u

/** \brief Enable NANO_OS_MAILBOX_SetName() system call */
#define NANO_OS_MAILBOX_SETNAME_ENABLED         1u


/*************************** Timer system calls *************************/

/** \brief Enable the timer objects */
#define NANO_OS_TIMER_ENABLED                   1u

/** \brief Enable NANO_OS_TIMER_Destroy() system call */
#define NANO_OS_TIMER_DESTROY_ENABLED                   1u

/** \brief Size of the timer task stack in number of elements */
#define NANO_OS_TIMER_TASK_STACK_SIZE           (NANO_OS_PORT_MIN_STACK_SIZE + 64u)


/*************************** Waitable timer system calls *************************/

/** \brief Enable the waitable timer objects */
#define NANO_OS_WAITABLE_TIMER_ENABLED          1u

/** \brief Enable NANO_OS_WAITABLE_TIMER_Destroy() system call */
#define NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED          1u

/** \brief Enable NANO_OS_WAITABLE_TIMER_SetName() system call */
#define NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED  1u


/*********************************************************/
/*          Configuration of Nano OS modules             */
/*********************************************************/

/** \brief Enable Nano OS modules */
#define NANO_OS_MODULES_ENABLED					1u


/*************************** Heap module *************************/

/** \brief Enable the heap module */
#define NANO_OS_HEAP_ENABLED                    1u

/** \brief Enable NANO_OS_HEAP_Free() system call */
#define NANO_OS_HEAP_FREE_ENABLED                       1u

/** \brief Enable the heap capacity to merge de-allocated blocks in memory if possible (contiguous blocks) */
#define NANO_OS_HEAP_MERGE_FREE_BLOCKS_ENABLED  1u

/** \brief Enable NANO_OS_HEAP_GetStats() system call */
#define NANO_OS_HEAP_GETSTATS_ENABLED                   1u

/** \brief Enable the heap commands in Nano OS console module */
#define NANO_OS_HEAP_CONSOLE_CMD_ENABLED        1u


/*********************** Statistics module *********************/


/** \brief Enable the statistics module */
#define NANO_OS_STATS_ENABLED            1u

/** \brief Enable NANO_OS_STATS_GetMemoryStats() system call */
#define NANO_OS_STATS_GETMEMORYSTATS_ENABLED            1u

/** \brief Enable NANO_OS_STATS_GetStackUsage() system call */
#define NANO_OS_STATS_GETSTACKUSAGE_ENABLED             1u

/** \brief Enable the statistics commands in Nano OS console module */
#define NANO_OS_STATS_CONSOLE_CMD_ENABLED       1u

/** \brief Enable the 'stack' statistics command in Nano OS console module */
#define NANO_OS_STATS_STACK_CMD_ENABLED         1u

/** \brief Enable the 'memstats' statistics command in Nano OS console module */
#define NANO_OS_STATS_MEMSTATS_CMD_ENABLED      1u


/*********************** Console module *********************/


/** \brief Enable the console module */
#define NANO_OS_CONSOLE_ENABLED                 1u

/** \brief Priority level of the console task */
#define NANO_OS_CONSOLE_TASK_PRIORITY           1u

/** \brief Size of the console task stack in number of elements */
#define NANO_OS_CONSOLE_TASK_STACK_SIZE         (NANO_OS_PORT_MIN_STACK_SIZE + 128u)

/** \brief Size of the console command buffer in number of characters
           (must be big enough to store a whole command line) */
#define NANO_OS_CONSOLE_CMD_BUFFER_SIZE         64u

/** \brief Number of commands stored into history (0 = history disabled) */
#define NANO_OS_CONSOLE_HISTORY_CMD_ENTRY_COUNT 5u



/*********************** Debug module *********************/


/** \brief Enable the debug module */
#define NANO_OS_DEBUG_ENABLED                   1u

/** \brief Priority level of the debug task */
#define NANO_OS_DEBUG_TASK_PRIORITY             1u

/** \brief Size of the debug task stack in number of elements */
#define NANO_OS_DEBUG_TASK_STACK_SIZE           (NANO_OS_PORT_MIN_STACK_SIZE + 128u)


/** \brief Enable the task requests in the debug module */
#define NANO_OS_DEBUG_TASK_REQS_ENABLED         1u

/** \brief Enable the wait object requests in the debug module */
#define NANO_OS_DEBUG_WAIT_OBJECT_REQS_ENABLED  1u

/** \brief Enable the memory access requests in the debug module */
#define NANO_OS_DEBUG_MEMORY_REQS_ENABLED       1u




#endif /* NANO_OS_CFG_H */
