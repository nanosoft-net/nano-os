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

#include "nano_os_trace.h"

/* Check if module is enabled */
#if (NANO_OS_TRACE_ENABLED == 1u)

#include "nano_os_port.h"
#include "nano_os_user.h"


/** \brief Size of a real time trace packet in bytes */
#define NANO_OS_TRACE_RTT_PACKET_SIZE           8u

/** \brief Start of trace packet magic */
#define NANO_OS_TRACE_PACKET_START_MAGIC        0xA0u

/** \brief Real time trace packet type */
#define NANO_OS_TRACE_RTT_PACKET_TYPE           0x0Au

/** \brief Trace request packet type */
#define NANO_OS_TRACE_REQ_PACKET_TYPE           0x02u

/** \brief Trace response packet type */
#define NANO_OS_TRACE_RESP_PACKET_TYPE          0x08u


/** \brief Initialize the trace module */
nano_os_error_t NANO_OS_TRACE_Init(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Initialize the user module which will send/receive the trace packets */
    ret = NANO_OS_USER_TraceInit();
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Real time trace event */
        NANO_OS_TRACE_Add(NOS_TRACE_START, 0u);
    }

    return ret;
}

/** \brief Add a trace entry */
void NANO_OS_TRACE_Add(const uint8_t event_type, const uint16_t event_id)
{
    uint8_t rtt_packet[NANO_OS_TRACE_RTT_PACKET_SIZE];

    /* Get the timestamp */
    const uint32_t timestamp = NANO_OS_PORT_GetTimestampInUs();

    /* Fill the RTT packet */
    rtt_packet[0u] = NANO_OS_TRACE_PACKET_START_MAGIC | NANO_OS_TRACE_RTT_PACKET_TYPE;
    rtt_packet[1u] = event_type;
    rtt_packet[2u] = NANO_OS_CAST(uint8_t, (event_id & 0xFFu));
    rtt_packet[3u] = NANO_OS_CAST(uint8_t, (event_id >> 8u));
    rtt_packet[4u] = NANO_OS_CAST(uint8_t, (timestamp & 0xFFu));
    rtt_packet[5u] = NANO_OS_CAST(uint8_t, ((timestamp >> 8u) & 0xFFu));
    rtt_packet[6u] = NANO_OS_CAST(uint8_t, ((timestamp >> 16u) & 0xFFu));
    rtt_packet[7u] = NANO_OS_CAST(uint8_t, ((timestamp >> 24u) & 0xFFu));

    /* Send the RTT packet */
    NANO_OS_USER_TraceSendRttPacket(rtt_packet, sizeof(rtt_packet));

    return;
}

#endif /* (NANO_OS_TRACE_ENABLED == 1u) */
