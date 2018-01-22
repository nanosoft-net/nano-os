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
#include "LPC17xx.h"


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 100 MHz */
    return 100000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* leds 0,1,2 => P1-28,29,31
       leds 3,4,5,6,7 => P2-2,3,4,5,6 */
    const uint32_t p1_nmask = ~( (3<<24) | (3<<26) | (3<<30) );
    const uint32_t p2_nmask = ~( (3<<4) | (3<<6) | (3<<8) | (3<<10) | (3<<12) );
    const uint32_t p1_mask = ( (1<<28) | (1<<29) | (1<<31) );
    const uint32_t p2_mask = ( (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) );

    /* Turn on power on GPIO */
    LPC_SC->PCONP |= (1<<15);

    /* Pin selection as output with no open drain */
    LPC_PINCON->PINSEL3 &= p1_nmask;
    LPC_PINCON->PINMODE3 &= p1_nmask;
    LPC_PINCON->PINMODE_OD1 &= ~p1_mask;
    LPC_GPIO1->FIODIR |= p1_mask;
    LPC_PINCON->PINSEL4 &= p2_nmask;
    LPC_PINCON->PINMODE4 &= p2_nmask;
    LPC_PINCON->PINMODE_OD2 &= ~p2_mask;
    LPC_GPIO2->FIODIR |= p2_mask;

    /* Turn all leds off */
    for (i = 0; i < 8u; i++)
    {
        NANO_OS_BSP_LedOff(i);
    }

    return ret;
}

/** \brief Get the number of LEDs available */
uint8_t NANO_OS_BSP_GetLedCount(void)
{
    return 8u;
}


/** \brief Turn ON the specified LED */
void NANO_OS_BSP_LedOn(const uint8_t led)
{
    switch(led)
    {
        case 0:
            LPC_GPIO1->FIOSET = NANO_OS_CAST(uint32_t, (1<<28));
            break;
        case 1:
            LPC_GPIO1->FIOSET = NANO_OS_CAST(uint32_t, (1<<29));
            break;
        case 2:
            LPC_GPIO1->FIOSET = NANO_OS_CAST(uint32_t, (1<<31));
            break;

        case 3:
            LPC_GPIO2->FIOSET = NANO_OS_CAST(uint32_t, (1<<2));
            break;
        case 4:
            LPC_GPIO2->FIOSET = NANO_OS_CAST(uint32_t, (1<<3));
            break;
        case 5:
            LPC_GPIO2->FIOSET = NANO_OS_CAST(uint32_t, (1<<4));
            break;
        case 6:
            LPC_GPIO2->FIOSET = NANO_OS_CAST(uint32_t, (1<<5));
            break;
        case 7:
            LPC_GPIO2->FIOSET = NANO_OS_CAST(uint32_t, (1<<6));
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
            LPC_GPIO1->FIOCLR = NANO_OS_CAST(uint32_t, (1<<28));
            break;
        case 1:
            LPC_GPIO1->FIOCLR = NANO_OS_CAST(uint32_t, (1<<29));
            break;
        case 2:
            LPC_GPIO1->FIOCLR = NANO_OS_CAST(uint32_t, (1<<31));
            break;

        case 3:
            LPC_GPIO2->FIOCLR = NANO_OS_CAST(uint32_t, (1<<2));
            break;
        case 4:
            LPC_GPIO2->FIOCLR = NANO_OS_CAST(uint32_t, (1<<3));
            break;
        case 5:
            LPC_GPIO2->FIOCLR = NANO_OS_CAST(uint32_t, (1<<4));
            break;
        case 6:
            LPC_GPIO2->FIOCLR = NANO_OS_CAST(uint32_t, (1<<5));
            break;
        case 7:
            LPC_GPIO2->FIOCLR = NANO_OS_CAST(uint32_t, (1<<6));
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
        (*start_address) = LPC_GPIO_BASE;
        (*size) = 0x100u;
    }
}

/** \brief Get the UART registers memory area description */
void NANO_OS_BSP_GetUartIoRegistersMem(uint32_t* const start_address, uint32_t* const size)
{
    /* Check parameters */
    if ((start_address != NULL) && (size != NULL))
    {
        (*start_address) = LPC_UART0_BASE;
        (*size) = 0x4000u;
    }
}
