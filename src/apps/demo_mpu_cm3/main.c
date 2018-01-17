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
#include "bsp.h"
#include "tasks.h"


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
