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
#include "stm32f767xx.h"


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 216 MHz */
    return 216000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Turn on power on GPIO */
    RCC->AHB1ENR |= (0x7FFu << 0u);

    /* leds 0,1,2 => PB0, PB7, PB14 */
    GPIOB->MODER &= ~((3u << 0u) | (3u << 14u) | (3u << 28u));
    GPIOB->MODER |= ((1u << 0u) | (1u << 14u) | (1u << 28u));

    /* Turn all leds off */
    for (i = 0; i < 3u; i++)
    {
        NANO_OS_BSP_LedOff(i);
    }

    return ret;
}

/** \brief Get the number of LEDs available */
uint8_t NANO_OS_BSP_GetLedCount(void)
{
    return 3u;
}


/** \brief Turn ON the specified LED */
void NANO_OS_BSP_LedOn(const uint8_t led)
{
    switch(led)
    {
        case 0:
            GPIOB->BSRR = (1u << 0u);
            break;
        case 1:
            GPIOB->BSRR = (1u << 7u);
            break;
        case 2:
            GPIOB->BSRR = (1u << 14u);
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
            GPIOB->BSRR = (1u << 16u);
            break;
        case 1:
            GPIOB->BSRR = (1u << 23u);
            break;
        case 2:
            GPIOB->BSRR = (1u << 30u);
            break;

        default:
            /* Do nothing */
            break;
    }
}

/** \brief Get the LED I/O registers memory area description */
void NANO_OS_BSP_GetLedIoRegistersMem(uint32_t* const start_address, uint32_t* const size)
{
    /* Check parameters */
    if ((start_address != NULL) && (size != NULL))
    {
        (*start_address) = GPIOA_BASE;
        (*size) = 0x3000u;
    }
}
