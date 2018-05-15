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
#include "LPC11xx.h"

#define BSP_LED_COUNT   4u


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

    /* Enable AHB clock to the GPIO domain. */
    LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);

    /* LED D21 IO as output */
    LPC_GPIO0->DIR |= 1<<7;

    /* RGB LED IO as output */
    LPC_GPIO0->DIR |= 1<<8;
    LPC_GPIO0->DIR |= 1<<9;
    LPC_GPIO0->DIR |= 1<<10;



    /* Turn all leds off */
    for (i = 0; i < BSP_LED_COUNT; i++)
    {
        NANO_OS_BSP_LedOff(i);
    }

    return ret;
}

/** \brief Get the number of LEDs available */
uint8_t NANO_OS_BSP_GetLedCount(void)
{
    return BSP_LED_COUNT;
}


/** \brief Turn ON the specified LED */
void NANO_OS_BSP_LedOn(const uint8_t led)
{
    switch(led)
    {
        case 0:
            LPC_GPIO0->MASKED_ACCESS[(1<<7)] = (1<<7);
            break;
        case 1:
            LPC_GPIO0->MASKED_ACCESS[(1<<8)] = (0<<8);
            break;
        case 2:
            LPC_GPIO0->MASKED_ACCESS[(1<<9)] = (0<<9);
            break;
        case 3:
            LPC_GPIO0->MASKED_ACCESS[(1<<10)] = (0<<10);
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
            LPC_GPIO0->MASKED_ACCESS[(1<<7)] = (0<<7);
            break;
        case 1:
            LPC_GPIO0->MASKED_ACCESS[(1<<8)] = (1<<8);
            break;
        case 2:
            LPC_GPIO0->MASKED_ACCESS[(1<<9)] = (1<<9);
            break;
        case 3:
            LPC_GPIO0->MASKED_ACCESS[(1<<10)] = (1<<10);
            break;


        default:
            /* Do nothing */
            break;
    }
}
