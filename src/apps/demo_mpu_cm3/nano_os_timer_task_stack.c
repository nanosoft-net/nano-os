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

#include "memory_regions.h"

/* Check if module is enabled */
#if (NANO_OS_TIMER_ENABLED == 1u)


/** \brief Timer task stack */
static nano_os_stack_t timer_task_stack[NANO_OS_TIMER_TASK_STACK_SIZE];


/** \brief Get timer task port specific configuration */
nano_os_error_t NANO_OS_PORT_USER_GetTimerTaskConfig(nano_os_port_task_init_data_t* const port_init_data, nano_os_stack_t** stack_origin)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((port_init_data != NULL) && (stack_origin != NULL))
    {
        /* No need to configure any MPU region since this task will run in privileged mode and
         * and so will gain access to the whole memory map.
         */

        /* Configure stack origin */
        (*stack_origin) = timer_task_stack;

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

#endif /* (NANO_OS_TIMER_ENABLED == 1u) */
