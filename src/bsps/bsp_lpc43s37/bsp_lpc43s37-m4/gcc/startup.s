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

    .syntax unified
    .cpu cortex-m4
    .fpu vfpv4
    .thumb
    
    .section .startup

    .global _start

    .extern pll_init
    .extern c_init
	.extern cpp_init
    .extern main

    .extern NANO_OS_PORT_SvcHandler
	.extern NANO_OS_PORT_PendSvHandler
	.extern NANO_OS_PORT_SystickHandler

    .extern _TOP_OF_STACKS_
    
    
    .weak NMI_Handler
    .weak HardFault_Handler
    .weak MPUFault_Handler
    .weak BusFault_Handler
    .weak UsageFault_Handler
    

	.weak   DAC_Handler
	.weak   M0APP_Handler
	.weak   DMA_Handler
	.weak   EEPROM_Handler
	.weak   ETH_Handler
	.weak   SDIO_MMC_Handler
	.weak   LCD_Handler
	.weak   USB0_OTG_Handler
	.weak	USB1_Handler
	.weak   SCTIMER_Handler
	.weak   RIT_Handler
	.weak   TIMER0_Handler
	.weak   TIMER1_Handler
	.weak   TIMER2_Handler
	.weak   TIMER3_Handler
	.weak   MCPWM_Handler
	.weak   ADC0_Handler
	.weak   I2C0_Handler
	.weak   I2C1_Handler
	.weak   SPI_Handler
	.weak   ADC1_Handler
	.weak   SSP0_Handler
	.weak   SSP1_Handler
	.weak   USART0_Handler
	.weak   USART1_Handler
	.weak   USART2_Handler
	.weak   USART3_Handler
	.weak   I2S0_Handler
	.weak   I2S1_Handler
	.weak   SPIFI_Handler
	.weak   SGPIO_Handler
	.weak   PIN_INT0_Handler
	.weak   PIN_INT1_Handler
	.weak   PIN_INT2_Handler
	.weak   PIN_INT3_Handler
	.weak   PIN_INT4_Handler
	.weak   PIN_INT5_Handler
	.weak   PIN_INT6_Handler
	.weak   PIN_INT7_Handler
	.weak   GINT0_Handler
	.weak   GINT1_Handler
	.weak   EVENT_ROUTER_Handler
	.weak   C_CAN1_Handler
	.weak   ADCHS_Handler
	.weak   ATIMER_Handler
	.weak   RTC_Handler
	.weak   WWDT_Handler
	.weak   M0SUB_Handler
	.weak   C_CAN0_Handler
	.weak   QEI_Handler


_start:
_vectors:
    /* Cortex M4 internal interrupts */
    .long   _TOP_OF_STACKS_           /* Main stack pointer */
    .long   Reset_Handler             /* Reset Handler */
    .long   NMI_Handler               /* NMI Handler */
    .long   HardFault_Handler         /* Hard Fault Handler */
    .long   MPUFault_Handler          /* MPU Fault Handler */
    .long   BusFault_Handler          /* Bus Fault Handler */
    .long   UsageFault_Handler        /* Usage Fault Handler */
    .long   0x00000000                /* Reserved */
    .long   0x00000000                /* Reserved */
    .long   0x00000000                /* Reserved */
    .long   0x00000000                /* Reserved */
    .long   NANO_OS_PORT_SvcHandler     /* SVCCall Handler */
    .long   0x00000000                /* Reserved */
    .long   0x00000000                /* Reserved */
    .long   NANO_OS_PORT_PendSvHandler  /* PendSV Handler */
    .long   NANO_OS_PORT_SystickHandler /*SysTick Handler */

    /* External Interrupts */
	.long   DAC_Handler
	.long   M0APP_Handler
	.long   DMA_Handler
	.long   0x00000000                /* Reserved */
	.long   EEPROM_Handler
	.long   ETH_Handler
	.long   SDIO_MMC_Handler
	.long   LCD_Handler
	.long   USB0_OTG_Handler
	.long	USB1_Handler
	.long	SCTIMER_Handler
	.long   RIT_Handler
	.long   TIMER0_Handler
	.long   TIMER1_Handler
	.long   TIMER2_Handler
	.long   TIMER3_Handler
	.long   MCPWM_Handler
	.long   ADC0_Handler
	.long   I2C0_Handler
	.long   I2C1_Handler
	.long   SPI_Handler
	.long   ADC1_Handler
	.long   SSP0_Handler
	.long   SSP1_Handler
	.long   USART0_Handler
	.long   USART1_Handler
	.long   USART2_Handler
	.long   USART3_Handler
	.long   I2S0_Handler
	.long   I2S1_Handler
	.long   SPIFI_Handler
	.long   SGPIO_Handler
	.long   PIN_INT0_Handler
	.long   PIN_INT1_Handler
	.long   PIN_INT2_Handler
	.long   PIN_INT3_Handler
	.long   PIN_INT4_Handler
	.long   PIN_INT5_Handler
	.long   PIN_INT6_Handler
	.long   PIN_INT7_Handler
	.long   GINT0_Handler
	.long   GINT1_Handler
	.long   EVENT_ROUTER_Handler
	.long   C_CAN1_Handler
	.long   0x00000000                /* Reserved */
	.long   ADCHS_Handler
	.long   ATIMER_Handler
	.long   RTC_Handler
	.long   0x00000000                /* Reserved */
	.long   WWDT_Handler
	.long   M0SUB_Handler
	.long   C_CAN0_Handler
	.long   QEI_Handler

