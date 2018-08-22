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
#include "same70q21.h"


/** \brief Get the SYSTICK input clock frequency in Hz */
uint32_t NANO_OS_PORT_USER_GetSystickInputClockFreq(void)
{
    /* 300 MHz */
    return 300000000u;
}


/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    uint8_t i;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Turn on power on all PIOs */
    PMC->PMC_WPMR = PMC_WPMR_WPKEY_PASSWD;
    PMC->PMC_PCR = (1u << 29u) | (1u << 20u) | (1u << 12u) | (4u << 8u) | (ID_PIOA << 0u);
    PMC->PMC_PCR = (1u << 29u) | (1u << 20u) | (1u << 12u) | (4u << 8u) | (ID_PIOB << 0u);
    PMC->PMC_PCR = (1u << 29u) | (1u << 20u) | (1u << 12u) | (4u << 8u) | (ID_PIOC << 0u);
    PMC->PMC_PCR = (1u << 29u) | (1u << 20u) | (1u << 12u) | (4u << 8u) | (ID_PIOD << 0u);
    PMC->PMC_PCR = (1u << 29u) | (1u << 20u) | (1u << 12u) | (4u << 8u) | (ID_PIOE << 0u);
    PMC->PMC_PCER0 |= (7u << 10u) |(3u << 16u);
    PMC->PMC_WPMR = (PMC_WPMR_WPKEY_PASSWD | (1u << 0u));

    /* leds 0 => PC8 */
    PIOC->PIO_WPMR = PIO_WPMR_WPKEY_PASSWD;
    PIOC->PIO_OER = (1u << 8u);
    PIOC->PIO_PER = (1u << 8u);
    PIOC->PIO_WPMR = (PIO_WPMR_WPKEY_PASSWD | (1u << 0u));

    /* Turn all leds off */
    for (i = 0; i < 1u; i++)
    {
        NANO_OS_BSP_LedOff(i);
    }

    return ret;
}

/** \brief Get the number of LEDs available */
uint8_t NANO_OS_BSP_GetLedCount(void)
{
    return 1u;
}


/** \brief Turn ON the specified LED */
void NANO_OS_BSP_LedOn(const uint8_t led)
{
    switch(led)
    {
        case 0:
            PIOC->PIO_CODR = (1u << 8u);
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
            PIOC->PIO_SODR = (1u << 8u);
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
        (*start_address) = NANO_OS_CAST(uint32_t, PIOC);
        (*size) = 0x0200u;
    }
}

/** \brief Get the UART registers memory area description */
void NANO_OS_BSP_GetUartIoRegistersMem(uint32_t* const start_address, uint32_t* const size)
{
    /* Check parameters */
    if ((start_address != NULL) && (size != NULL))
    {
        (*start_address) = NANO_OS_CAST(uint32_t, USART1);
        (*size) = 0x4000u;
    }
}


/**
* HardFaultHandler_C:
* This is called from the HardFault_HandlerAsm with a pointer the Fault stack
* as the parameter. We can then read the values from the stack and place them
* into local variables for ease of reading.
* We then read the various Fault Status and Address Registers to help decode
* cause of the fault.
* The function ends with a BKPT instruction to force control back into the debugger
*/
void HardFault_HandlerC(unsigned long *hardfault_args){
volatile unsigned long stacked_r0 ;
volatile unsigned long stacked_r1 ;
volatile unsigned long stacked_r2 ;
volatile unsigned long stacked_r3 ;
volatile unsigned long stacked_r12 ;
volatile unsigned long stacked_lr ;
volatile unsigned long stacked_pc ;
volatile unsigned long stacked_psr ;
volatile unsigned long _CFSR ;
volatile unsigned long _HFSR ;
volatile unsigned long _DFSR ;
volatile unsigned long _AFSR ;
volatile unsigned long _BFAR ;
volatile unsigned long _MMAR ;
stacked_r0 = ((unsigned long)hardfault_args[0]) ;
stacked_r1 = ((unsigned long)hardfault_args[1]) ;
stacked_r2 = ((unsigned long)hardfault_args[2]) ;
stacked_r3 = ((unsigned long)hardfault_args[3]) ;
stacked_r12 = ((unsigned long)hardfault_args[4]) ;
stacked_lr = ((unsigned long)hardfault_args[5]) ;
stacked_pc = ((unsigned long)hardfault_args[6]) ;
stacked_psr = ((unsigned long)hardfault_args[7]) ;
// Configurable Fault Status Register
// Consists of MMSR, BFSR and UFSR
_CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;
// Hard Fault Status Register
_HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;
// Debug Fault Status Register
_DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;
// Auxiliary Fault Status Register
_AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;
// Read the Fault Address Registers. These may not contain valid values.
// Check BFARVALID/MMARVALID to see if they are valid values
// MemManage Fault Address Register
_MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
// Bus Fault Address Register
_BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;
__asm("BKPT #0\n") ; // Break into the debugger
}
__attribute__((naked)) void HardFault_Handler(void)
{
__asm volatile (
" movs r0,#4 \n"
" movs r1, lr \n"
" tst r0, r1 \n"
" beq _MSP \n"
" mrs r0, psp \n"
" b _HALT \n"
"_MSP: \n"
" mrs r0, msp \n"
"_HALT: \n"
" ldr r1,[r0,#20] \n"
" b HardFault_HandlerC \n"
" bkpt #0 \n"
);
}