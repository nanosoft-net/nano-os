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
#include "bsp.h"
#include "nano_os_port_mpu.h"

/** \brief Stack size in number of elements */
#define TASK_STACK_SIZE     128u


/** \brief LED task stack */
static nano_os_stack_t led_task_stack[TASK_STACK_SIZE];

/** \brief LED task context */
static nano_os_task_t led_task;

/** \brief LED task */
static void* LED_Task(void* param);


/* Linker exported symbols */
extern char _LED_TASK_VAR_START_[];
extern char _LED_TASK_VAR_END_[];


/** \brief Create LED task */
nano_os_error_t LED_TASK_Init(void)
{
    nano_os_error_t ret;
    uint32_t start_address;
    uint32_t size;
    nano_os_task_init_data_t task_init_data;

    (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));

    /* Configure a region for the task private data */
    task_init_data.port_init_data.mem_regions[0u].base_address = NANO_OS_CAST(uint32_t, _LED_TASK_VAR_START_);
    NANO_OS_MPU_ComputeRegionAttributes(&task_init_data.port_init_data.mem_regions[0u].attributes,
                                        true,
                                        NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                        NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                        false,
                                        NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                        (NANO_OS_CAST(uint32_t, _LED_TASK_VAR_END_) - NANO_OS_CAST(uint32_t, _LED_TASK_VAR_START_)));

    /* Configure a region to allow access to LED I/O registers */
    NANO_OS_BSP_GetLedIoRegistersMem(&start_address, &size);
    task_init_data.port_init_data.mem_regions[1u].base_address = start_address;
    NANO_OS_MPU_ComputeRegionAttributes(&task_init_data.port_init_data.mem_regions[1u].attributes,
                                        true,
                                        NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                        NANO_OS_PORT_MPU_ATTR_MEM_DEVICE_NOT_SHAREABLE,
                                        false,
                                        NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                        size);

    /* Create the task */
    task_init_data.name = "LED task";
    task_init_data.base_priority = 4u;
    task_init_data.stack_origin = led_task_stack;
    task_init_data.stack_size = TASK_STACK_SIZE;
    task_init_data.task_func = LED_Task;
    task_init_data.param = NULL;
    ret = NANO_OS_TASK_Create(&led_task, &task_init_data);

    return ret;
}


/** \brief LED task */
static void* LED_Task(void* param)
{
    /* Init */
    uint8_t led_id = 0;
    bool led_on = true;
    NANO_OS_UNUSED(param);

    /* Task loop */
    while(true)
    {
        /* Wait for the synchronization semaphore */
        NANO_OS_SEMAPHORE_Wait(&g_synchro_sem, 0xFFFFFFFFu);

        /* Drive LEDs */
        if (led_on)
        {
            NANO_OS_BSP_LedOn(led_id);
            led_id++;
            if (led_id == NANO_OS_BSP_GetLedCount())
            {
                led_on = false;
                led_id--;
            }
        }
        else
        {
            NANO_OS_BSP_LedOff(led_id);
            led_id--;
            if (led_id > NANO_OS_BSP_GetLedCount())
            {
                led_on = true;
                led_id++;
            }
        }
    }

    return NULL;
}

