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

#include "nano_os_data.h"
#include "nano_os_port.h"
#include "nano_os_syscall.h"
#include "nano_os_scheduler.h"


/** \brief Manage the entry of a system call */
void NANO_OS_SYSCALL_Enter(const bool nano_os_must_be_started)
{
    /* If needed, ensure that OS is started */
    if (nano_os_must_be_started)
    {
        NANO_OS_ERROR_ASSERT(g_nano_os.started, NOS_ERR_OS_NOT_STARTED);
    }

    /* No specific operation on syscall entry if OS is not started */
    if (g_nano_os.started)
    {
        /* Port specific operations */
        if (NANO_OS_PORT_ATOMIC_READ32(g_nano_os.int_nesting_count) == 0u)
        {
            NANO_OS_PORT_SYSCALL_ENTER();
        }

        /* Increment the lock count */
        NANO_OS_PORT_ATOMIC_INC32(g_nano_os.lock_count);
    }
}

/** \brief Manage the exit of a system call */
void NANO_OS_SYSCALL_Exit(void)
{
    /* No specific operation on syscall exit if OS is not started */
    if (g_nano_os.started)
    {
        nano_os_int_status_reg_t int_status_reg;

        /* Disable interrupts */
        NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

        /* Decrement the lock count */
        g_nano_os.lock_count--;

        /* If no more locks and not in interrupt, check for context switch */
        if ((g_nano_os.lock_count == 0u) && (g_nano_os.int_nesting_count == 0u))
        {
            /* Call the scheduler */
            NANO_OS_SCHEDULER_Schedule(false);
        }

        /* Enable interrupts */
        NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);

        /* Port specific operations */
        if (NANO_OS_PORT_ATOMIC_READ32(g_nano_os.int_nesting_count) == 0u)
        {
            NANO_OS_PORT_SYSCALL_EXIT();
        }
    }
}
