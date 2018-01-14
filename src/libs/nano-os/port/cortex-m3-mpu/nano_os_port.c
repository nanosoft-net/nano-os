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

##include "nano_os.h"
#include "nano_os_port.h"
#include "nano_os_data.h"
#include "nano_os_tools.h"
#include "nano_os_trace.h"
include "nano_os_interrupt.h"
#include "nano_os_port_mpu.h"
#include "nano_os_scheduler.h"
#include "nano_os_port_stacks.h"
#include "nano_os_port_user.h"


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


typedef struct
{
    uint32_t CPUID;                        /*!< Offset: 0x00  CPU ID Base Register                                  */
    uint32_t ICSR;                         /*!< Offset: 0x04  Interrupt Control State Register                      */
    uint32_t VTOR;                         /*!< Offset: 0x08  Vector Table Offset Register                          */
    uint32_t AIRCR;                        /*!< Offset: 0x0C  Application Interrupt / Reset Control Register        */
    uint32_t SCR;                          /*!< Offset: 0x10  System Control Register                               */
    uint32_t CCR;                          /*!< Offset: 0x14  Configuration Control Register                        */
    uint8_t  SHP[12];                      /*!< Offset: 0x18  System Handlers Priority Registers (4-7, 8-11, 12-15) */
    uint32_t SHCSR;                        /*!< Offset: 0x24  System Handler Control and State Register             */
    uint32_t CFSR;                         /*!< Offset: 0x28  Configurable Fault Status Register                    */
    uint32_t HFSR;                         /*!< Offset: 0x2C  Hard Fault Status Register                            */
    uint32_t DFSR;                         /*!< Offset: 0x30  Debug Fault Status Register                           */
    uint32_t MMFAR;                        /*!< Offset: 0x34  Mem Manage Address Register                           */
    uint32_t BFAR;                         /*!< Offset: 0x38  Bus Fault Address Register                            */
    uint32_t AFSR;                         /*!< Offset: 0x3C  Auxiliary Fault Status Register                       */
    uint32_t PFR[2];                       /*!< Offset: 0x40  Processor Feature Register                            */
    uint32_t DFR;                          /*!< Offset: 0x48  Debug Feature Register                                */
    uint32_t ADR;                          /*!< Offset: 0x4C  Auxiliary Feature Register                            */
    uint32_t MMFR[4];                      /*!< Offset: 0x50  Memory Model Feature Register                         */
    uint32_t ISAR[5];                      /*!< Offset: 0x60  ISA Feature Register                                  */
} SCB_Type;
/* Memory mapping of Cortex-M3 Hardware */
#define SCS_BASE            (0xE000E000)                              /*!< System Control Space Base Address */
#define SCB_BASE            (SCS_BASE +  0x0D00)                      /*!< System Control Block Base Address */
#define SCB                 ((SCB_Type *)           SCB_BASE)         /*!< SCB configuration struct          */
void HardFault_Handler(void)
{
    volatile uint32_t shcsr = SCB->SHCSR;
    volatile uint32_t hfsr = SCB->HFSR;
    while(true)
    {}
}

void MPUFault_Handler(void)
{
    volatile uint32_t shcsr = SCB->SHCSR;
    volatile uint32_t cfsr = SCB->CFSR;
    volatile uint32_t mmfar = SCB->MMFAR;
    volatile uint32_t bfar = SCB->BFAR;
    while(true)
    {}
}


