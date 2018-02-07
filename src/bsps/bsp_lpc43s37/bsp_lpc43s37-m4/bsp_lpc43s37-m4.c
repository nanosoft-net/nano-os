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
#include "chip_lpc43xx.h"


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 204 MHz */
    return 204000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Turn on power on GPIO */
    LPC_CCU1->CLKCCU[CLK_MX_SCU].CFG = (1u << 0u);
    LPC_CCU1->CLKCCU[CLK_MX_GPIO].CFG = (1u << 0u);

    /* Leds 0,1,2 =>
     * P6_9 : GPIO3[5]
     * P6_11 : GPIO3[7]
     * P2_7 : GPIO0[7] */
    LPC_SCU->SFSP[3u][5u] = (1u << 7u);
    LPC_SCU->SFSP[3u][7u] = (1u << 7u);
    LPC_SCU->SFSP[0u][7u] = (1u << 7u);
    LPC_GPIO_PORT->DIR[3u] |= (1u << 5u) | (1u << 7u);
    LPC_GPIO_PORT->DIR[0u] |= (1u << 7u);

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
            LPC_GPIO_PORT->CLR[3u] = NANO_OS_CAST(uint32_t, (1u << 5u));
            break;
        case 1:
            LPC_GPIO_PORT->CLR[3u] = NANO_OS_CAST(uint32_t, (1u << 7u));
            break;
        case 2:
            LPC_GPIO_PORT->CLR[0u] = NANO_OS_CAST(uint32_t, (1u << 7u));
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
            LPC_GPIO_PORT->SET[3u] = NANO_OS_CAST(uint32_t, (1u << 5u));
            break;
        case 1:
            LPC_GPIO_PORT->SET[3u] = NANO_OS_CAST(uint32_t, (1u << 7u));
            break;
        case 2:
            LPC_GPIO_PORT->SET[0u] = NANO_OS_CAST(uint32_t, (1u << 7u));
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
        (*start_address) = LPC_GPIO_PORT_BASE;
        (*size) = 0xC000u;
    }
}

/** \brief Get the UART registers memory area description */
void NANO_OS_BSP_GetUartIoRegistersMem(uint32_t* const start_address, uint32_t* const size)
{
    /* Check parameters */
    if ((start_address != NULL) && (size != NULL))
    {
        (*start_address) = LPC_USART0_BASE;
        (*size) = 0x40000u;
    }
}
