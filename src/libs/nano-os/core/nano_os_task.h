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

#ifndef NANO_OS_TASK_H
#define NANO_OS_TASK_H

#include "nano_os_types.h"
#include "nano_os_wait_object.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Nano OS task states */
typedef enum _nano_os_task_state_t
{
    /** \brief Invalid */
    NOS_TS_INVALID = -1,
    /** \brief Unused task */
    NOS_TS_FREE = 0u,
    /** \brief Ready task */
    NOS_TS_READY = 1u,
    /** \brief Pending task */
    NOS_TS_PENDING = 2u,
    /** \brief Running task */
    NOS_TS_RUNNING = 3u,
    /** \brief Dead task */
    NOS_TS_DEAD = 4u
} nano_os_task_state_t;



/** \brief Nano OS task context */
typedef struct _nano_os_task_t
{
    /** \brief Current top of stack */
    nano_os_stack_t* top_of_stack;

    #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
    /** \brief Port specific task data */
    nano_os_port_task_data_t port_data;
    #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */


    /** \brief Stack origin */
    nano_os_stack_t* stack_origin;
    /** \brief Stack size in number of stack elements */
    size_t stack_size;

    #if (NANO_OS_TASK_NAME_ENABLED == 1u)
    /** \brief Task name */
    const char* name;
    #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */

    /** \brief Task state */
    nano_os_task_state_t state;


    /** \brief Current priority */
    uint8_t priority;
    /** \brief Base priority */
    uint8_t base_priority;

    #if (NANO_OS_TASK_ID_ENABLED == 1u)
    /** \brief Global object id affected by Nano-OS */
    uint16_t object_id;
    #endif /* (NANO_OS_TASK_ID_ENABLED == 1u) */


    /** \brief Wait object on which the task is waiting */
    nano_os_wait_object_t* waiting_object;
    /** \brief Wait timeout */
    uint32_t wait_timeout;
    /** \brief Wait status */
    nano_os_error_t wait_status;


    #if (NANO_OS_TASK_SLEEP_ENABLED == 1u)
    /** \brief Wait object to put the task into sleep */
    nano_os_wait_object_t sleep_object;
    #endif /* (NANO_OS_TASK_SLEEP_ENABLED == 1u) */


    #if (NANO_OS_TASK_JOIN_ENABLED == 1u)
    /** \brief Wait object to synchronize with the end of the task */
    nano_os_wait_object_t join_object;
    /** \brief Return value */
    void* ret_value;
    #endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */


    #if (NANO_OS_FLAG_SET_ENABLED == 1u)
    /** \brief Flag set wait mask */
    uint32_t flag_set_wait_mask;
    /** \brief Flag set wait mode */
    bool flag_set_wait_all;
    #endif /* (NANO_OS_FLAG_SET_ENABLED == 1u) */

    #if (NANO_OS_MAILBOX_ENABLED == 1u)
    /** \brief Buffer to sore received mail */
    void* mailbox_buffer;
    #endif /* (NANO_OS_MAILBOX_ENABLED == 1u) */

    #if (NANO_OS_ROUND_ROBIN_ENABLED == 1u)
    /** \brief Timeslice counter for round robin scheduling */
    uint32_t timeslice;
    #endif /* (NANO_OS_ROUND_ROBIN_ENABLED == 1u) */


    #if (NANO_OS_TASK_EXECUTION_COUNTER_ENABLED == 1u)
    /** \brief Execution counter */
    uint32_t execution_counter;
    #endif /* (NANO_OS_TASK_EXECUTION_COUNTER_ENABLED == 1u) */

    #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
    /** \brief Total execution time in µs */
    uint32_t execution_time;
    /** \brief CPU usage in per thousand */
    uint16_t cpu_usage;
    #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */


    /** \brief Next task in list */
    struct _nano_os_task_t* next;
    /** \brief Next task in suspent list */
    struct _nano_os_task_t* suspent_next;
    /** \brief Next task in global_list */
    struct _nano_os_task_t* global_next;

} nano_os_task_t;


/** \brief Nano OS task function */
typedef void* (*fp_nano_os_task_func_t)(void* const param);


/** \brief Nano OS task init data */
typedef struct _nano_os_task_init_data_t
{
    #if (NANO_OS_TASK_NAME_ENABLED == 1u)
    /** \brief Task name */
    const char* name;
    #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */

    /** \brief Base priority */
    uint8_t base_priority;
    /** \brief Stack origin */
    nano_os_stack_t* stack_origin;
    /** \brief Stack size in number of stack elements */
    size_t stack_size;
    /** \brief Task function */
    fp_nano_os_task_func_t task_func;
    /** \brief Task parameter */
    void* param;

    #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
    /** \brief Port specific init data */
    nano_os_port_task_init_data_t port_init_data;
    #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */

} nano_os_task_init_data_t;





/** \brief Create a new task */
nano_os_error_t NANO_OS_TASK_Create(nano_os_task_t* const task, const nano_os_task_init_data_t* const task_init_data);

#if (NANO_OS_TASK_SLEEP_ENABLED == 1u)

/** \brief Suspend the current task for fixed number of ticks */
nano_os_error_t NANO_OS_TASK_Sleep(const uint32_t tick_count);

#endif /* (NANO_OS_TASK_SLEEP_ENABLED == 1u) */

#if (NANO_OS_TASK_JOIN_ENABLED == 1u)

/** \brief Wait for the end of a task for fixed number of ticks */
nano_os_error_t NANO_OS_TASK_Join(nano_os_task_t* const task, void** return_value, const uint32_t tick_count);

#endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */

#if (NANO_OS_TASK_GET_ENABLED == 1u)

/** \brief Get the context of a task identified by its task id */
nano_os_error_t NANO_OS_TASK_Get(const uint16_t task_id, nano_os_task_t** task);

#endif /* (NANO_OS_TASK_GET_ENABLED == 1u) */



/** \brief Task startup function (should not be called directly by application) */
void NANO_OS_TASK_Start(nano_os_task_t* const task, fp_nano_os_task_func_t task_func, void* const param);







#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_TASK_H */
