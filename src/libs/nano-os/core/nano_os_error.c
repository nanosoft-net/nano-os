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


#include "nano_os_error.h"
#include "nano_os_syscall.h"
#include "nano_os_port.h"
#include "nano_os.h"


/** \brief Nano OS error handler */
void NANO_OS_ERROR_Handler(const nano_os_error_t error)
{
    /* Error handler is also a syscall to ensure access to all protected memory */
    NANO_OS_SYSCALL_Enter(false);

    /* Disable interrupts */
    NANO_OS_PORT_DISABLE_INTERRUPTS();

    #if (NANO_OS_ERROR_HANDLER_CALLBACK_ENABLED == 1u)
    /* User callback */
    NANO_OS_USER_ErrorHandlerCallback(error);
    #endif /* (NANO_OS_ERROR_HANDLER_CALLBACK_ENABLED == 1u) */

    /* Endless loop */
    while (error != NOS_ERR_SUCCESS)
    {}
}
