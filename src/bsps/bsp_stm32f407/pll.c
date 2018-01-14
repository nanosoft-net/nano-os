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

#include "stm32f407xx.h"

/** \brief Initialize CPU speed using PLL */
void pll_init()
{
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001u;

    /* Reset CFGR register */
    RCC->CFGR = 0x00000000u;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFFu;

    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010u;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFFu;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000u;

    /* Configure the Vector Table location add offset address ------------------*/
    SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH */

    /* Configure flash latency */
    FLASH->ACR = (3u << 8u) | (5u << 0u);

    /* Start external oscillator (HSE) */
    RCC->CR |= (1u << 16u);
    while ((RCC->CR & (1u << 17u)) == 0u)
    {}

    /* Configure PLL

        f(VCO clock) = f(PLL clock input) × (PLLN / PLLM)
        f(PLL general clock output) = f(VCO clock) / PLLP

        HSE = 25MHz, N = 300, M = 25, P = 2, Q = 2
        => fPLL = 150MHz
    */
    RCC->PLLCFGR = (2u << 24u) | (1u << 22u) | (0u << 16u) | (300u << 6u) | (25u << 0u);
    RCC->CR |= (1u << 24u);
    while ((RCC->CR & (1u << 25u)) == 0u)
    {}

    /* Configure peripheral clocks

         AHPPresc = 1, APB2Presc = 2, APB1Presc = 4
         => CPU clock = 150MHz, APB2 periph = 75MHz, APB1 periph = 37.5MHz
     */
    RCC->CFGR = (4u << 13u) | (5u << 10u) | (0u << 4u) | (2u << 0u);
}