/** \brief Port specific initialization */
nano_os_error_t NANO_OS_PORT_Init(nano_os_port_init_data_t* const port_init_data)
{
	uint32_t systick_input_freq_hz;
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    SCB->SHCSR = (0x07u << 16u);
    /* Check parameters */
    if (port_init_data != NULL)
    {
        /* Switch to priviledged mode */
        NANO_OS_PORT_SwitchToPriviledgedMode();

        /* Disable interrupts */
        NANO_OS_PORT_DISABLE_INTERRUPTS();

        /* Initialize port data */
        g_nano_os.idle_task_stack = g_idle_task_stack;
        (void)MEMSET(&port_init_data->idle_task_init_data, 0, sizeof(nano_os_port_task_init_data_t));
        port_init_data->idle_task_init_data.mem_regions[0u].base_address = NANO_OS_CAST(uint32_t, _IDLE_TASK_VAR_START_);
        NANO_OS_MPU_ComputeRegionAttributes(&port_init_data->idle_task_init_data.mem_regions[0u].attributes,
                                            true,
                                            NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                            NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_TROUGH_NO_WRITE_ALLOC,
                                            true,
                                            NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                            (NANO_OS_CAST(uint32_t, _IDLE_TASK_VAR_END_) - NANO_OS_CAST(uint32_t, _IDLE_TASK_VAR_START_)));

        g_nano_os.isr_request_task_stack = g_isr_request_task_stack;
        (void)MEMSET(&port_init_data->isr_request_task_init_data, 0, sizeof(nano_os_port_task_init_data_t));
        port_init_data->isr_request_task_init_data.is_priviledged = true;
        port_init_data->isr_request_task_init_data.mem_regions[0u].base_address = NANO_OS_CAST(uint32_t, _ISR_REQUEST_TASK_VAR_START_);
        NANO_OS_MPU_ComputeRegionAttributes(&port_init_data->isr_request_task_init_data.mem_regions[0u].attributes,
                                            true,
                                            NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                            NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_TROUGH_NO_WRITE_ALLOC,
                                            true,
                                            NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                            (NANO_OS_CAST(uint32_t, _ISR_REQUEST_TASK_VAR_END_) - NANO_OS_CAST(uint32_t, _ISR_REQUEST_TASK_VAR_START_)));

        /* Configure global MPU regions */
        g_nano_os.port_data.common_regions[0u].base_address = NANO_OS_CAST(uint32_t, _OS_VAR_START_);
        NANO_OS_MPU_ComputeRegionAttributes(&g_nano_os.port_data.common_regions[0u].attributes,
                                            true,
                                            NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                            NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_TROUGH_NO_WRITE_ALLOC,
                                            true,
                                            NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                            (NANO_OS_CAST(uint32_t, _OS_VAR_END_) - NANO_OS_CAST(uint32_t, _OS_VAR_START_)));

        g_nano_os.port_data.common_regions[1u].base_address = NANO_OS_CAST(uint32_t, _COMMON_CODE_START_);
        NANO_OS_MPU_ComputeRegionAttributes(&g_nano_os.port_data.common_regions[1u].attributes,
                                            false,
                                            NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                            NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_TROUGH_NO_WRITE_ALLOC,
                                            false,
                                            NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                            (NANO_OS_CAST(uint32_t, _COMMON_CODE_END_) - NANO_OS_CAST(uint32_t, _COMMON_CODE_START_)));

        g_nano_os.port_data.common_regions[2u].base_address = NANO_OS_CAST(uint32_t, _COMMON_DATA_START_);
        NANO_OS_MPU_ComputeRegionAttributes(&g_nano_os.port_data.common_regions[2u].attributes,
                                            true,
                                            NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                            NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_TROUGH_NO_WRITE_ALLOC,
                                            true,
                                            NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                            (NANO_OS_CAST(uint32_t, _COMMON_DATA_END_) - NANO_OS_CAST(uint32_t, _COMMON_DATA_START_)));
        /* Initialize MPU */
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

            systick_input_freq_hz = NANO_OS_PORT_USER_GetSystickInputClockFreq();
            SYSTICK_LOAD_REG = (systick_input_freq_hz / NANO_OS_TICK_RATE_HZ) - 1u;
            SYSTICK_CTRL_REG = 0x07u;
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

        /* Compute top of stack (full descending stack on Cortex-M3 processors) */
        task->top_of_stack = task->stack_origin + task->stack_size;

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


#if ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u))

/** \brief Get the current timestamp in �s */
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

    /* Compute the timestamp in �s */
    us_timestamp = ((SYSTICK_LOAD_REG - counter_value) * (1000000u / NANO_OS_TICK_RATE_HZ)) / SYSTICK_LOAD_REG;
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

