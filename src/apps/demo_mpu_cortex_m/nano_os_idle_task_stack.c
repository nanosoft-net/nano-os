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

#include "../demo_mpu_cortex_m/memory_regions.h"
#include "nano_os_api.h"
#include "nano_os_port_mpu.h"



/** \brief Idle task stack */
static nano_os_stack_t idle_task_stack[NANO_OS_IDLE_TASK_STACK_SIZE];


/** \brief Get idle task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetIdleTaskConfig(nano_os_port_task_init_data_t* const port_init_data, nano_os_stack_t** stack_origin)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((port_init_data != NULL) && (stack_origin != NULL))
    {
        /* Configure MPU region to have access to task private data = stack */
        ret = NANO_OS_MPU_ComputeRegionAttributes(&port_init_data->mem_regions[0u],
                                                  NANO_OS_CAST(uint32_t, _IDLE_TASK_VAR_START_),
                                                  true,
                                                  NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS,
                                                  NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC,
                                                  false,
                                                  NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL,
                                                  (NANO_OS_CAST(uint32_t, _IDLE_TASK_VAR_END_) - NANO_OS_CAST(uint32_t, _IDLE_TASK_VAR_START_)));

        /* Configure stack origin */
        (*stack_origin) = idle_task_stack;
    }

    return ret;
}

