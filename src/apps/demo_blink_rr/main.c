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

/** \brief Stack size in number of elements */
#define TASK_STACK_SIZE     128u


/** \brief Main task stack */
static nano_os_stack_t main_task_stack[TASK_STACK_SIZE];

/** \brief Main task context */
static nano_os_task_t main_task;

/** \brief Main task */
static void* MAIN_Task(void* param);



/** \brief LED tasks stack */
static nano_os_stack_t led_task_stack[3u][TASK_STACK_SIZE];

/** \brief LED tasks context */
static nano_os_task_t led_task[3u];

/** \brief LED tasks timers */
static nano_os_timer_t led_task_timer[3u];

/** \brief LED tasks */
static void* LED_Task(void* param);



/** \brief Main task */
static void* MAIN_Task(void* param)
{
    bool led_on = true;

    /* Get parameter */
    const uint32_t led_id = NANO_OS_CAST(uint32_t, param);

    /* Task loop */
    while(true)
    {
        /* Update the LED state */
        if (led_on)
        {
            NANO_OS_BSP_LedOn(led_id);
        }
        else
        {
            NANO_OS_BSP_LedOff(led_id);
        }
        led_on = !led_on;

        /* Wait for the next period */
        NANO_OS_TASK_Sleep(NANO_OS_MS_TO_TICKS(500u));
    }

    return NULL;
}

/** \brief LED task */
static void* LED_Task(void* param)
{
    bool led_on = true;
    bool has_elasped = false;

    /* Get parameter */
    const uint32_t task_id = NANO_OS_CAST(uint32_t, param);
    const uint32_t led_id = task_id + 1u;

    /* Create the timer */
    (void)NANO_OS_TIMER_Create(&led_task_timer[task_id], NULL, NULL);
    (void)NANO_OS_TIMER_Start(&led_task_timer[task_id], NANO_OS_MS_TO_TICKS(250u * led_id), NANO_OS_MS_TO_TICKS(250u * led_id));

    /* Task loop */
    while(true)
    {
        /* Update the LED state */
        if (led_on)
        {
            NANO_OS_BSP_LedOn(led_id);
        }
        else
        {
            NANO_OS_BSP_LedOff(led_id);
        }
        led_on = !led_on;

        /* Wait for the next period */
        do
        {
            (void)NANO_OS_TIMER_HasElapsed(&led_task_timer[task_id], &has_elasped);
        }
        while (!has_elasped);
    }

    return NULL;
}

/** \brief Application entry point */
int main(void)
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
            uint32_t i;
            /* Create the LED tasks */
            for (i = 0; (i < 3) && (ret == NOS_ERR_SUCCESS); i++)
            {
                (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
                #if (NANO_OS_TASK_NAME_ENABLED == 1u)
                task_init_data.name = "LED task";
                #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
                task_init_data.base_priority = 4u;
                task_init_data.stack_origin = led_task_stack[i];
                task_init_data.stack_size = TASK_STACK_SIZE;
                task_init_data.task_func = LED_Task;
                task_init_data.param = NANO_OS_CAST(void*, i);
                ret = NANO_OS_TASK_Create(&led_task[i], &task_init_data);
            }
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
                task_init_data.param = NANO_OS_CAST(void*, 0u);
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

    return 0;
}
