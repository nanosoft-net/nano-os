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
#include "sama5d3x.h"
#include "aic.h"
#include "pio.c"

/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Initialize interrupt controller */
    AIC_Init();

    /* Led 0 => PE23
     * Led 1 => PE24
     */
    PIO_Configure(PIO_E, 23, PIOM_OUTPUT, PIOPM_NONE);
    PIO_Configure(PIO_E, 24, PIOM_OUTPUT, PIOPM_NONE);

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
            PIO_SetLow(PIO_E, 23);
            break;
        case 1:
            PIO_SetHigh(PIO_E, 24);
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
            PIO_SetHigh(PIO_E, 23);
            break;
        case 1:
            PIO_SetLow(PIO_E, 24);
            break;
        case 2:

            break;

        default:
            /* Do nothing */
            break;
    }
}
