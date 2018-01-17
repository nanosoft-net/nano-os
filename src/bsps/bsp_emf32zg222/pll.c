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

#include "efm32zg222f32.h"

/** \brief Initialize CPU speed using PLL */
void pll_init()
{
    /*
        Configuration with HFXO = 24MHz :

        HFCLK = HFXO = 24MHz
        HFCORECLK = 24MHz
        HFPERCLK = 24MHz
        LFACLK = 12MHz
        LFBCLK = 12MHz
    */
    
    /* Use 24MHz external oscillator (HFXO) */
    CMU->OSCENCMD = (1u << 2u);
    while ((CMU->STATUS & (1u << 3u)) == 0u)
    {}
    CMU->CMD = (2u << 0u);

    /* Set HFCORECLK = HFCLK */
    CMU->HFCORECLKDIV &= ~(0x0Fu);

    /* Set HFPERCLK = HFCLK */
    CMU->HFPERCLKDIV = (1u << 8u);

    /* Set LFACLK = HFCORECLK/2 */
    CMU->LFCLKSEL &= ~(1u << 16u);
    CMU->LFCLKSEL |= (3u << 0u);
    /* CMU_LFCLKSEL */

    /* Set LFBCLK = HFCORECLK/2 */
    CMU->LFCLKSEL &= ~(1u << 20u);
    CMU->LFCLKSEL |= (3u << 2u);
}
