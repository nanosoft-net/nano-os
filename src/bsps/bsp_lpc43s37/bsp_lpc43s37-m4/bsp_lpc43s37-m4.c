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


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 120 MHz */
    return 120000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

//    /* Turn on power on GPIO */
//    LPC_SC->PCONP |= (1u << 15u);
//
//    /* Leds 0,1 => P2-26,27 */
//    LPC_IOCON->P2_26 = 0u;
//    LPC_IOCON->P2_27 = 0u;
//    LPC_GPIO2->DIR = (1u << 26u) | (1u << 27u);

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
//        case 0:
//            LPC_GPIO2->SET = NANO_OS_CAST(uint32_t, (1<<26));
//            break;
//        case 1:
//            LPC_GPIO2->SET = NANO_OS_CAST(uint32_t, (1<<27));
//            break;
//
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
//        case 0:
//            LPC_GPIO2->CLR = NANO_OS_CAST(uint32_t, (1<<26));
//            break;
//        case 1:
//            LPC_GPIO2->CLR = NANO_OS_CAST(uint32_t, (1<<27));
//            break;
//
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
//        (*start_address) = LPC_GPIO0_BASE;
        (*size) = 0x100u;
    }
}

/** \brief Get the UART registers memory area description */
void NANO_OS_BSP_GetUartIoRegistersMem(uint32_t* const start_address, uint32_t* const size)
{
    /* Check parameters */
    if ((start_address != NULL) && (size != NULL))
    {
//        (*start_address) = LPC_UART0_BASE;
        (*size) = 0x4000u;
    }
}
