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
#include "memory_regions.h"

/* Check if module is enabled */
#if (NANO_OS_DEBUG_ENABLED == 1u)


/** \brief Get debug task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetDebugTaskConfig(nano_os_port_task_init_data_t* const port_init_data)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (port_init_data != NULL)
    {
        uint32_t uart_regs, uart_regs_size;

        /* Allow read-only access to the whole RAM area to enable the retrieval of task and sync objects infos in the whole system */
        port_init_data->mem_regions[0u].base_address = NANO_OS_CAST(uint32_t, _NANO_OS_MODULES_RO_DATA_START_);
        ret = NANO_OS_MPU_ComputeRegionAttributes(&port_init_data->mem_regions[0u].attributes,
                                                  true,
                                                  NANO_OS_PORT_MPU_ATTR_AP_UNPRIVILEDGED_READ_ONLY,
                                                  NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                  false,
                                                  NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                  (NANO_OS_CAST(uint32_t, _NANO_OS_MODULES_RO_DATA_END_) - NANO_OS_CAST(uint32_t, _NANO_OS_MODULES_RO_DATA_START_)));

        /* Configure MPU region to have access to module private data = stack + internal variables */
        port_init_data->mem_regions[1u].base_address = NANO_OS_CAST(uint32_t, _DEBUG_MODULE_VAR_START_);
        ret = NANO_OS_MPU_ComputeRegionAttributes(&port_init_data->mem_regions[1u].attributes,
                                                  true,
                                                  NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                                  NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                  false,
                                                  NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                  (NANO_OS_CAST(uint32_t, _DEBUG_MODULE_VAR_END_) - NANO_OS_CAST(uint32_t, _DEBUG_MODULE_VAR_START_)));

        /* Add a region to allow access to UART registers for access to the debug link */
        NANO_OS_BSP_GetUartIoRegistersMem(&uart_regs, &uart_regs_size);
        port_init_data->mem_regions[2u].base_address = uart_regs;
        NANO_OS_MPU_ComputeRegionAttributes(&port_init_data->mem_regions[2u].attributes,
                                            true,
                                            NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                            NANO_OS_PORT_MPU_ATTR_MEM_DEVICE_NOT_SHAREABLE,
                                            false,
                                            NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                            uart_regs_size);

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

#endif /* (NANO_OS_DEBUG_ENABLED == 1u) */
