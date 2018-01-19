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
#include "nano_os_port_mpu.h"

#include "bsp.h"
#include "tasks.h"
#include "memory_regions.h"

/** \brief Task synchronization semaphore */
nano_os_semaphore_t g_synchro_sem;

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
            /* Create the LED task */
            ret = LED_TASK_Init();
            if (ret == NOS_ERR_SUCCESS)
            {
                /* Create the main task */
                ret = MAIN_TASK_Init();
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
        mpu_regions[0u].base_address = NANO_OS_CAST(uint32_t, _OS_VAR_START_);
        ret = NANO_OS_MPU_ComputeRegionAttributes(&mpu_regions[0u].attributes,
                                                  true,
                                                  NANO_OS_PORT_MPU_ATTR_AP_UNPRIVILEDGED_READ_ONLY,
                                                  NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                  false,
                                                  NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                  (NANO_OS_CAST(uint32_t, _OS_VAR_END_) - NANO_OS_CAST(uint32_t, _OS_VAR_START_)));

        /* Allow full access to code shared by all tasks */
        mpu_regions[1u].base_address = NANO_OS_CAST(uint32_t, _COMMON_CODE_START_);
        ret |= NANO_OS_MPU_ComputeRegionAttributes(&mpu_regions[1u].attributes,
                                                   false,
                                                   NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                                   NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                   false,
                                                   NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                   (NANO_OS_CAST(uint32_t, _COMMON_CODE_END_) - NANO_OS_CAST(uint32_t, _COMMON_CODE_START_)));

        /* Allow full access to variables shared by all tasks */
        mpu_regions[2u].base_address = NANO_OS_CAST(uint32_t, _COMMON_DATA_START_);
        ret |= NANO_OS_MPU_ComputeRegionAttributes(&mpu_regions[2u].attributes,
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

