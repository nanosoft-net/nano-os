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


/** \brief Enable Nano OS debug on UART */
#define BSP_LPC11C24_UART_DEBUG_ENABLED   1u


/* Check if module is enabled */
#if (BSP_LPC11C24_UART_DEBUG_ENABLED == 1u)

#include "uart.h"
#include "nano_os.h"
#include "nano_os_user.h"
#include "nano_os_port.h"
#include "nano_os_tools.h"


/** \brief Initialize the hardware drivers of the user module which will send/receive the debug packets */
nano_os_error_t NANO_OS_USER_DebugHwInit(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Initialize UART */
    UART_Init();

    return ret;
}

/** \brief Initialize the user module which will send/receive the debug packets */
nano_os_error_t NANO_OS_USER_DebugInit(void)
{
    /* Nothing to do */
    return NOS_ERR_SUCCESS;
}

/** \brief Wait forever for an incoming byte on the debug link (this function must be blocking) */
nano_os_error_t NANO_OS_USER_DebugReadByte(uint8_t* const c)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Wait for a byte on the UART */
    UART_Receive(c, 1u);

    return ret;
}

/** \brief Initialize a new packet to send on the debug link */
nano_os_error_t NANO_OS_USER_DebugInitPacket(const size_t packet_size)
{
    /* Nothing to do */
    NANO_OS_UNUSED(packet_size);
    return NOS_ERR_SUCCESS;
}

/** \brief Fill current packet with data to send on the debug link */
nano_os_error_t NANO_OS_USER_DebugWriteToPacket(const uint8_t* const data, const size_t size)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Send the string on the UART */
    UART_Send(data, size);

    return ret;
}

/** \brief Finalize the current packet to send on the debug link */
nano_os_error_t NANO_OS_USER_DebugFinalizePacket(void)
{
    /* Nothing to do */
    return NOS_ERR_SUCCESS;
}


#endif /* (BSP_LPC11C24_UART_DEBUG_ENABLED == 1u) */
