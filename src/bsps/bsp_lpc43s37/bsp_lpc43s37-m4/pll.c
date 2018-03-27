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
    volatile uint32_t delay;

    /* Configure flash latency to run up to 204MHz */
    LPC_CREG->FLASHCFGA = (9u << 12u) | (1u << 31u);
    LPC_CREG->FLASHCFGB = (9u << 12u) | (1u << 31u);

    /* Select the IRC as BASE_M4_CLK source */
    LPC_CGU->BASE_CLK[CLK_BASE_MX] = (1u << 24u);

    /* Wait 250µs */
    delay = 10000u;
    while(delay--) {}

    /* Set the AUTOBLOCK bit (bit 11). This bit re-synchronizes the clock output during
       frequency changes that prevents glitches when switching clock frequencies. */
    LPC_CGU->BASE_CLK[CLK_BASE_MX] |= (1u << 11u);

    /* Reconfigure PLL1 :
       – Select the M and N divider values to produce the final desired PLL1 output
         frequency foutPLL.
       – Select the IRC as clock source for PLL1.

       FCCO = M * FCLKIN / N
       FCLKOUT = FCCO / (2 * P) =  (M * FCLKIN) / (2 * P * N)

       M = 17, N = 1, P = 1:
       FCCO = 204MHz
       FCLKOUT = 102MHz
    */
    LPC_CGU->PLL1_CTRL = (0u << 8u) | (0u << 11u) | (0u << 12u) | (16u << 16u) | (1u << 24u);

    /* Wait for the PLL1 to lock */
    while ((LPC_CGU->PLL1_STAT & (1u << 0u)) == 0u)
    {}

    /* Select PLL1 as BASE_M4_CLK source. The BASE_M4_CLK now operates in the
       mid-frequency range */
    LPC_CGU->BASE_CLK[CLK_BASE_MX] = (1u << 11u) | (9u << 24u);

    /* Wait 50µs */
    delay = 250u;
    while(delay--) {}

    /* Set the PLL1 P-divider to direct output mode (DIRECT = 1) */
    LPC_CGU->PLL1_CTRL |= (1u << 7u);

    /* Configure Peripheral clocks = IDVA clock
       IDIVA clock = PLL1 / 2 = 102MHz */
    LPC_CGU->IDIV_CTRL[CLK_IDIV_A] = (1u << 2u) | (1u << 11u) | (9u << 24u);
    LPC_CGU->BASE_CLK[CLK_BASE_APB1] = (1u << 11u) | (12u << 24u);
    LPC_CGU->BASE_CLK[CLK_BASE_APB3] = (1u << 11u) | (12u << 24u);
    LPC_CGU->BASE_CLK[CLK_BASE_PERIPH] = (1u << 11u) | (12u << 24u);
}
