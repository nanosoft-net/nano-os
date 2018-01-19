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

#include "tasks.h"
#include "nano_os_port_mpu.h"

/** \brief Stack size in number of elements */
#define TASK_STACK_SIZE     128u


/** \brief Main task stack */
static nano_os_stack_t main_task_stack[TASK_STACK_SIZE];

/** \brief Main task context */
static nano_os_task_t main_task;

/** \brief Main task */
static void* MAIN_Task(void* param);



/* Linker exported symbols */
extern char _MAIN_TASK_VAR_START_[];
extern char _MAIN_TASK_VAR_END_[];




/** \brief Create main task */
nano_os_error_t MAIN_TASK_Init(void)
{
    nano_os_error_t ret;
    nano_os_task_init_data_t task_init_data;

    (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
    task_init_data.name = "Main task";
    task_init_data.base_priority = 5u;
    task_init_data.stack_origin = main_task_stack;
    task_init_data.stack_size = TASK_STACK_SIZE;
    task_init_data.task_func = MAIN_Task;
    task_init_data.param = NANO_OS_CAST(void*, 500u);

    /* Configure a region for the task private data */
    task_init_data.port_init_data.mem_regions[0u].base_address = NANO_OS_CAST(uint32_t, _MAIN_TASK_VAR_START_);
    NANO_OS_MPU_ComputeRegionAttributes(&task_init_data.port_init_data.mem_regions[0u].attributes,
                                        true,
                                        NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                        NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                        false,
                                        NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                        (NANO_OS_CAST(uint32_t, _MAIN_TASK_VAR_END_) - NANO_OS_CAST(uint32_t, _MAIN_TASK_VAR_START_)));

    task_init_data.port_init_data.mem_regions[1u].base_address = NANO_OS_CAST(uint32_t, _MAIN_TASK_VAR_START_);
    NANO_OS_MPU_ComputeRegionAttributes(&task_init_data.port_init_data.mem_regions[1u].attributes,
                                        false,
                                        NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                        NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_TROUGH_NO_WRITE_ALLOC,
                                        false,
                                        NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                        (NANO_OS_CAST(uint32_t, _MAIN_TASK_VAR_END_) - NANO_OS_CAST(uint32_t, _MAIN_TASK_VAR_START_)));

    ret = NANO_OS_TASK_Create(&main_task, &task_init_data);

    return ret;
}

/** \brief Main task */
static void* MAIN_Task(void* param)
{
    /* Get parameter */
    const uint32_t period = NANO_OS_CAST(uint32_t, param);

    /* Create synchro semaphore */
    NANO_OS_SEMAPHORE_Create(&g_synchro_sem, 0u, 1u, QT_PRIORITY);

    /* Task loop */
    while(true)
    {
        /* Wake up the LED task */
        NANO_OS_SEMAPHORE_Post(&g_synchro_sem);

        /* Wait for the next period */
        NANO_OS_TASK_Sleep(NANO_OS_MS_TO_TICKS(period));
    }

    return NULL;
}
