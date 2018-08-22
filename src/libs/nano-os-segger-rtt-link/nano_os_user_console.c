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
#if (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 1u)

#include "SEGGER_RTT.h"

#include "nano_os_api.h"


#if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

#include "nano_os_segger_rtt_link_multiplexer.h"

#else

/** \brief Buffer to send console output packet */
static uint8_t nano_os_segger_rtt_link_console_output_packet_buffer[128u];

/** \brief Buffer to receive console input packet */
static uint8_t nano_os_segger_rtt_link_console_input_packet_buffer[32u];

#endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */



/** \brief Initialize the hardware drivers of the user module which will send/receive the console packets */
nano_os_error_t NANO_OS_USER_ConsoleHwInit(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    #if (NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 0u)
    /* Initialize SEGGER library */
    SEGGER_RTT_Init();

    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)
    /* Initialize the multiplexer */
    ret = NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Init();
    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

    #endif /* (NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 0u) */

    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 0u)
    /* Configure buffers */
    {
        int32_t err;
        err = SEGGER_RTT_ConfigUpBuffer(NANO_OS_CONSOLE_PACKET_BUFFER,
                                        "Nano OS Console Output",
                                        nano_os_segger_rtt_link_console_output_packet_buffer,
                                        sizeof(nano_os_segger_rtt_link_console_output_packet_buffer),
                                        SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
        if (err != 0)
        {
            ret = NOS_ERR_FAILURE;
        }
        err = SEGGER_RTT_ConfigDownBuffer(NANO_OS_CONSOLE_PACKET_BUFFER,
                                          "Nano OS Console Input",
                                          nano_os_segger_rtt_link_console_input_packet_buffer,
                                          sizeof(nano_os_segger_rtt_link_console_input_packet_buffer),
                                          SEGGER_RTT_MODE_NO_BLOCK_SKIP);
        if (err != 0)
        {
            ret = NOS_ERR_FAILURE;
        }
    }
    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 0u) */

    return ret;
}

/** \brief Initialize the user module which will send/receive the console packets */
nano_os_error_t NANO_OS_USER_ConsoleInit(void)
{
    // Nothing to do
    return NOS_ERR_SUCCESS;
}

/** \brief Wait forever for an incoming char on the console link (this function must be blocking) */
nano_os_error_t NANO_OS_USER_ConsoleReadChar(char* const c)
{
    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

    /* Read data using the multiplexer */
    const nano_os_error_t ret = NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Read(NANO_OS_CONSOLE_PACKET_BUFFER, NANO_OS_CAST(uint8_t*, c));

    #else

    int32_t nb_char_read;
    const nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Wait for a char */
    do
    {
        /* Try to read a char */
        nb_char_read = SEGGER_RTT_ReadNoLock(NANO_OS_CONSOLE_PACKET_BUFFER, c, 1u);
        if (nb_char_read == 0u)
        {
            /* Sleep until next try */
            NANO_OS_TASK_Sleep(NANO_OS_MS_TO_TICKS(NANO_OS_SEGGER_RTT_LINK_CONSOLE_POOLING_PERIOD));
        }
    }
    while (nb_char_read == 0u);

    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

    return ret;
}

/** \brief Write a string on the console link */
nano_os_error_t NANO_OS_USER_ConsoleWriteString(const char* const string)
{
    const nano_os_error_t ret = NOS_ERR_SUCCESS;

    #if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

    /* Write using the multiplexer */
    NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Send(NANO_OS_CONSOLE_PACKET_BUFFER, string, STRNLEN(string, 255u));

    #else

    /* Write directly on RTT link */
    SEGGER_RTT_WriteWithOverwriteNoLock(NANO_OS_CONSOLE_PACKET_BUFFER, string, STRNLEN(string, 255u));

    #endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

    return ret;
}



#endif /* (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 1u) */
