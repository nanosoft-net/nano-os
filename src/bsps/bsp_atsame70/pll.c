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

#include "same70q21.h"


/** \brief Initialize CPU speed using PLL */
void pll_init()
{
    uint32_t reg;

    /* Clock configuration :

        XTAL = 12MHz
        MAINCK = 12MHz
        PLLA = 300MHz
        HCLK = 300MHz
        MCK = 150MHz
    */

    /* Configure FLASH wait states */
    EFC->EEFC_WPMR = EEFC_WPMR_WPKEY_PASSWD;
	EFC->EEFC_FMR = EEFC_FMR_FWS(5);
    EFC->EEFC_WPMR = (EEFC_WPMR_WPKEY_PASSWD | (1u << 0u));

    /* Disable PMC write protection */
    PMC->PMC_WPMR = PMC_WPMR_WPKEY_PASSWD;

    /* Enable external 12MHz oscillator */
    PMC->CKGR_MOR |= (CKGR_MOR_KEY_PASSWD | (8u << 8u) | (1u << 0u));

    /* Wait ready */
    while ((PMC->PMC_SR & (1u << 0u)) == 0u)
    {}

    /* Switch to external oscillator */
    PMC->CKGR_MOR |= (CKGR_MOR_KEY_PASSWD | (1u << 24u));

    /* Wait ready */
    while ((PMC->PMC_SR & (1u << 16u)) == 0u)
    {}

    /* Select MAINCK as MCK and HCLK input */
    reg = PMC->PMC_MCKR & ~(3u << 0u);
    PMC->PMC_MCKR = reg | (1u << 0u);

    /* Wait ready */
    while ((PMC->PMC_SR & (1u << 3u)) == 0u)
    {} 

    /* Disable internal oscillator */
    reg = PMC->CKGR_MOR & ~(1u << 3u); 
    PMC->CKGR_MOR = (CKGR_MOR_KEY_PASSWD | reg);

    /* Configure PLLA: DIV = 1, MUL = 24 => PLL out = MAINCK * (MUL + 1) / DIV = 300MHz */
    PMC->CKGR_PLLAR = (1u << 29u) | (24u << 16u) | (0x3Fu << 8u) | (1u << 0u);

    /* Wait ready */
    while ((PMC->PMC_SR & (1u << 1u)) == 0u)
    {} 

    /* Switch to MAINCK as MCK and HCLK input prescaler (HCLK = MAINCK) and divider = 2 (MCK = MAINCK / 2) */ 
    reg = PMC->PMC_MCKR & ~(3u << 8u);
    PMC->PMC_MCKR = reg | (1u << 8u);
    PMC->PMC_MCKR |= (1u << 0u);
    
    /* Wait ready */
    while ((PMC->PMC_SR & (1u << 3u)) == 0u)
    {} 

    /* Select PLLA as MCK and HCLK input prescaler (HCLK = PLLA) and divider = 2 (MCK = PLLA / 2) */ 
    reg = PMC->PMC_MCKR & ~(3u << 0u);
    PMC->PMC_MCKR = reg | (2u << 0u);

    /* Wait ready */
    while ((PMC->PMC_SR & (1u << 3u)) == 0u)
    {} 

    /* Enable PMC write protection */
    PMC->PMC_WPMR = (PMC_WPMR_WPKEY_PASSWD | (1u << 0u));
}
