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

#include "nano_os_api.h"
#include "bsp.h"
#include "defect.h"
#include "ipc_task.h"
#include "heartbeat_task.h"

/** \brief Stack size in number of elements */
#define TASK_STACK_SIZE     128u


/** \brief Main task stack */
static nano_os_stack_t main_task_stack[TASK_STACK_SIZE];

/** \brief Main task context */
static nano_os_task_t main_task;

/** \brief Main task */
static void* MAIN_Task(void* param);



/** \brief LED task stack */
static nano_os_stack_t led_task_stack[TASK_STACK_SIZE];

/** \brief LED task context */
static nano_os_task_t led_task;

/** \brief LED task */
static void* LED_Task(void* param);


/** \brief Task synchronization semaphore */
static nano_os_semaphore_t synchro_sem;



/** \brief Main task */
static void* MAIN_Task(void* param)
{
    /* Get parameter */
    const uint32_t period = NANO_OS_CAST(uint32_t, param);

    /* Initialize defects */
    DEFECT_Init();

    /* Initialize IPC */
    IPC_TASK_Init();

    /* Initialize heartbeat */
    HEARTBEAT_TASK_Init();

    /* Create synchro semaphore */
    NANO_OS_SEMAPHORE_Create(&synchro_sem, 0u, 1u, QT_PRIORITY);

    /* Task loop */
    while(true)
    {
        /* Wake up the LED task */
        NANO_OS_SEMAPHORE_Post(&synchro_sem);

        /* Wait for the next period */
        NANO_OS_TASK_Sleep(NANO_OS_MS_TO_TICKS(period));
    }

    return NULL;
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
        NANO_OS_SEMAPHORE_Wait(&synchro_sem, 0xFFFFFFFFu);

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

/** \brief Application entry point */
void main(void)
{
    nano_os_error_t ret;
    nano_os_task_init_data_t task_init_data;

    /* Initialize board */
    ret = NANO_OS_BSP_Init();
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Initialize operating system */
        ret = NANO_OS_Init();
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Create the LED task */
            (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            task_init_data.name = "LED task";
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
            task_init_data.base_priority = 4u;
            task_init_data.stack_origin = led_task_stack;
            task_init_data.stack_size = TASK_STACK_SIZE;
            task_init_data.task_func = LED_Task;
            task_init_data.param = NULL;
            ret = NANO_OS_TASK_Create(&led_task, &task_init_data);
            if (ret == NOS_ERR_SUCCESS)
            {
                /* Create the main task */
                #if (NANO_OS_TASK_NAME_ENABLED == 1u)
                task_init_data.name = "Main task";
                #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
                task_init_data.base_priority = 5u;
                task_init_data.stack_origin = main_task_stack;
                task_init_data.stack_size = TASK_STACK_SIZE;
                task_init_data.task_func = MAIN_Task;
                task_init_data.param = NANO_OS_CAST(void*, 500u);
                task_init_data.port_init_data.is_priviledged = true;
                ret = NANO_OS_TASK_Create(&main_task, &task_init_data);
                if (ret == NOS_ERR_SUCCESS)
                {
                    /* Start operating system */
                    ret = NANO_OS_Start();
                }
            }
        }
    }

    /* If we are here, an error happened during initialization */
    while(true)
    {}
}
