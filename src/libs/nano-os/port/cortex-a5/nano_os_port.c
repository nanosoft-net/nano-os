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
#include "nano_os_user.h"
#include "nano_os_tools.h"
#include "nano_os_trace.h"
#include "nano_os_interrupt.h"
#include "nano_os_scheduler.h"



/** \brief Switch the CPU to priviledged mode */
extern void NANO_OS_PORT_SwitchToPriviledgedMode(void);


/** \brief Port specific initialization */
nano_os_error_t NANO_OS_PORT_Init(nano_os_port_init_data_t* const port_init_data)
{
    nano_os_error_t ret;
    NANO_OS_UNUSED(port_init_data);

    /* Switch to priviledged mode */
    NANO_OS_PORT_SwitchToPriviledgedMode();

    /* Disable interrupts */
    NANO_OS_PORT_DISABLE_INTERRUPTS();

    /* Initialize port data */
    (void)MEMSET(port_init_data, 0, sizeof(nano_os_port_init_data_t));
    port_init_data->isr_request_task_init_data.is_priviledged = true;
    #if (NANO_OS_TIMER_ENABLED == 1u)
    port_init_data->timer_task_init_data.is_priviledged = true;
    #endif /* (NANO_OS_TIMER_ENABLED == 1u) */

    /* Configure and start system timer */
    ret = NANO_OS_USER_SystemTimerInit(NANO_OS_TickInterrupt);

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
        /* Compute top of stack (full descending stack on Cortex-A5 processors) */
        task->top_of_stack = task->stack_origin + task_init_data->stack_size;

        /* Ensure 8 byte stack alignment */
        task->top_of_stack = NANO_OS_CAST(nano_os_stack_t*, NANO_OS_CAST(uint32_t, task->top_of_stack) & ~(0x07u));

        /* First part = exception stack frame */
        task->top_of_stack--;
        task->top_of_stack[0] = NANO_OS_CAST(nano_os_stack_t, NANO_OS_TASK_Start);          /* PC  */
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
        /* CPSR */
        task->top_of_stack--;
        task->port_data.is_priviledged = task_init_data->port_init_data.is_priviledged;
        if (task->port_data.is_priviledged)
        {
            task->top_of_stack[0] = 0x0000001Fu;
        }
        else
        {
            task->top_of_stack[0] = 0x00000010u;
        }


        /* Second part = rest of registers saving */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x0B0B0B0Bu;                      /* R11 */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x0A0A0A0Au;                      /* R10 */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x09090909u;                      /* R9  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x08080808u;                      /* R8  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x07070707u;                      /* R7  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x06060606u;                      /* R6  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x05050505u;                      /* R5  */
        task->top_of_stack--;
        task->top_of_stack[0] = 0x04040404u;                      /* R4  */

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

/** \brief Port specific interrupt level context switch */
void NANO_OS_PORT_ContextSwitchFromIsr(void)
{
    /* Indicate that a context switch is needed at the end of the ISR */
    g_nano_os.port_data.isr_context_switch_needed = true;
}
