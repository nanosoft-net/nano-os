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

#include "nano_os_segger_rtt_link_cfg.h"

/* Check if module is enabled */
#if (NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 1u)

#include "SEGGER_RTT.h"
#include "nano_os_user.h"
#include "nano_os_port.h"


#if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

#include "nano_os_segger_rtt_link_multiplexer.h"

#else

/** \brief Buffer to send real time trace packet */
static uint8_t nano_os_segger_rtt_link_rtt_packet_buffer[64u];

#endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */


/** \brief Initialize the user module which will send/receive the trace packets */
nano_os_error_t NANO_OS_USER_TraceInit(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Initialize SEGGER library */
    SEGGER_RTT_Init();

    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)
    /* Initialize the multiplexer */
    ret = NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Init();
    #else
    /* Configure buffers */
    {
        int32_t err;
        err = SEGGER_RTT_ConfigUpBuffer(NANO_OS_TRACE_RTT_PACKET_BUFFER,
                                        "Nano OS Real Time Trace",
                                        nano_os_segger_rtt_link_rtt_packet_buffer,
                                        sizeof(nano_os_segger_rtt_link_rtt_packet_buffer),
                                        SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        if (err != 0)
        {
            ret = NOS_ERR_FAILURE;
        }
    }
    #endif /* #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

    return ret;
}


/** \brief Send a real time trace packet */
void NANO_OS_USER_TraceSendRttPacket(const void* const rtt_packet, const size_t packet_size)
{
    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

    /* Send trace packet */
    (void)NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Send(NANO_OS_TRACE_RTT_PACKET_BUFFER, rtt_packet, packet_size);

    #else

    /* Disable interrupts to protect against trace data sent from interrupt handlers */
    nano_os_int_status_reg_t int_status_reg;
    NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

    /* Send trace packet */
    SEGGER_RTT_WriteWithOverwriteNoLock(NANO_OS_TRACE_RTT_PACKET_BUFFER, rtt_packet, packet_size);

    /* Restore interrupts */
    NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);

    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */
}

#endif /* (NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 1u) */
