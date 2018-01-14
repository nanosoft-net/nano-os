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



/** \brief LED task parameter */
typedef struct _led_task_param_t
{
    /** \brief LED id */
    uint8_t led_id;
    /** \brief LED on state */
    bool led_on;
} led_task_param_t;

/** \brief LED task stack */
static nano_os_stack_t led_task_stack[TASK_STACK_SIZE];

/** \brief LED task context */
static nano_os_task_t led_task;

/** \brief LED task */
static void* LED_Task(void* param);





/** \brief Main task */
static void* MAIN_Task(void* param)
{
    led_task_param_t led_task_param;
    nano_os_task_init_data_t task_init_data;

    /* Get parameter */
    const uint32_t period = NANO_OS_CAST(uint32_t, param);

    /* Initialize LED task parameters */
    led_task_param.led_id = 0u;
    led_task_param.led_on = true;

    (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
    #if (NANO_OS_TASK_NAME_ENABLED == 1u)
    task_init_data.name = "LED task";
    #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
    task_init_data.base_priority = 4u;
    task_init_data.stack_origin = led_task_stack;
    task_init_data.stack_size = TASK_STACK_SIZE;
    task_init_data.task_func = LED_Task;
    task_init_data.param = &led_task_param;

    /* Task loop */
    while(true)
    {
        void* return_value;
        uint8_t next_led_id;

        /* Create the LED task */
        NANO_OS_TASK_Create(&led_task, &task_init_data);

        /* Get the LED task result */
        NANO_OS_TASK_Join(&led_task, &return_value, NANO_OS_MS_TO_TICKS(period));
        next_led_id = NANO_OS_CAST(uint8_t, NANO_OS_CAST(uint32_t, return_value));

        /* Wait for the next period */
        NANO_OS_TASK_Sleep(NANO_OS_MS_TO_TICKS(period));

        /* Update LED task parameters */
        if (led_task_param.led_id == next_led_id)
        {
            led_task_param.led_on = !led_task_param.led_on;
        }
        else
        {
            led_task_param.led_id = next_led_id;
        }
    }

    return NULL;
}

/** \brief LED task */
static void* LED_Task(void* param)
{
    /* Get parameter */
    const led_task_param_t* task_param = NANO_OS_CAST(led_task_param_t*, param);

    /* Apply LED state and compute next LED id */
    uint8_t led_id;
    if (task_param->led_on)
    {
        NANO_OS_BSP_LedOn(task_param->led_id);
        led_id = task_param->led_id + 1u;
        if (led_id == NANO_OS_BSP_GetLedCount())
        {
            led_id = task_param->led_id;
        }
    }
    else
    {
        NANO_OS_BSP_LedOff(task_param->led_id);
        if (task_param->led_id == 0)
        {
            led_id = 0u;
        }
        else
        {
            led_id = task_param->led_id - 1u;
        }
    }

    return NANO_OS_CAST(void*, NANO_OS_CAST(uint32_t, led_id));
}

/** \brief Application entry point */
int main(void)
{
    nano_os_error_t ret;

    /* Initialize board */
    ret = NANO_OS_BSP_Init();
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Initialize operating system */
        ret = NANO_OS_Init();
        if (ret == NOS_ERR_SUCCESS)
        {
            nano_os_task_init_data_t task_init_data;

            /* Create the main task */
            (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            task_init_data.name = "Main task";
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
            task_init_data.base_priority = 5u;
            task_init_data.stack_origin = main_task_stack;
            task_init_data.stack_size = TASK_STACK_SIZE;
            task_init_data.task_func = MAIN_Task;
            task_init_data.param = NANO_OS_CAST(void*, 500u);
            ret = NANO_OS_TASK_Create(&main_task, &task_init_data);
            if (ret == NOS_ERR_SUCCESS)
            {
                /* Start operating system */
                ret = NANO_OS_Start();
            }
        }
    }

    /* If we are here, an error happened during initialization */
    while(true)
    {}

    return 0;
}
