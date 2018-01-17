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
#include "efm32zg222f32.h"


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 24 MHz */
    return 24000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Enable GPIO clock */
    CMU->HFPERCLKEN0 |= (1u << 7u);

    /* Led 0 => PC10
     * Led 1 => PC11
     */
    GPIO->P[2u].MODEH &= ~((0x0Fu << 8) | (0x0Fu << 12));
    GPIO->P[2u].MODEH |= (0x04u << 8) | (0x04u << 12);

    /* Turn all leds off */
    for (i = 0; i < 2u; i++)
    {
        NANO_OS_BSP_LedOff(i);
    }

    return ret;
}

/** \brief Get the number of LEDs available */
uint8_t NANO_OS_BSP_GetLedCount(void)
{
    return 2u;
}


/** \brief Turn ON the specified LED */
void NANO_OS_BSP_LedOn(const uint8_t led)
{
    switch(led)
    {
        case 0:
            GPIO->P[2u].DOUTSET = (1u << 10u);
            break;
        case 1:
            GPIO->P[2u].DOUTSET = (1u << 11u);
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
            GPIO->P[2u].DOUTCLR = (1u << 10u);
            break;
        case 1:
            GPIO->P[2u].DOUTCLR = (1u << 11u);
            break;
            
        default:
            /* Do nothing */
            break;
    }
}
