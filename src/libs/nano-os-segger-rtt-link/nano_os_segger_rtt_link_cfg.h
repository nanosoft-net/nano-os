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

#ifndef NANO_OS_SEGGER_RTT_LINK_CFG_H
#define NANO_OS_SEGGER_RTT_LINK_CFG_H

#include "nano_os_safe_cfg.h"

/*********************************************************/
/*   Configuration of Nano OS SEGGER RTT link library    */
/*********************************************************/



/*********************************************************/
/*                   User settings                       */
/*   This settings must be adapted to your project       */
/*********************************************************/

/** \brief Enable trace output using SEGGER RTT link */
#define NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED               1u

/** \brief Enable console input/output using SEGGER RTT link */
#define NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED             1u

/** \brief Enable debug input/output using SEGGER RTT link */
#define NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED               1u

/** \brief Polling period in ms for checking for console input data */
#define NANO_OS_SEGGER_RTT_LINK_CONSOLE_POOLING_PERIOD      100u

/** \brief Polling period in ms for checking for debug input data */
#define NANO_OS_SEGGER_RTT_LINK_DEBUG_POOLING_PERIOD        100u

/** \brief Multiplex all data on one link (every data is sent/received on buffer 0
           this allow data exchange with a PC application using only telnet
           and avoid needed J-Link API SDK to retrieve data from multiple RTT buffers */
#define NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED           1u

/** \brief Polling period in ms for checking for multiplexer input data */
#define NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_POOLING_PERIOD    100u



/*********************************************************/
/*                 Nano OS settings                      */
/*              Do not edit this settings                */
/*********************************************************/

/* Adjust enabled functionalities depending on Nano OS configuration */
#if (NANO_OS_TRACE_ENABLED == 0u)
#undef NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED
#define NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED           0u
#endif /* (NANO_OS_TRACE_ENABLED == 0u) */

#if (NANO_OS_CONSOLE_ENABLED == 0u)
#undef NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED
#define NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED         0u
#endif /* (NANO_OS_CONSOLE_ENABLED == 0u) */

#if (NANO_OS_DEBUG_ENABLED == 0u)
#undef NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED
#define NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED           0u
#endif /* (NANO_OS_DEBUG_ENABLED == 0u) */

/* Compute RTT buffer needed */
#if ((NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 1u) && (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 1u) && (NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED == 1u))


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     3u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   2u

/** \brief Buffer to use for console packets */
#define NANO_OS_CONSOLE_PACKET_BUFFER               0u

/** \brief Buffer to use for debug packets */
#define NANO_OS_DEBUG_PACKET_BUFFER                 1u

/** \brief Buffer to use for real time trace packets */
#define NANO_OS_TRACE_RTT_PACKET_BUFFER             2u


#elif ((NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 1u) && (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 1u))


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     2u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   1u

/** \brief Buffer to use for console packets */
#define NANO_OS_CONSOLE_PACKET_BUFFER               0u

/** \brief Buffer to use for real time trace packets */
#define NANO_OS_TRACE_RTT_PACKET_BUFFER             1u


#elif ((NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 1u) && (NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED == 1u))


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     2u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   1u

/** \brief Buffer to use for debug packets */
#define NANO_OS_DEBUG_PACKET_BUFFER                 0u

/** \brief Buffer to use for real time trace packets */
#define NANO_OS_TRACE_RTT_PACKET_BUFFER             1u


#elif ((NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED == 1u) && (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 1u))


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     2u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   1u

/** \brief Buffer to use for console packets */
#define NANO_OS_CONSOLE_PACKET_BUFFER               0u

/** \brief Buffer to use for debug packets */
#define NANO_OS_DEBUG_PACKET_BUFFER                 1u


#elif (NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 1u)


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     1u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   0u

/** \brief Buffer to use for real time trace packets */
#define NANO_OS_TRACE_RTT_PACKET_BUFFER             0u


#elif (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 1u)


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     1u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   1u

/** \brief Buffer to use for console packets */
#define NANO_OS_CONSOLE_PACKET_BUFFER               0u


#elif (NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED == 1u)


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     1u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   1u

/** \brief Buffer to use for debug packets */
#define NANO_OS_DEBUG_PACKET_BUFFER                 0u


#else


/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     0u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   0u


#endif /* ((NANO_OS_SEGGER_RTT_LINK_TRACE_ENABLED == 1u) && (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED == 1u)) */


#if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

/* Undefined previously defined buffer counts as only 1 buffer is needed */
#undef NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT
#undef NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT

/** \brief Number of up buffers used */
#define NANO_OS_SEGGER_RTT_LINK_UP_BUFFER_COUNT     1u

/** \brief Number of down buffers used */
#define NANO_OS_SEGGER_RTT_LINK_DOWN_BUFFER_COUNT   1u

/** \brief Buffer to use to send/receive data */
#define NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_BUFFER    0u

#endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

#endif /* NANO_OS_SEGGER_RTT_LINK_CFG_H */
