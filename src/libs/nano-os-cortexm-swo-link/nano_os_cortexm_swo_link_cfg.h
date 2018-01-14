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

#ifndef NANO_OS_CORTEXM_SWO_LINK_CFG_H
#define NANO_OS_CORTEXM_SWO_LINK_CFG_H

#include "nano_os_safe_cfg.h"

/*********************************************************/
/*   Configuration of Nano OS SEGGER RTT link library    */
/*********************************************************/



/*********************************************************/
/*                   User settings                       */
/*   This settings must be adapted to your project       */
/*********************************************************/

/** \brief ITM channel to use to send trace data on SWO link */
#define NANO_OS_CORTEXM_SWO_LINK_ITM_CHANNEL            0u

/** \brief Selected SWO output frequency in Hz */
#define NANO_OS_CORTEXM_SWO_LINK_FREQUENCY              4500000u

/** \brief CPU clock frequency in Hz */
#define NANO_OS_CORTEXM_SWO_LINK_CPU_FREQUENCY          216000000u



/*********************************************************/
/*                 Nano OS settings                      */
/*              Do not edit this settings                */
/*********************************************************/



#endif /* NANO_OS_CORTEXM_SWO_LINK_CFG_H */
