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

#include "bsp.h"
#include "stm32f407xx.h"


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 150 MHz */
    return 150000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Turn on power on GPIO */
    RCC->AHB1ENR |= (0x1FFu << 0u);

    /* leds 0,1,2,3 => PG6, PG8, PI9, PC7 */
    GPIOG->MODER &= ~((3u << 12u) | (3u << 16u));
    GPIOG->MODER |= ((1u << 12u) | (1u << 16u));
    GPIOI->MODER &= ~((3u << 18u));
    GPIOI->MODER |= ((1u << 18u));
    GPIOC->MODER &= ~((3u << 14u));
    GPIOC->MODER |= ((1u << 14u));

    /* Turn all leds off */
    for (i = 0; i < 4u; i++)
    {
        NANO_OS_BSP_LedOff(i);
    }

    return ret;
}

/** \brief Get the number of LEDs available */
uint8_t NANO_OS_BSP_GetLedCount(void)
{
    return 4u;
}


/** \brief Turn ON the specified LED */
void NANO_OS_BSP_LedOn(const uint8_t led)
{
    switch(led)
    {
        case 0:
            GPIOG->BSRRL = (1u << 6u);
            break;
        case 1:
            GPIOG->BSRRL = (1u << 8u);
            break;
        case 2:
            GPIOI->BSRRL = (1u << 9u);
            break;
        case 3:
            GPIOC->BSRRL = (1u << 7u);
            break;

        default:
            /* Do nothing */
            break;
    }
}

/** \brief Turn OFF the specified LED */
void NANO_OS_BSP_LedOff(const uint8_t led)
{
    switch(led)
    {
        case 0:
            GPIOG->BSRRH = (1u << 6u);
            break;
        case 1:
            GPIOG->BSRRH = (1u << 8u);
            break;
        case 2:
            GPIOI->BSRRH = (1u << 9u);
            break;
        case 3:
            GPIOC->BSRRH = (1u << 7u);
            break;

        default:
            /* Do nothing */
            break;
    }
}
