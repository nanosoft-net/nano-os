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
#include "nano_os_port.h"
#include "nano_os_data.h"
#include "nano_os_tools.h"
#include "nano_os_trace.h"
#include "nano_os_interrupt.h"
#include "nano_os_scheduler.h"
#include "nano_os_port_user.h"


/** \brief Address of the Systick control register */
#define SYSTICK_CTRL_REG       (* NANO_OS_CAST(volatile uint32_t*, 0xe000e010))

/** \brief Address of the Systick load value register */
#define SYSTICK_LOAD_REG       (* NANO_OS_CAST(volatile uint32_t*, 0xe000e014))

/** \brief Address of the Systick current value register */
#define SYSTICK_CVR_REG        (* NANO_OS_CAST(volatile uint32_t*, 0xe000e018))



/** \brief Port specific initialization */
nano_os_error_t NANO_OS_PORT_Init(nano_os_port_init_data_t* const port_init_data)
{
    uint32_t systick_input_freq_hz;
    nano_os_error_t ret = NOS_ERR_SUCCESS;
    NANO_OS_UNUSED(port_init_data);

    /* Disable interrupts */
    NANO_OS_PORT_DISABLE_INTERRUPTS();

    /* Configure and start systick */
    systick_input_freq_hz = NANO_OS_PORT_USER_GetSystickInputClockFreq();
    SYSTICK_LOAD_REG = (systick_input_freq_hz / NANO_OS_TICK_RATE_HZ) - 1u;
    SYSTICK_CTRL_REG = 0x07u;

    return ret;
}


/** \brief Get the port version */
nano_os_error_t NANO_OS_PORT_GetVersion(nano_os_version_t* const port_version)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (port_version != NULL)
    {
        /* Fill Port version */
        port_version->major = 1;
        port_version->minor = 0;

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/** \brief Port specific initialization of the task context */
nano_os_error_t NANO_OS_PORT_InitTask(nano_os_task_t* const task, const nano_os_task_init_data_t* const task_init_data)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((task != NULL) &&
        (task_init_data != NULL) &&
        (task_init_data->stack_size >= NANO_OS_PORT_MIN_STACK_SIZE) &&
        (task_init_data->task_func != NULL))
    {
        /* Compute top of stack (full descending stack on Cortex-M0 processors) */
        task->top_of_stack = task->stack_origin + task_init_data->stack_size;

        /* First part = exception stack frame */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x01000000u;                                                /* PSR */
        task->top_of_stack--;
        task->top_of_stack[0] = NANO_OS_CAST(nano_os_stack_t, NANO_OS_TASK_Start);          /* PC  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x0E0E0E0Eu;                                                /* LR  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x0C0C0C0Cu;                                                /* R12 */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x03030303u;                                                /* R3  */
        task->top_of_stack--;
        task->top_of_stack[0] = NANO_OS_CAST(nano_os_stack_t, task_init_data->param);       /* R2  */
        task->top_of_stack--;
        task->top_of_stack[0] = NANO_OS_CAST(nano_os_stack_t, task_init_data->task_func);   /* R1  */
        task->top_of_stack--;
        task->top_of_stack[0] = NANO_OS_CAST(nano_os_stack_t, task);                        /* R0  */

        /* Second part = rest of registers saving */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x07070707u;                      /* R7  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x06060606u;                      /* R6  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x05050505u;                      /* R5  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x04040404u;                      /* R4  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x0B0B0B0Bu;                      /* R11 */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x0A0A0A0Au;                      /* R10 */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x09090909u;                      /* R9  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x08080808u;                      /* R8  */

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


#if ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u))

/** \brief Get the current timestamp in µs */
uint32_t NANO_OS_PORT_GetTimestampInUs(void)
{
    uint32_t us_timestamp;
    uint32_t counter_value;
    uint32_t tick_count;

    /* Read the current tick count and timer value */
    do
    {
        tick_count = g_nano_os.tick_count;
        counter_value = SYSTICK_CVR_REG;
    }
    /* If an interrupt occurred between reading the timer counter and
       reading the tick count, we do another reading.
     */
    while (tick_count != g_nano_os.tick_count);

    /* Compute the timestamp in µs */
    us_timestamp = ((SYSTICK_LOAD_REG - counter_value) * (1000000u / NANO_OS_TICK_RATE_HZ)) / SYSTICK_LOAD_REG;
    us_timestamp += (NANO_OS_TICKS_TO_MS(tick_count) * 1000u);

    return us_timestamp;
}

#endif /* ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)) */


/** \brief Handler for the Systick timer interrupt */
void NANO_OS_PORT_SystickHandler(void)
{
    /* Signal OS that we are in an interrupt handler */
    NANO_OS_INTERRUPT_Enter();

    /* Real time trace event */
    NANO_OS_TRACE_INTERRUPT_ENTRY(0u);

    /* OS tick interrupt handler */
    NANO_OS_TickInterrupt();

    /* Real time trace event */
    NANO_OS_TRACE_INTERRUPT_EXIT(0u);

    /* Signal OS that we exit the interrupt handler */
    NANO_OS_INTERRUPT_Exit();
}

