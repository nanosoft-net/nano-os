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
#include "nano_os_port_mpu.h"
#include "nano_os_scheduler.h"
#include "nano_os_port_user.h"


/** \brief Address of the Coprocessor Access Control Register */
#define CACR_REG                (* NANO_OS_CAST(volatile uint32_t*, 0xe000ed88))

/** \brief Address of the Floating-point Context Control Register */
#define FPCCR_REG               (* NANO_OS_CAST(volatile uint32_t*, 0xe000ef34))


/** \brief Address of the Systick control register */
#define SYSTICK_CTRL_REG       (* NANO_OS_CAST(volatile uint32_t*, 0xe000e010))

/** \brief Address of the Systick load value register */
#define SYSTICK_LOAD_REG       (* NANO_OS_CAST(volatile uint32_t*, 0xe000e014))

/** \brief Address of the Systick current value register */
#define SYSTICK_CVR_REG        (* NANO_OS_CAST(volatile uint32_t*, 0xe000e018))

/** \brief Switch the CPU to priviledged mode */
extern void NANO_OS_PORT_SwitchToPriviledgedMode(void);

/** \brief Port specific first task context switch */
extern nano_os_error_t NANO_OS_PORT_FirstContextSwitchAsm(void);



/** \brief Port specific initialization */
nano_os_error_t NANO_OS_PORT_Init(nano_os_port_init_data_t* const port_init_data)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (port_init_data != NULL)
    {
        /* Switch to priviledged mode */
        NANO_OS_PORT_SwitchToPriviledgedMode();

        /* Disable interrupts */
        NANO_OS_PORT_DISABLE_INTERRUPTS();

        /* Enable FPU in both priviledged and unpriviledged modes */
        CACR_REG = (3u << 20u) | (3u << 22u);

        /* Floating point context enabled based on CONTROL reg setting */
        FPCCR_REG |= (1u << 31u);

        /* Initialize port data */
        (void)MEMSET(&port_init_data->idle_task_init_data, 0, sizeof(nano_os_port_task_init_data_t));
        ret = NANO_OS_PORT_USER_GetIdleTaskConfig(&port_init_data->idle_task_init_data, &g_nano_os.idle_task_stack);
        if (ret == NOS_ERR_SUCCESS)
        {
            port_init_data->idle_task_init_data.use_fpu = false;
            port_init_data->idle_task_init_data.is_priviledged = false;
        }
        if (ret == NOS_ERR_SUCCESS)
        {
            (void)MEMSET(&port_init_data->isr_request_task_init_data, 0, sizeof(nano_os_port_task_init_data_t));
            ret = NANO_OS_PORT_USER_GetInterruptServiceRequestTaskConfig(&port_init_data->isr_request_task_init_data, &g_nano_os.isr_request_task_stack);
        }
        if (ret == NOS_ERR_SUCCESS)
        {
            port_init_data->isr_request_task_init_data.use_fpu = false;
            port_init_data->isr_request_task_init_data.is_priviledged = true;
        }
        #if (NANO_OS_TIMER_ENABLED == 1u)
        if (ret == NOS_ERR_SUCCESS)
        {
            (void)MEMSET(&port_init_data->timer_task_init_data, 0, sizeof(nano_os_port_task_init_data_t));
            ret = NANO_OS_PORT_USER_GetTimerTaskConfig(&port_init_data->timer_task_init_data, &g_nano_os.timer_task_stack);
        }
        if (ret == NOS_ERR_SUCCESS)
        {
            port_init_data->timer_task_init_data.use_fpu = false;
            port_init_data->timer_task_init_data.is_priviledged = true;
        }
        #endif /* (NANO_OS_TIMER_ENABLED == 1u) */
        #if (NANO_OS_CONSOLE_ENABLED == 1u)
        if (ret == NOS_ERR_SUCCESS)
        {
            (void)MEMSET(&port_init_data->console_task_init_data, 0, sizeof(nano_os_port_task_init_data_t));
            ret = NANO_OS_PORT_USER_GetConsoleTaskConfig(&port_init_data->console_task_init_data);
        }
        #endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */
        #if (NANO_OS_DEBUG_ENABLED == 1u)
        if (ret == NOS_ERR_SUCCESS)
        {
            (void)MEMSET(&port_init_data->debug_task_init_data, 0, sizeof(nano_os_port_task_init_data_t));
            ret = NANO_OS_PORT_USER_GetDebugTaskConfig(&port_init_data->debug_task_init_data);
        }
        #endif /* (NANO_OS_DEBUG_ENABLED == 1u) */


        /* Configure global MPU regions */
        if (ret == NOS_ERR_SUCCESS)
        {
            ret = NANO_OS_PORT_USER_GetGlobalMpuConfig(g_nano_os.port_data.common_regions);
        }

        /* Initialize MPU */
        if (ret == NOS_ERR_SUCCESS)
        {
            ret = NANO_OS_PORT_MPU_Init();
            if (ret == NOS_ERR_SUCCESS)
            {
                uint8_t i;

                /* Finalize global MPU region configuration */
                for(i = 0; i < NANO_OS_PORT_COMMON_MPU_REGION_COUNT; i++)
                {
                    /* Check if the region is enabled */
                    if ((g_nano_os.port_data.common_regions[i].base_address != 0u) ||
                        (g_nano_os.port_data.common_regions[i].attributes != 0u))
                    {
                        const uint8_t region_number = i;

                        /* Add the VALID bit and the region number to the base address field */
                        g_nano_os.port_data.common_regions[i].base_address |= ((1u << 4u) | region_number);

                        /* Add the ENABLE bit to the attributes */
                        g_nano_os.port_data.common_regions[i].attributes |= 0x01u;
                    }
                }

                /* Check if the port provides the system timer */
                #if (NANO_OS_PORT_PROVIDES_SYSTEM_TIMER == 1u)
                {
                    /* Configure and start systick */
                    const uint32_t systick_input_freq_hz = NANO_OS_PORT_USER_GetSystickInputClockFreq();
                    SYSTICK_LOAD_REG = (systick_input_freq_hz / NANO_OS_TICK_RATE_HZ) - 1u;
                    SYSTICK_CTRL_REG = 0x07u;
                }
                #else

                /* Configure and start system timer */
                ret = NANO_OS_USER_SystemTimerInit(NANO_OS_TickInterrupt);

                #endif /* (NANO_OS_PORT_PROVIDES_SYSTEM_TIMER == 1u) */
            }
        }
    }

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
        uint8_t i;

        /* Compute top of stack (full descending stack on Cortex-M7 processors) */
        task->top_of_stack = task->stack_origin + task_init_data->stack_size;

        /* First part = exception stack frame */

        /* FPU registers */
        task->port_data.use_fpu = task_init_data->port_init_data.use_fpu;
        if (task->port_data.use_fpu)
        {
            task->top_of_stack--;
            task->top_of_stack[0] = 0x00000000u;                       /* FPSR  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x00000000u;                       /* FPSR  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x0F0F0F0Fu;                       /* S15  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x0E0E0E0Eu;                       /* S14  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x0D0D0D0Du;                       /* S13  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x0C0C0C0Cu;                       /* S12  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x0B0B0B0Bu;                       /* S11  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x0A0A0A0Au;                       /* S10  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x09090909u;                       /* S09  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x08080808u;                       /* S08  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x07070707u;                       /* S07  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x06060606u;                       /* S06  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x05050505u;                       /* S05  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x04040404u;                       /* S04  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x03030303u;                       /* S03  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x02020202u;                       /* S02  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x01010101u;                       /* S01  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x00000000u;                       /* S00  */
        }

        /* Core registers */
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
        
        /* Control register */
        task->top_of_stack--;
        task->port_data.is_priviledged = task_init_data->port_init_data.is_priviledged;
        if (task->port_data.is_priviledged)
        {
            task->top_of_stack[0] = 0x00000000u;
        }
        else
        {
            task->top_of_stack[0] = 0x00000001u;
        }
        if (task->port_data.use_fpu)
        {
            task->top_of_stack[0] |= (1u << 2u);
        }

        /* FPU registers */
        if (task->port_data.use_fpu)
        {
            task->top_of_stack--;
            task->top_of_stack[0] = 0x1F1F1F1Fu;                       /* S31  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x1E1E1E1Eu;                       /* S30  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x1D1D1D1Du;                       /* S29  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x1C1C1C1Cu;                       /* S28  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x1B1B1B1Bu;                       /* S27  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x1A1A1A1Au;                       /* S26  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x19191919u;                       /* S25  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x18181818u;                       /* S24  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x17171717u;                       /* S23  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x16161616u;                       /* S22  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x15151515u;                       /* S21  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x14141414u;                       /* S20  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x13131313u;                       /* S19  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x12121212u;                       /* S18  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x11111111u;                       /* S17  */
            task->top_of_stack--;
            task->top_of_stack[0] = 0x10101010u;                       /* S16  */
        }

        /* Finalize MPU region configuration for the task */
        (void)MEMCPY(task->port_data.mem_regions, task_init_data->port_init_data.mem_regions, sizeof(task->port_data.mem_regions));
        for(i = 0; i < NANO_OS_PORT_TASK_MPU_REGION_COUNT; i++)
        {
            const uint8_t region_number = NANO_OS_PORT_COMMON_MPU_REGION_COUNT + i;

            /* Check if the region is enabled */
            if ((task->port_data.mem_regions[i].base_address != 0u) ||
                (task->port_data.mem_regions[i].attributes != 0u))
            {
                /* Add the ENABLE bit to the attributes */
                task->port_data.mem_regions[i].attributes |= 0x01u;
            }
            else
            {
                /* Disable the region */
                task->port_data.mem_regions[i].attributes = 0x00u;
            }

            /* Add the VALID bit and the region number to the base address field */
            task->port_data.mem_regions[i].base_address |= ((1u << 4u) | region_number);
        }

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/* Check if the port provides the system timer */
#if (NANO_OS_PORT_PROVIDES_SYSTEM_TIMER == 1u)

