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

#ifndef NANO_OS_DATA_H
#define NANO_OS_DATA_H

#include "nano_os_interrupt.h"
#include "nano_os_task.h"
#include "nano_os_timer.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Nano OS handle */
typedef struct _nano_os_t
{
    /** \brief Current running task */
    nano_os_task_t* current_task;
    /** \brief Next running task */
    nano_os_task_t* next_running_task;

    #if (NANO_OS_PORT_CONTAINS_GLOBAL_DATA == 1)
    /** \brief Port specific data */
    nano_os_port_data_t port_data;
    #endif /* (NANO_OS_PORT_CONTAINS_GLOBAL_DATA == 1) */

    /** \brief Indicate that the OS is started */
    bool started;

    /** \brief Indicate that a schedule is needed */
    bool schedule_needed;

    /** \brief Current tick count */
    uint32_t tick_count;

    /** \brief Current lock count */
    uint32_t lock_count;

    /** \brief Current interrupt nesting count */
    uint32_t int_nesting_count;


    /** \brief List of suspent tasks */
    nano_os_task_t* suspent_tasks;

    /** \brief List of ready tasks */
    nano_os_task_t* ready_tasks[NANO_OS_NUMBER_OF_PRIORITY_LEVELS];
    /** \brief Ready tasks bitfield */
    uint8_t ready_tasks_bitfield[NANO_OS_NUMBER_OF_PRIORITY_LEVELS / 8u + 1u];


    /** \brief Idle task context */
    nano_os_task_t idle_task;

    #if (NANO_OS_PORT_PROVIDES_STACKS == 1u)
    /** \brief Idle task stack */
    nano_os_stack_t* idle_task_stack;
    #else
    /** \brief Idle task stack */
    nano_os_stack_t idle_task_stack[NANO_OS_IDLE_TASK_STACK_SIZE];
    #endif /* (NANO_OS_PORT_PROVIDES_STACKS == 1u) */


    /** \brief Interrupt service requests */
    nano_os_isr_service_request_t isr_requests[NANO_OS_MAX_ISR_REQUEST_COUNT];
    /** \brief Current service request count */
    uint8_t isr_request_count;
    /** \brief Service request read pointer */
    nano_os_isr_service_request_t* isr_request_read;
    /** \brief Service request write pointer */
    nano_os_isr_service_request_t* isr_request_write;
    /** \brief ISR request task context */
    nano_os_task_t isr_request_task;
    #if (NANO_OS_PORT_PROVIDES_STACKS == 1u)
    /** \brief ISR request task stack */
    nano_os_stack_t* isr_request_task_stack;
    #else
    /** \brief ISR request task stack */
    nano_os_stack_t isr_request_task_stack[NANO_OS_ISR_REQUEST_TASK_STACK_SIZE];
    #endif /* (NANO_OS_PORT_PROVIDES_STACKS == 1u) */
    /** \brief Wait object to signal the isr request task */
    nano_os_wait_object_t isr_request_task_wait_object;


    #if (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u)
    /** \brief List of all registered wait objects */
    nano_os_wait_object_t* wait_objects;
    /** \brief Number of wait objects */
    uint16_t wait_object_count;
    #endif /* (NANO_OS_WAIT_OBJECT_LIST_ENABLED == 1u) */

    /** \brief List of all registered tasks */
    nano_os_task_t* tasks;

    /** \brief Number of tasks */
    uint16_t task_count;


    #if (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u)
    /** \brief Stack usage marker */
    nano_os_stack_t stack_usage_marker;
    #endif /* (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u) */


    #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
    /** \brief Start of CPU usage measurement timestamp in µs */
    uint32_t start_of_cpu_usage_timestamp;
    /** \brief Context switch timestamp in µs */
    uint32_t context_switch_timestamp;
    /** \brief Next CPU usage measurement time */
    uint32_t next_cpu_usage_measurement_time;
    #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */


    #if (NANO_OS_TIMER_ENABLED == 1)
    /** \brief List of started timers */
    nano_os_timer_t* started_timers;
    /** \brief Wait object to signal the timer task */
    nano_os_wait_object_t timer_task_wait_object;
    /** \brief Timer task context */
    nano_os_task_t timer_task;
    #if (NANO_OS_PORT_PROVIDES_STACKS == 1u)
    /** \brief Timer task stack */
    nano_os_stack_t* timer_task_stack;
    #else
    /** \brief Timer task stack */
    nano_os_stack_t timer_task_stack[NANO_OS_TIMER_TASK_STACK_SIZE];
    #endif /* (NANO_OS_PORT_PROVIDES_STACKS == 1u) */
    #endif /* (NANO_OS_TIMER_ENABLED == 1) */


    #if (NANO_OS_TASK_ID_ENABLED == 1u)
    /** \brief Next object id to assign */
    uint16_t next_object_id;
    #endif /* (NANO_OS_TASK_ID_ENABLED == 1u) */

} nano_os_t;


/** \brief Instance of Nano OS */
extern nano_os_t g_nano_os;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_DATA_H */
