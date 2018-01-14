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

#include "sama5d3x.h"

/** \brief Initialize CPU speed using PLL */
void pll_init()
{
    /* Start 12MHz external oscillator */
    PMC->CKGR_MOR = (1 << 0) | (1 << 3) | (0x37 << 16);
    while( (PMC->CKGR_MCFR & (1 << 16)) == 0 )
    {}

    /* Wait external oscillator ready */
    while( (PMC->PMC_SR & (1 << 0)) == 0 )
    {}

    /* Switch on external oscillator */
    PMC->CKGR_MOR = (PMC->CKGR_MOR & ~(1 << 1)) | (0x37 << 16);
    PMC->CKGR_MOR |= (1 << 24) | (0x37 << 16);

    /* Wait end of switch */
    while( (PMC->PMC_SR & (1 << 16)) == 0 )
    {}
    while( (PMC->PMC_SR & (1 << 3)) == 0 )
    {}

    /* Disable internal RC oscillator */
    PMC->CKGR_MOR = (PMC->CKGR_MOR & (~(1 << 3))) | (0x37 << 16);

    /* Select PLLA = 12 * (43 + 1) / 1 = 528MHz */
    PMC->CKGR_PLLAR = (1 << 0) | (43 << 18) | (1 << 29) | (0x3f << 8);
    PMC->PMC_PLLICPR = (0x3 << 8);
    while( (PMC->PMC_SR & (1 << 1)) == 0 )
    {}

    /* Select PLLA as master clock source
       Set MCK to 528 / 4 = 132MHz */
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(0x01 << 12));
    while( (PMC->PMC_SR & (1 << 3)) == 0 )
    {}
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(0x07 << 4));
    while( (PMC->PMC_SR & (1 << 3)) == 0 )
    {}
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(0x03 << 8)) | (2 << 8);
    while( (PMC->PMC_SR & (1 << 3)) == 0 )
    {}
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~(0x03 << 0)) | (2 << 0);
    while( (PMC->PMC_SR & (1 << 3)) == 0 )
    {}

    /* Disable watchdog */
    WDT->WDT_MR = (1 << 15);
}
