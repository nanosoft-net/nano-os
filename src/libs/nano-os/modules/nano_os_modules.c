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

#include "nano_os_modules.h"

/* Check if module is enabled */
#if (NANO_OS_MODULES_ENABLED == 1u)

#include "nano_os_console.h"
#include "nano_os_debug.h"
#include "nano_os_stats.h"

/** \brief Initialize the modules */
nano_os_error_t NANO_OS_MODULES_Init(const nano_os_port_init_data_t* const port_init_data)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    #if (NANO_OS_CONSOLE_ENABLED == 1u)
    /* Initialize the console module */
    if (ret == NOS_ERR_SUCCESS)
    {
        ret = NANO_OS_CONSOLE_Init(port_init_data);
    }
    #endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */

    #if (NANO_OS_DEBUG_ENABLED == 1u)
    /* Initialize the debug module */
    if (ret == NOS_ERR_SUCCESS)
    {
        ret = NANO_OS_DEBUG_Init(port_init_data);
    }
    #endif /* (NANO_OS_DEBUG_ENABLED == 1u) */

    #if (NANO_OS_STATS_ENABLED == 1u)
    /* Initialize the statistics module */
    if (ret == NOS_ERR_SUCCESS)
    {
        ret = NANO_OS_STATS_Init();
    }
    #endif /* (NANO_OS_STATS_ENABLED == 1u) */

    return ret;
}


#endif /* (NANO_OS_MODULES_ENABLED == 1u) */