#if ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u))

/** \brief Get the current timestamp in µs */
uint32_t NANO_OS_PORT_GetTimestampInUs(void)
{
    uint32_t us_timestamp;
    uint32_t counter_value;
    uint32_t tick_count;
    const uint32_t load_reg = SYSTICK_LOAD_REG;

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
    us_timestamp = ((load_reg - counter_value) * (1000000u / NANO_OS_TICK_RATE_HZ)) / load_reg;
    us_timestamp += (NANO_OS_TICKS_TO_MS(tick_count) * 1000u);

    return us_timestamp;
}

#endif /* ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)) */


/** \brief Port specific first task context switch */
nano_os_error_t NANO_OS_PORT_FirstContextSwitch(void)
{
    uint8_t i;
    nano_os_error_t ret;

    /* Apply global MPU region configuration */
    for(i = 0; i < NANO_OS_PORT_COMMON_MPU_REGION_COUNT; i++)
    {
        /* Check if the region is enabled */
        if ((g_nano_os.port_data.common_regions[i].base_address != 0u) ||
            (g_nano_os.port_data.common_regions[i].attributes != 0u))
        {
            const uint8_t region_number = i;

            /* Add the VALID bit and the region number to the base address field */
            g_nano_os.port_data.common_regions[i].base_address |= ((1u << 4u) | region_number);

            /* Add the ENABLE bit to the attributes */
            g_nano_os.port_data.common_regions[i].attributes |= 0x01u;

            /* Apply configuration */
            NANO_OS_MPU_ConfigureRegion(g_nano_os.port_data.common_regions[i].base_address,
                                        g_nano_os.port_data.common_regions[i].attributes);
        }
    }

    /* Enable MPU */
    NANO_OS_MPU_Enable();

    /* Call next steps of the first context switch written in assembly language */
    ret = NANO_OS_PORT_FirstContextSwitchAsm();

    return ret;
}


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

#endif /* (NANO_OS_PORT_PROVIDES_SYSTEM_TIMER == 1u) */