.thumb_func
.type Reset_Handler, %function
Reset_Handler:

    /* PLL init */
    bl pll_init

    /* C vars init */
    bl c_init

    /* C++ init */
    bl cpp_init

    /* Branch to main */
    bl main

    /* To prevent exiting from main */
    b       .
  
.thumb_func
NMI_Handler:
    b       .
    
.thumb_func
HardFault_Handler:
    b       .

.thumb_func
MPUFault_Handler:
    b       .

.thumb_func
BusFault_Handler:
    b       .

.thumb_func
UsageFault_Handler:
    b       .

    

.thumb_func
DAC_Handler:
    b        .

.thumb_func
M0APP_Handler:
    b        .

.thumb_func
DMA_Handler:
    b        .

.thumb_func
EEPROM_Handler:
    b        .

.thumb_func
ETH_Handler:
    b        .

.thumb_func
SDIO_MMC_Handler:
    b        .

.thumb_func
LCD_Handler:
    b        .

.thumb_func
USB0_OTG_Handler:
    b        .

.thumb_func
USB1_Handler:
    b        .

.thumb_func
SCTIMER_Handler:
    b        .

.thumb_func
RIT_Handler:
    b        .

.thumb_func
TIMER0_Handler:
    b        .

.thumb_func
TIMER1_Handler:
    b        .

.thumb_func
TIMER2_Handler:
    b        .

.thumb_func
TIMER3_Handler:
    b        .

.thumb_func
MCPWM_Handler:
    b        .

.thumb_func
ADC0_Handler:
    b        .

.thumb_func
I2C0_Handler:
    b        .

.thumb_func
I2C1_Handler:
    b        .

.thumb_func
SPI_Handler:
    b        .

.thumb_func
ADC1_Handler:
    b        .

.thumb_func
SSP0_Handler:
    b        .

.thumb_func
SSP1_Handler:
    b        .

.thumb_func
USART0_Handler:
    b        .

.thumb_func
USART1_Handler:
    b        .

.thumb_func
USART2_Handler:
    b        .

.thumb_func
USART3_Handler:
    b        .

.thumb_func
I2S0_Handler:
    b        .

.thumb_func
I2S1_Handler:
    b        .

.thumb_func
SPIFI_Handler:
    b        .

.thumb_func
SGPIO_Handler:
    b        .

.thumb_func
PIN_INT0_Handler:
    b        .

.thumb_func
PIN_INT1_Handler:
    b        .

.thumb_func
PIN_INT2_Handler:
    b        .

.thumb_func
PIN_INT3_Handler:
    b        .

.thumb_func
PIN_INT4_Handler:
    b        .

.thumb_func
PIN_INT5_Handler:
    b        .

.thumb_func
PIN_INT6_Handler:
    b        .

.thumb_func
PIN_INT7_Handler:
    b        .

.thumb_func
GINT0_Handler:
    b        .

.thumb_func
GINT1_Handler:
    b        .

.thumb_func
EVENT_ROUTER_Handler:
    b        .

.thumb_func
C_CAN1_Handler:
    b        .

.thumb_func
ADCHS_Handler:
    b        .

.thumb_func
ATIMER_Handler:
    b        .

.thumb_func
RTC_Handler:
    b        .

.thumb_func
WWDT_Handler:
    b        .

.thumb_func
M0SUB_Handler:
    b        .

.thumb_func
C_CAN0_Handler:
    b        .

.thumb_func
QEI_Handler:
    b        .


    
    
    .end
