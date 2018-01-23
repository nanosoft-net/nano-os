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

#include "memory_regions.h"
#include "tasks.h"
#include "nano_os_api.h"
#include "nano_os_port_mpu.h"
#include "mpu_faults.h"
#include "bsp.h"
#include "nano_os_console_cmds.h"

/** \brief System control block registers */
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


/* Memory mapping of System Control Block */
#define SCS_BASE            (0xE000E000)                              /*!< System Control Space Base Address */
#define SCB_BASE            (SCS_BASE +  0x0D00)                      /*!< System Control Block Base Address */
#define SCB                 ((SCB_Type *)           SCB_BASE)         /*!< SCB configuration struct          */



/** \brief Task synchronization semaphore */
nano_os_semaphore_t g_synchro_sem;

/** \brief Triggers an MPU fault by driving an I/O from the main task */
volatile bool g_mpu_fault_io;

/** \brief Triggers an MPU fault by modifying an internal variable of Nano OS from the main task */
volatile bool g_mpu_fault_os_main;

/** \brief Triggers an MPU fault by modifying an internal variable of Nano OS from the led task */
volatile bool g_mpu_fault_os_led;

/** \brief Triggers an MPU fault by modifying an internal variable of the main task from the led task */
volatile bool g_mpu_fault_task_main;

/** \brief Triggers an MPU fault by modifying an internal variable of the led task from the main task */
volatile bool g_mpu_fault_task_led;


/** \brief Application entry point */
int main(void)
{
    nano_os_error_t ret;

    /* Redirect MPU faults to MPUFault_Handler instead of HardFault_Handler */
    SCB->SHCSR = (0x07u << 16u);

    /* Initialize board */
    ret = NANO_OS_BSP_Init();
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Initialize operating system */
        ret = NANO_OS_Init();
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Create the LED task */
            ret = LED_TASK_Init();
            if (ret == NOS_ERR_SUCCESS)
            {
                /* Create the main task */
                ret = MAIN_TASK_Init();

                #if (NANO_OS_CONSOLE_ENABLED == 1u)
                /* Initialize demo application console commands */
                if (ret == NOS_ERR_SUCCESS)
                {
                    ret = NANO_OS_CONSOLE_CMDS_Init();
                }
                #endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */

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

/** \brief Get port specific global MPU configuration */
nano_os_error_t NANO_OS_PORT_USER_GetGlobalMpuConfig(nano_os_port_mpu_region_t* const mpu_regions)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (mpu_regions != NULL)
    {
        /* Protect access to Nano OS internal variables by allowing only prvileged tasks */
        ret = NANO_OS_MPU_ComputeRegionAttributes(&mpu_regions[0u],
                                                  NANO_OS_CAST(uint32_t, _OS_VAR_START_),
                                                  true,
                                                  NANO_OS_PORT_MPU_ATTR_AP_UNPRIVILEDGED_READ_ONLY,
                                                  NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                  false,
                                                  NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                  (NANO_OS_CAST(uint32_t, _OS_VAR_END_) - NANO_OS_CAST(uint32_t, _OS_VAR_START_)));

        /* Allow full access to code shared by all tasks */
        ret |= NANO_OS_MPU_ComputeRegionAttributes(&mpu_regions[1u],
                                                   NANO_OS_CAST(uint32_t, _COMMON_CODE_START_),
                                                   false,
                                                   NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                                   NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                   false,
                                                   NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                   (NANO_OS_CAST(uint32_t, _COMMON_CODE_END_) - NANO_OS_CAST(uint32_t, _COMMON_CODE_START_)));

        /* Allow full access to variables shared by all tasks */
        ret |= NANO_OS_MPU_ComputeRegionAttributes(&mpu_regions[2u],
                                                   NANO_OS_CAST(uint32_t, _COMMON_DATA_START_),
                                                   true,
                                                   NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                                   NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                   false,
                                                   NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                   (NANO_OS_CAST(uint32_t, _COMMON_DATA_END_) - NANO_OS_CAST(uint32_t, _COMMON_DATA_START_)));

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/** \brief Hard fault handler */
void HardFault_Handler(void)
{
   /* Uncomment the following block to get more information on the cause of the exception */
   /*
   volatile uint32_t shcsr = SCB->SHCSR;
   volatile uint32_t hfsr = SCB->HFSR;
   */
   while(true)
   {}
}

/** \brief MPU fault handler */
void MPUFault_Handler(void)
{
   /* Uncomment the following block to get more information on the cause of the exception */
   /*
   volatile uint32_t shcsr = SCB->SHCSR;
   volatile uint32_t cfsr = SCB->CFSR;
   volatile uint32_t mmfar = SCB->MMFAR;
   volatile uint32_t bfar = SCB->BFAR;
   */
   while(true)
   {}
}

