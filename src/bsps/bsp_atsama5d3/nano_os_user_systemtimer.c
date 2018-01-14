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

#include "pit.h"
#include "nano_os.h"
#include "nano_os_user.h"
#include "nano_os_port.h"
#include "nano_os_tools.h"


/** \brief Initialize and start the system timer */
nano_os_error_t NANO_OS_USER_SystemTimerInit(const fp_nano_os_system_timer_callback_func_t system_timer_irq_callback)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Initialize PIT timer */
    PIT_Init(system_timer_irq_callback);

    /* Start PIT timer */
    PIT_Start();

    return ret;
}

#if (NANO_OS_TRACE_ENABLED == 1u)
/** \brief Get the current timestamp in µs */
uint32_t NANO_OS_USER_SystemTimerGetTimestampInUs(void)
{

    return 0;
}
#endif /* (NANO_OS_TRACE_ENABLED == 1u) */
