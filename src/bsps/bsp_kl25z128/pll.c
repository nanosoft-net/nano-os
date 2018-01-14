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

#include "MKL25Z4.h"

/** \brief Initialize CPU speed using PLL */
void pll_init()
{
    /* Disable the WDOG module */
    /* SIM_COPC: COPT=0,COPCLKS=0,COPW=0 */
    SIM->COPC = (uint32_t)0x00u;
    /* SIM->SCGC5: PORTA=1 */
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK);     /* Enable clock gate for ports to enable pin routing */
    /* SIM->CLKDIV1: OUTDIV1=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,OUTDIV4=1,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0,??=0 */
    SIM->CLKDIV1 = (uint32_t)0x00010000UL; /* Update system prescalers */
    /* PORTA->PCR18: ISF=0,MUX=0 */
    PORTA->PCR[18] &= (uint32_t)~0x01000700UL;
    /* PORTA->PCR19: ISF=0,MUX=0 */
    PORTA->PCR[19] &= (uint32_t)~0x01000700UL;
    /* Switch to FBE Mode */
    /* OSC0->CR: ERCLKEN=1,??=0,EREFSTEN=0,??=0,SC2P=1,SC4P=0,SC8P=0,SC16P=1 */
    OSC0->CR = (uint8_t)0x89U;
    /* MCG->C2: LOCRE0=0,??=0,RANGE0=2,HGO0=0,EREFS0=1,LP=0,IRCS=0 */
    MCG->C2 = (uint8_t)0x24U;
    /* MCG->C1: CLKS=2,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
    MCG->C1 = (uint8_t)0x9AU;
    /* MCG->C4: DMX32=0,DRST_DRS=0 */
    MCG->C4 &= (uint8_t)~(uint8_t)0xE0U;
    /* MCG->C5: ??=0,PLLCLKEN0=0,PLLSTEN0=0,PRDIV0=1 */
    MCG->C5 = (uint8_t)0x03U;
    /* MCG->C6: LOLIE0=0,PLLS=0,CME0=0,VDIV0=0 */
    MCG->C6 = (uint8_t)0x00U;
    while((MCG->S & MCG_S_IREFST_MASK) != 0x00U) { /* Check that the source of the FLL reference clock is the external reference clock. */
    }
    while((MCG->S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
    }
    /* Switch to PBE Mode */
    /* MCG->C6: LOLIE0=0,PLLS=1,CME0=0,VDIV0=0 */
    MCG->C6 = (uint8_t)0x40U;
    while((MCG->S & 0x0CU) != 0x08U) {    /* Wait until external reference clock is selected as MCG output */
    }
    while((MCG->S & MCG_S_LOCK0_MASK) == 0x00U) { /* Wait until locked */
    }
    /* Switch to PEE Mode */
    /* MCG->C1: CLKS=0,FRDIV=3,IREFS=0,IRCLKEN=1,IREFSTEN=0 */
    MCG->C1 = (uint8_t)0x1AU;
    while((MCG->S & 0x0CU) != 0x0CU) {    /* Wait until output of the PLL is selected */
    }

}
