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
    .cpu cortex-m0plus
    .fpu softvfp
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
    
    .weak   DMA_Handler
    .weak   GPIO_EVEN_Handler
    .weak   TIMER0_Handler
    .weak   ACMP0_Handler
    .weak   ADC0_Handler
    .weak   I2C0_Handler
    .weak   GPIO_ODD_Handler
    .weak   TIMER1_Handler
    .weak   USART1_RX_Handler
    .weak   USART1_TX_Handler
    .weak   LEUART0_Handler
    .weak   PCNT0_Handler
    .weak   RTC_Handler
    .weak   CMU_Handler
    .weak   VCMP_Handler
    .weak   MSC_Handler
    .weak   AES_Handler

_start:
_vectors:
    /* Cortex M0+ internal interrupts */
    .long   _TOP_OF_STACKS_           /* Main stack pointer */
    .long   Reset_Handler             /* Reset Handler */
    .long   NMI_Handler               /* NMI Handler */
    .long   HardFault_Handler         /* Hard Fault Handler */
    .long   0x00000000                /* Reserved */
    .long   0x00000000                /* Reserved */
    .long   0x00000000                /* Reserved */
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
    .long   DMA_Handler               /* 16: DMA Interrupt */
    .long   GPIO_EVEN_Handler         /* 17: GPIO EVEN Interrupt */
    .long   TIMER0_Handler            /* 18: TIMER0 Interrupt */
    .long   ACMP0_Handler             /* 19: ACMP0 Interrupt */
    .long   ADC0_Handler              /* 20: ADC0 Interrupt */
    .long   I2C0_Handler              /* 21: I2C0 Interrupt */
    .long   GPIO_ODD_Handler          /* 22: GPIO ODD Interrupt */
    .long   TIMER1_Handler            /* 23: TIMER1 Interrupt */
    .long   USART1_RX_Handler         /* 24: USART1 RX Interrupt */
    .long   USART1_TX_Handler         /* 25: USART1 TX Interrupt */
    .long   LEUART0_Handler           /* 26: LEUART0 Interrupt */
    .long   PCNT0_Handler             /* 27: PCNT0 Interrupt */
    .long   RTC_Handler               /* 28: RTC Interrupt */
    .long   CMU_Handler               /* 29: CMU Interrupt */
    .long   VCMP_Handler              /* 30:  VCMP Interrupt */
    .long   MSC_Handler               /* 31: MSC Interrupt */
    .long   AES_Handler               /* 32: AES Interrupt */


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
DMA_Handler:
    b       .
    
.thumb_func
GPIO_EVEN_Handler:
    b       .

.thumb_func
TIMER0_Handler:
    b       .
    
.thumb_func
ACMP0_Handler:
    b       .
    
.thumb_func
ADC0_Handler:
    b       .
    
.thumb_func
I2C0_Handler:
    b       .

.thumb_func
GPIO_ODD_Handler:
    b       .
    
.thumb_func
TIMER1_Handler:
    b       .

.thumb_func
USART1_RX_Handler:
    b       .
    
.thumb_func
USART1_TX_Handler:
    b       .
    
.thumb_func
LEUART0_Handler:
    b       .
    
.thumb_func
PCNT0_Handler:
    b       .
    
.thumb_func
RTC_Handler:
    b       .
    
.thumb_func
CMU_Handler:
    b       .

.thumb_func
VCMP_Handler:
    b       .
    
.thumb_func
MSC_Handler:
    b       .
    
.thumb_func
AES_Handler:
    b       .

    .end
