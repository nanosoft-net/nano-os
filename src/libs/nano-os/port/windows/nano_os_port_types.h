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

#ifndef NANO_OS_PORT_TYPES_H
#define NANO_OS_PORT_TYPES_H


/* Standard integer types*/
#if (NANO_OS_USE_STD_INT == 0)
typedef signed char             int8_t;
typedef unsigned char           uint8_t;
typedef short int               int16_t;
typedef unsigned short int      uint16_t;
typedef long int                int32_t;
typedef unsigned long int       uint32_t;
typedef long long int           int64_t;
typedef unsigned long long int  uint64_t;
#endif /* (NANO_OS_USE_STD_INT == 0) */

/* Standard bool type definition */
#if (NANO_OS_USE_STD_BOOL == 0u)
#define bool    uint8_t
#endif /* (NANO_OS_USE_STD_BOOL == 0u) */


/** \brief Stack data type */
typedef uint32_t nano_os_stack_t;

#include <Windows.h>

/** \brief Interrupt status register */
typedef bool nano_os_int_status_reg_t;


/** \brief Virtual interrupt handler prototype */
typedef void (*fp_nano_os_virtual_interrupt_handler_t)(void);

/** \brief Port specific global data */
typedef struct _nano_os_port_data_t
{
    /** \brief Scheduler mutex */
    CRITICAL_SECTION scheduler_mutex;

    /** \brief Virtual interrupt module task */
    HANDLE virtual_int_task;
    /** \brief Virtual interrupt enable mutex */
    CRITICAL_SECTION virtual_int_enable_mutex;
    /** \brief Virtual interrupt enable condition variable */
    CONDITION_VARIABLE virtual_int_enable_condvar;
    /** \brief Virtual interrupt enable state */
    bool enabled;

    /** \brief Virtual interrupt trigger mutex */
    CRITICAL_SECTION virtual_int_trigger_mutex;
    /** \brief Virtual interrupt trigger condition variable */
    CONDITION_VARIABLE virtual_int_trigger_condvar;
    /** \brief Virtual interrupt handler to execute */
    fp_nano_os_virtual_interrupt_handler_t virtual_int_handler;

    /** \brief Tick timer task handle */
    HANDLE tick_timer_task;

    #if (NANO_OS_TRACE_ENABLED == 1u)
    /** \brief Frequency of the high performance counter */
    LARGE_INTEGER hpc_frequency;
    #endif /* (NANO_OS_TRACE_ENABLED == 1u) */
} nano_os_port_data_t;


/** \brief Port specific task data */
typedef struct _nano_os_port_task_data_t
{
    /** \brief Windows task handle */
    HANDLE win_handle;
    /** \brief Task function */
    void* (*task_func)(void*);
    /** \brief Task parameter */
    void* param;
    /** \brief Event allow the task to run */
    HANDLE run_event;
    /** \brief Indicate if the thread was suspent */
    bool suspent;
} nano_os_port_task_data_t;

/** \brief Port specific task init data */
typedef struct _nano_os_port_task_init_data_t
{
    /** \brief Dummy byte to avoid empty struct */
    uint8_t dummy;
} nano_os_port_task_init_data_t;


#endif /* NANO_OS_PORT_TYPES_H */
