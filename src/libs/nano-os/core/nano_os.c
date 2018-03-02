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

#include "nano_os.h"
#include "nano_os_data.h"
#include "nano_os_port.h"
#include "nano_os_trace.h"
#include "nano_os_tools.h"
#include "nano_os_scheduler.h"
#include "nano_os_modules.h"



/** \brief Instance of Nano OS */
nano_os_t g_nano_os;


#if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
/** \brief Compute per task CPU usage */
static void NANO_OS_ComputeCpuUsage(void);
#endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */

/** \brief Idle task */
static void* NANO_OS_IdleTask(void* param);


#if (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u)
/** \brief Initialize debug informations for Segger GDB RTOS plugin */
static void NANO_OS_InitSeggerGdbRtosPluginInfos(void);
#endif /* (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u) */



/** \brief Initialize Nano OS */
nano_os_error_t NANO_OS_Init(void)
{
    nano_os_error_t ret;
    nano_os_port_init_data_t port_init_data;

    /* 0 init of the instance */
    (void)MEMSET(&g_nano_os, 0, sizeof(nano_os_t));

    #if (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u)
    /* Init stack usage marker */
    (void)MEMSET(&g_nano_os.stack_usage_marker, NANO_OS_TASK_STACK_USAGE_MARKER, sizeof(nano_os_stack_t));
    #endif /* (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u) */

    #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
    /* Init next CPU usage measurement time */
    g_nano_os.next_cpu_usage_measurement_time = NANO_OS_CPU_USAGE_MEASUREMENT_PERIOD;
    #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */

    #if (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u)
    /* Initialize debug informations for Segger GDB RTOS plugin */
    NANO_OS_InitSeggerGdbRtosPluginInfos();
    #endif /* (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u) */

    /* Port specific initialization */
    ret = NANO_OS_PORT_Init(&port_init_data);
    if (ret == NOS_ERR_SUCCESS)
    {
        #if (NANO_OS_TRACE_ENABLED == 1u)
        /* Initialize the trace module */
        if (ret == NOS_ERR_SUCCESS)
        {
            ret = NANO_OS_TRACE_Init();
        }
        #endif /* (NANO_OS_TRACE_ENABLED == 1u) */

        /* Initialize the interrupt service module */
        if (ret == NOS_ERR_SUCCESS)
        {
            ret = NANO_OS_INTERRUPT_Init(&port_init_data);
        }

        #if (NANO_OS_TIMER_ENABLED == 1u)
        /* Initialize the timer module */
        if (ret == NOS_ERR_SUCCESS)
        {
            ret = NANO_OS_TIMER_Init(&port_init_data);
        }
        #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

        #if (NANO_OS_MODULES_ENABLED == 1u)
        /* Initialize the external modules */
        if (ret == NOS_ERR_SUCCESS)
        {
            ret = NANO_OS_MODULES_Init(&port_init_data);
        }
        #endif /* (NANO_OS_MODULES_ENABLED == 1u) */

        /* Create idle task */
        if (ret == NOS_ERR_SUCCESS)
        {
            nano_os_task_init_data_t task_init_data;
            (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            task_init_data.name = "Idle task";
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
            task_init_data.base_priority = 0u;
            task_init_data.stack_origin = g_nano_os.idle_task_stack;
            task_init_data.stack_size = NANO_OS_IDLE_TASK_STACK_SIZE;
            task_init_data.task_func = NANO_OS_IdleTask;
            task_init_data.param = NULL;
            #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
            (void)MEMCPY(&task_init_data.port_init_data, &port_init_data.idle_task_init_data, sizeof(nano_os_port_task_init_data_t));
            #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */
            ret = NANO_OS_TASK_Create(&g_nano_os.idle_task, &task_init_data);
        }
    }

    return ret;
}

/** \brief Start Nano OS (this function returns only in case of error at during first scheduling) */
nano_os_error_t NANO_OS_Start(void)
{
    const nano_os_error_t ret = NANO_OS_SCHEDULER_FirstScheduling();
    return ret;
}

/** \brief Get the current tick count since Nano OS is started */
nano_os_error_t NANO_OS_GetTickCount(uint32_t* tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (tick_count != NULL)
    {
        /* Get the tick count */
        (*tick_count) = NANO_OS_PORT_ATOMIC_READ32(g_nano_os.tick_count);

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

/** \brief Get Nano OS version and the current used port version */
nano_os_error_t NANO_OS_GetVersion(nano_os_version_t* const version, nano_os_version_t* const port_version)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((version != NULL) && (port_version != NULL))
    {
        /* Get the port version */
        ret = NANO_OS_PORT_GetVersion(port_version);

        /* Fill Nano OS version */
        version->major = 1;
        version->minor = 0;
    }

    return ret;
}


/** \brief Handle the tick interrupt */
void NANO_OS_TickInterrupt(void)
{
    nano_os_error_t ret;

    /* Update tick */
    NANO_OS_PORT_ATOMIC_INC32(g_nano_os.tick_count);

    #if (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u)
    /* Check current task stack pointer range */
    {
        nano_os_task_t* const current_task = g_nano_os.current_task;
        const uint32_t task_sp = NANO_OS_PORT_GET_TASK_SP();
        const uint32_t lower_limit = NANO_OS_CAST(uint32_t, current_task->stack_origin);
        const uint32_t upper_limit = NANO_OS_CAST(uint32_t, &current_task->stack_origin[current_task->stack_size]);
        NANO_OS_ERROR_ASSERT(((task_sp >= lower_limit) && (task_sp < upper_limit)), NOS_ERR_INVALID_SP);
    }
    #endif /* (NANO_OS_RUNTIME_SP_CHECK_ENABLED == 1u) */

    #if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
    /* CPU usage measurement */
    if (g_nano_os.tick_count == g_nano_os.next_cpu_usage_measurement_time)
    {
        NANO_OS_ComputeCpuUsage();
    }
    #endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */

    /* Handle timers */
    #if (NANO_OS_TIMER_ENABLED == 1u)
    if (NANO_OS_PORT_ATOMIC_READ32(g_nano_os.lock_count) == 0u)
    {
        NANO_OS_TIMER_TickHandler();
    }
    else
    {
        /* Fill interrupt service request to wake up tasks */
        nano_os_isr_service_request_t request;
        request.service_func = NANO_OS_CAST(fp_nano_os_isr_func_t, NANO_OS_TIMER_TickHandler);

        /* Queue request */
        ret = NANO_OS_INTERRUPT_QueueRequest(&request);
        NANO_OS_ERROR_ASSERT_RET(ret);
    }
    #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

    /* Handle suspent tasks */
    if (NANO_OS_PORT_ATOMIC_READ32(g_nano_os.lock_count) == 0u)
    {
        NANO_OS_SCHEDULER_HandleSuspentTasks();
    }
    else
    {
        /* Fill interrupt service request to wake up tasks */
        nano_os_isr_service_request_t request;
        request.service_func = NANO_OS_CAST(fp_nano_os_isr_func_t, NANO_OS_SCHEDULER_HandleSuspentTasks);

        /* Queue request */
        ret = NANO_OS_INTERRUPT_QueueRequest(&request);
        NANO_OS_ERROR_ASSERT_RET(ret);
    }

    #if (NANO_OS_TICK_CALLBACK_ENABLED == 1u)
    /* User callback */
    NANO_OS_USER_TickCallback();
    #endif /* (NANO_OS_TICK_CALLBACK_ENABLED == 1u) */
}


#if (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)
/** \brief Compute per task CPU usage */
static void NANO_OS_ComputeCpuUsage(void)
{
    /* Get current timestamp */
    const uint32_t now_in_us = NANO_OS_PORT_GetTimestampInUs();

    /* Get elapsed time since last timestamp measurement */
    const uint32_t elapsed_in_us = now_in_us - g_nano_os.start_of_cpu_usage_timestamp;

    /* Compute CPU usage for each task */
    nano_os_task_t* task = g_nano_os.tasks;
    g_nano_os.current_task->execution_time += now_in_us - g_nano_os.context_switch_timestamp;
    while (task != NULL)
    {
        task->cpu_usage = NANO_OS_CAST(uint16_t, ((task->execution_time * 1000u) / elapsed_in_us));
        task->execution_time = 0u;
        task = task->global_next;
    }

    /* Next CPU usage measurement */
    g_nano_os.context_switch_timestamp = now_in_us;
    g_nano_os.start_of_cpu_usage_timestamp = now_in_us;
    g_nano_os.next_cpu_usage_measurement_time += NANO_OS_CPU_USAGE_MEASUREMENT_PERIOD;
}
#endif /* (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u) */


/** \brief Idle task */
static void* NANO_OS_IdleTask(void* param)
{
    NANO_OS_UNUSED(param);

    /* Infinite loop */
    while (true)
    {
        /* Port specific operations */
        NANO_OS_PORT_IDLE_TASK_HOOK();

        #if (NANO_OS_IDLE_TASK_CALLBACK_ENABLED == 1u)
        /* User callback */
        NANO_OS_USER_IdleTaskCallback();
        #endif /* (NANO_OS_IDLE_TASK_CALLBACK_ENABLED == 1u) */
    }

    /* Shall never reached here... */
    return NULL;
}



#if (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u)


/** \brief Debug informations for Segger GDB RTOS plugin */
typedef struct _nano_os_segger_gdb_rtos_plugin_info_t
{
    /** \brief Port name */
    const char* port_name;

    /** \brief Offset of pointer to current task in nano_os_t structure */
    uint16_t current_task_offset;
    /** \brief Offset of tick count in nano_os_t structure */
    uint16_t tick_count_offset;
    /** \brief Offset of global task list in nano_os_t structure */
    uint16_t task_list_offset;

    /** \brief Offset of top of stack in nano_os_task_t structure */
    uint8_t top_of_stack_offset;
    /** \brief Offset of stack origin in nano_os_task_t structure */
    uint8_t stack_origin_offset;
    /** \brief Offset of stack size in nano_os_task_t structure */
    uint8_t stack_size_offset;
    /** \brief Offset of task name in nano_os_task_t structure */
    uint8_t task_name_offset;
    /** \brief Offset of task state in nano_os_task_t structure */
    uint8_t task_state_offset;
    /** \brief Offset of task priority in nano_os_task_t structure */
    uint8_t task_priority_offset;
    /** \brief Offset of task id in nano_os_task_t structure */
    uint8_t task_id_offset;
    /** \brief Offset of task wait object in nano_os_task_t structure */
    uint8_t task_wait_object_offset;
    /** \brief Offset of task wait timeout in nano_os_task_t structure */
    uint8_t task_wait_timeout_offset;
    /** \brief Offset of task time slice in nano_os_task_t structure */
    uint8_t task_time_slice_offset;
    /** \brief Offset of next task in nano_os_task_t structure */
    uint8_t next_task_offset;
    /** \brief Offset of the port specific data in nano_os_task_t structure */
    uint8_t task_port_data_offset;

    /** \brief Offset of wait object type in nano_os_wait_object_t structure */
    uint8_t wait_object_type_offset;
    /** \brief Offset of wait object id in nano_os_wait_object_t structure */
    uint8_t wait_object_id_offset;
    /** \brief Offset of wait object name in nano_os_wait_object_t structure */
    uint8_t wait_object_name_offset;

} nano_os_segger_gdb_rtos_plugin_info_t;


/* Debug informations for Segger GDB RTOS plugin */
nano_os_segger_gdb_rtos_plugin_info_t g_nano_os_debug_infos;

/** \brief Macro to compute a field offset within a structure */
#define NANO_OS_FIELD_OFFSET(cast, structure, field)     NANO_OS_CAST(cast, NANO_OS_CAST(uint32_t, &structure.field) - NANO_OS_CAST(uint32_t, &structure))

/** \brief Macro to compute an 8 bits field offset within a structure */
#define NANO_OS_FIELD_OFFSET8(structure, field)    NANO_OS_FIELD_OFFSET(uint8_t, structure, field)

/** \brief Macro to compute a 16 bits field offset within a structure */
#define NANO_OS_FIELD_OFFSET16(structure, field)    NANO_OS_FIELD_OFFSET(uint16_t, structure, field)

/** \brief Invalid 8 bits field offset */
#define NANO_OS_INVALID_FIELD_OFFSET8   0xFFu

/** \brief Invalid 16 bits field offset */
#define NANO_OS_INVALID_FIELD_OFFSET16  0xFFFFu


/** \brief Initialize debug informations for Segger GDB RTOS plugin */
static void NANO_OS_InitSeggerGdbRtosPluginInfos(void)
{
    /* Port name */
    g_nano_os_debug_infos.port_name = NANO_OS_PORT_NAME;

    /* Offsets in nano_os_t structure */
    g_nano_os_debug_infos.current_task_offset = NANO_OS_FIELD_OFFSET16(g_nano_os, current_task);
    g_nano_os_debug_infos.tick_count_offset = NANO_OS_FIELD_OFFSET16(g_nano_os, tick_count);
    g_nano_os_debug_infos.task_list_offset = NANO_OS_FIELD_OFFSET16(g_nano_os, tasks);

    /* Offsets in nano_os_task_t structure */
    g_nano_os_debug_infos.top_of_stack_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, top_of_stack);
    g_nano_os_debug_infos.stack_origin_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, stack_origin);
    g_nano_os_debug_infos.stack_size_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, stack_size);
    #if (NANO_OS_TASK_NAME_ENABLED == 1u)
    g_nano_os_debug_infos.task_name_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, name);
    #else
    g_nano_os_debug_infos.task_name_offset = NANO_OS_INVALID_FIELD_OFFSET8;
    #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
    g_nano_os_debug_infos.task_state_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, state);
    g_nano_os_debug_infos.task_priority_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, priority);
    #if (NANO_OS_TASK_ID_ENABLED == 1u)
    g_nano_os_debug_infos.task_id_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, object_id);
    #else
    g_nano_os_debug_infos.task_name_offset = NANO_OS_INVALID_FIELD_OFFSET8;
    #endif /* (NANO_OS_TASK_ID_ENABLED == 1u) */
    g_nano_os_debug_infos.task_wait_object_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, waiting_object);
    g_nano_os_debug_infos.task_wait_timeout_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, wait_timeout);
    g_nano_os_debug_infos.task_time_slice_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, timeslice);
    g_nano_os_debug_infos.next_task_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, global_next);
    #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
    g_nano_os_debug_infos.task_port_data_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.idle_task, port_data);
    #else
    g_nano_os_debug_infos.task_port_data_offset = NANO_OS_INVALID_FIELD_OFFSET8;
    #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */

    /* Offsets in nano_os_wait_object_type_t structure */
    g_nano_os_debug_infos.wait_object_type_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.isr_request_task_wait_object, type);
    #if (NANO_OS_TRACE_ENABLED == 1u)
    g_nano_os_debug_infos.wait_object_id_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.isr_request_task_wait_object, object_id);
    #else
    g_nano_os_debug_infos.wait_object_id_offset = NANO_OS_INVALID_FIELD_OFFSET8;
    #endif /* (NANO_OS_TRACE_ENABLED == 1u) */
    #if (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u)
    g_nano_os_debug_infos.wait_object_name_offset = NANO_OS_FIELD_OFFSET8(g_nano_os.isr_request_task_wait_object, name);
    #else
    g_nano_os_debug_infos.wait_object_name_offset = NANO_OS_INVALID_FIELD_OFFSET8;
    #endif /* (NANO_OS_WAIT_OBJECT_NAME_ENABLED == 1u) */
}




#endif /* (NANO_OS_SEGGER_GDB_RTOS_PLUGIN_ENABLED == 1u) */

