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

#ifndef NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_H
#define NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_H

#include "nano_os_segger_rtt_link_cfg.h"

/* Check if module is enabled */
#if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

#include "nano_os_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Initialize the multiplexer for RTT packets */
nano_os_error_t NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Init(void);

/** \brief Send data using the multiplexer on the RTT link */
void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Send(const uint8_t channel, const void* const data, const size_t data_size);

/** \brief Wait forever for an incoming byte the RTT link */
nano_os_error_t NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Read(const uint8_t channel, uint8_t* const data);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */

#endif /* NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_H */
