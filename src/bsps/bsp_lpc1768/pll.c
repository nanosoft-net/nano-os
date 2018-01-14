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

#include "LPC17xx.h"

/** \brief Initialize CPU speed using PLL */
void pll_init()
{
    /* Enable and disconnect PLL0 if needed */
    if( (LPC_SC->PLL0CON & (1<<1))!=0 )
    {
        /* Disconnect PLL0 */
        LPC_SC->PLL0CON = 0x01;
        LPC_SC->PLL0FEED = 0xAA;
        LPC_SC->PLL0FEED = 0x55;
    }
    if( (LPC_SC->PLL0CON & (1<<0))!=0 )
    {
        /* Disable PLL0 */
        LPC_SC->PLL0CON = 0x00;
        LPC_SC->PLL0FEED = 0xAA;
        LPC_SC->PLL0FEED = 0x55;
    }

    /* Turn on main oscillator (12Mhz) */
    LPC_SC->SCS = (1<<5);

    /* Wait for main oscillator start */
    while( !(LPC_SC->SCS & (1 << 6)) );

    /* PLL0 clock source = main oscillator */
    LPC_SC->CLKSRCSEL = 0x01;

    /* PLL0 configuration fCC0 = 2 * main_osc * M / N */
    LPC_SC->PLL0CFG = 0x00020031; /* M=50, N=3 => fCC0 = 400Mhz */
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;

    /* Enable PLL0 */
    LPC_SC->PLL0CON = 0x01;
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;

    /* CPU clock is PLL0/4 */
    LPC_SC->CCLKCFG = 0x03;

    /* Wait for PLL0 lock */
    while( !(LPC_SC->PLL0STAT & (1 << 26)) );

    /* Connect PLL0 */
    LPC_SC->PLL0CON = 0x03;
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;

    /* Peripheral clock = CPU clock */
    LPC_SC->PCLKSEL0 = 0x55555555;
    LPC_SC->PCLKSEL1 = 0x55555555;
}
