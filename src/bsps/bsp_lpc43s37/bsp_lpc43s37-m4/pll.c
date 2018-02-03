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

#include "chip_lpc43xx.h"

/** \brief Initialize CPU speed using PLL */
void pll_init()
{
//
//    /* Disable PLL0 */
//    LPC_SC->PLL0CON = 0x00u;
//    LPC_SC->PLL0FEED = 0xAAu;
//    LPC_SC->PLL0FEED = 0x55u;
//
//    /* Wait for PLL shutdown */
//    while ((LPC_SC->PLL0STAT & (1u << 8u)) != 0u);
//
//    /* Turn on main oscillator (12Mhz) */
//    LPC_SC->SCS = (1u << 5u);
//
//    /* Wait for main oscillator start */
//    while( (LPC_SC->SCS & (1u << 6u)) == 0u);
//
//    /* PLL0 clock source = main oscillator */
//    LPC_SC->CLKSRCSEL = 0x01u;
//
//    /* PLL0 configuration fPLL = main_osc * M, fCC0 = fPLL * 2 * P */
//    LPC_SC->PLL0CFG = 0x00000009u; /* M=10, P=1 => fCC0 = 240Mhz */
//    LPC_SC->PLL0FEED = 0xAAu;
//    LPC_SC->PLL0FEED = 0x55u;
//
//    /* Enable PLL0 */
//    LPC_SC->PLL0CON = 0x01u;
//    LPC_SC->PLL0FEED = 0xAAu;
//    LPC_SC->PLL0FEED = 0x55u;
//
//    /* Wait for PLL0 lock */
//    while ((LPC_SC->PLL0STAT & (1u << 10u)) == 0u);
//
//    /* CPU clock is PLL0/2 => 120MHz */
//    LPC_SC->CCLKSEL = (2u << 0u) | (1u << 8u);
//
//    /* Peripheral clock = CPU clock / 2 = 60MHz*/
//    LPC_SC->PCLKSEL = 0x02;
}
