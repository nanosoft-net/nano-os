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

    .extern _TOP_OF_STACKS_
    
    
    .weak NMI_Handler
    .weak HardFault_Handler
    .weak MPUFault_Handler
    .weak BusFault_Handler
    .weak UsageFault_Handler
    

	.long   RTC_Handler
	.long   M4CORE_Handler
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
	.long   GINT1_Handler
	.long   PIN_INT4_Handler
	.long   TIMER3_Handler
	.long   MCPWM_Handler
	.long   ADC0_Handler
	.long   I2C0_I2C1_Handler
	.long   SGPIO_Handler
	.long   SPI_DAC_Handler
	.long   ADC1_Handler
	.long   SSP0_SSP1_Handler
	.long   EVENT_ROUTER_Handler
	.long   USART0_Handler
	.long   USART1_Handler
	.long   USART2_CANC1_Handler
	.long   USART3_Handler
	.long   I2S0_I2S1_QEI_Handler
	.long   C_CAN0_Handler
	.long   SPIFI_ADCHS_Handler
	.long   M0SUB_Handler


_start:
_vectors:
    /* Cortex M0 internal interrupts */
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
    .long   0x00000000                /*SysTick Handler */

    /* External Interrupts */
	.long   RTC_Handler
	.long   M4CORE_Handler
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
	.long   GINT1_Handler
	.long   PIN_INT4_Handler
	.long   TIMER3_Handler
	.long   MCPWM_Handler
	.long   ADC0_Handler
	.long   I2C0_I2C1_Handler
	.long   SGPIO_Handler
	.long   SPI_DAC_Handler
	.long   ADC1_Handler
	.long   SSP0_SSP1_Handler
	.long   EVENT_ROUTER_Handler
	.long   USART0_Handler
	.long   USART1_Handler
	.long   USART2_CANC1_Handler
	.long   USART3_Handler
	.long   I2S0_I2S1_QEI_Handler
	.long   C_CAN0_Handler
	.long   SPIFI_ADCHS_Handler
	.long   M0SUB_Handler


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
RTC_Handler:
    b        .

.thumb_func
M4CORE_Handler:
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
GINT1_Handler:
    b        .

.thumb_func
PIN_INT4_Handler:
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
I2C0_I2C1_Handler:
    b        .

.thumb_func
SGPIO_Handler:
    b        .

.thumb_func
SPI_DAC_Handler:
    b        .

.thumb_func
ADC1_Handler:
    b        .

.thumb_func
SSP0_SSP1_Handler:
    b        .

.thumb_func
EVENT_ROUTER_Handler:
    b        .

.thumb_func
USART0_Handler:
    b        .

.thumb_func
USART1_Handler:
    b        .

.thumb_func
USART2_CANC1_Handler:
    b        .

.thumb_func
USART3_Handler:
    b        .

.thumb_func
I2S0_I2S1_QEI_Handler:
    b        .

.thumb_func
C_CAN0_Handler:
    b        .

.thumb_func
SPIFI_ADCHS_Handler:
    b        .

.thumb_func
M0SUB_Handler:
    b        .

    
    .end
