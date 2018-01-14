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
#include "MKL25Z4.h"


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 48 MHz */
    return 48000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Led 0 => PB18
     * Led 1 => PB19
     * Led 2 => PD1
     */
    PORTB->PCR[18u] = (1u << 8u);
    PORTB->PCR[19u] = (1u << 8u);
    PORTD->PCR[1u] = (1u << 8u);
    PTB->PDDR |= ((1u << 18u) | (1u << 19u));
    PTD->PDDR |= (1u << 1u);

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
            PTB->PCOR = (1u << 18u);
            break;
        case 1:
            PTB->PCOR = (1u << 19u);
            break;
        case 2:
            PTD->PCOR = (1u << 1u);
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
            PTB->PSOR = (1u << 18u);
            break;
        case 1:
            PTB->PSOR = (1u << 19u);
            break;
        case 2:
            PTD->PSOR = (1u << 1u);
            break;

        default:
            /* Do nothing */
            break;
    }
}
