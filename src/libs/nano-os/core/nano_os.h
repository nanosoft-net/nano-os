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

#ifndef NANO_OS_H
#define NANO_OS_H

#include "nano_os_types.h"


/** \brief Macro to convert milliseconds into OS ticks */
#define NANO_OS_MS_TO_TICKS(ms) ((ms) * NANO_OS_TICK_RATE_HZ / 1000u)

/** \brief Macro to convert OS ticks into milliseconds */
#define NANO_OS_TICKS_TO_MS(ticks) ((ticks) * 1000u / NANO_OS_TICK_RATE_HZ)



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Initialize Nano OS */
nano_os_error_t NANO_OS_Init(void);

/** \brief Start Nano OS (this function returns only in case of error at during first scheduling) */
nano_os_error_t NANO_OS_Start(void);

/** \brief Get the current tick count since Nano OS is started */
nano_os_error_t NANO_OS_GetTickCount(uint32_t* tick_count);

/** \brief Get Nano OS version and the current used port version */
nano_os_error_t NANO_OS_GetVersion(nano_os_version_t* const version, nano_os_version_t* const port_version);


/** \brief Handle the tick interrupt */
void NANO_OS_TickInterrupt(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_H */
