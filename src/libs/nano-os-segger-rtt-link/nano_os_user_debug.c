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
#if (NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED == 1u)

#include "SEGGER_RTT.h"
#include "nano_os.h"
#include "nano_os_user.h"
#include "nano_os_port.h"


#if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

#include "nano_os_segger_rtt_link_multiplexer.h"

#else

/** \brief Buffer to send debug output packet */
static uint8_t nano_os_segger_rtt_link_debug_output_packet_buffer[256u];

/** \brief Buffer to receive debug input packet */
static uint8_t nano_os_segger_rtt_link_debug_input_packet_buffer[16u];

#endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */



/** \brief Initialize the hardware drivers of the user module which will send/receive the debug packets */
nano_os_error_t NANO_OS_USER_DebugHwInit(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    #if ((NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 0u) && (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 0u))
    /* Initialize SEGGER library */
    SEGGER_RTT_Init();

    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)
    /* Initialize the multiplexer */
    ret = NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Init();
    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

    #endif /* ((NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 0u) && (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 0u)) */

    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 0u)
    /* Configure buffers */
    {
        int32_t err;
        err = SEGGER_RTT_ConfigUpBuffer(NANO_OS_DEBUG_PACKET_BUFFER,
                                        "Nano OS Debug Output",
                                        nano_os_segger_rtt_link_debug_output_packet_buffer,
                                        sizeof(nano_os_segger_rtt_link_debug_output_packet_buffer),
                                        SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
        if (err != 0)
        {
            ret = NOS_ERR_FAILURE;
        }
        err = SEGGER_RTT_ConfigDownBuffer(NANO_OS_DEBUG_PACKET_BUFFER,
                                          "Nano OS Debug Input",
                                          nano_os_segger_rtt_link_debug_input_packet_buffer,
                                          sizeof(nano_os_segger_rtt_link_debug_input_packet_buffer),
                                          SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        if (err != 0)
        {
            ret = NOS_ERR_FAILURE;
        }
    }
    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 0u) */

    return ret;
}

/** \brief Initialize the user module which will send/receive the debug packets */
nano_os_error_t NANO_OS_USER_DebugInit(void)
{
    // Nothing to do
    return NOS_ERR_SUCCESS;
}

/** \brief Wait forever for an incoming byte on the debug link (this function must be blocking) */
nano_os_error_t NANO_OS_USER_DebugReadByte(uint8_t* const c)
{
    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

    /* Read data using the multiplexer */
    const nano_os_error_t ret = NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Read(NANO_OS_DEBUG_PACKET_BUFFER, c);

    #else

    int32_t nb_byte_read;
    const nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Wait for a byte */
    do
    {
        /* Try to read a byte */
        nb_byte_read = SEGGER_RTT_ReadNoLock(NANO_OS_DEBUG_PACKET_BUFFER, c, 1u);
        if (nb_byte_read == 0u)
        {
            /* Sleep until next try */
            NANO_OS_TASK_Sleep(NANO_OS_MS_TO_TICKS(NANO_OS_SEGGER_RTT_LINK_DEBUG_POOLING_PERIOD));
        }
    }
    while (nb_byte_read == 0u);

    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

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
    const nano_os_error_t ret = NOS_ERR_SUCCESS;

    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

    /* Write using the multiplexer */
    NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Send(NANO_OS_DEBUG_PACKET_BUFFER, data, size);

    #else

    /* Write directly on RTT link */
    SEGGER_RTT_WriteWithOverwriteNoLock(NANO_OS_DEBUG_PACKET_BUFFER, data, size);

    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

    return ret;
}

/** \brief Finalize the current packet to send on the debug link */
nano_os_error_t NANO_OS_USER_DebugFinalizePacket(void)
{
    /* Nothing to do */
    return NOS_ERR_SUCCESS;
}


#endif /* (NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED == 1u) */
