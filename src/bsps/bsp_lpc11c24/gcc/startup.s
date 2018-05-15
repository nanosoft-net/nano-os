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
    .cpu cortex-m0
    .thumb
    
    .section .startup

    .global _start

    .extern SystemInit
    .extern c_init
    .extern cpp_init
    .extern main

    .extern NANO_OS_PORT_SvcHandler
    .extern NANO_OS_PORT_PendSvHandler
    .extern NANO_OS_PORT_SystickHandler

    .extern _TOP_OF_STACKS_
    
    
    .weak NMI_Handler
    .weak HardFault_Handler
    
    .weak   WAKEUP_Handler
    .weak	CAN_Handler
    .weak   SSP1_Handler
    .weak   I2C_Handler
    .weak   TIMER16_0_Handler
    .weak   TIMER16_1_Handler
    .weak   TIMER32_0_Handler
    .weak   TIMER32_1_Handler
    .weak   SSP0_Handler
    .weak   UART_Handler
    .weak   ADC_Handler
    .weak   WDT_Handler
    .weak   BOD_Handler
    .weak   PIOINT3_Handler
    .weak   PIOINT2_Handler
    .weak   PIOINT1_Handler
    .weak   PIOINT0_Handler

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
    .long   WAKEUP_Handler            /* 16: PIO0_0 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 17: PIO0_1 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 18: PIO0_2 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 19: PIO0_3 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 20: PIO0_4 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 21: PIO0_5 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 22: PIO0_6 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 23: PIO0_7 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 24: PIO0_8 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 25: PIO0_9 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 26: PIO0_10 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 27: PIO0_11 Wakeup Interrupt */
    .long   WAKEUP_Handler            /* 28: PIO1_0 Wakeup Interrupt */

    .long   CAN_Handler         /* 29: CAN Interrupt */
    .long   SSP1_Handler            /* 30: SSP1 Interrupt */
    .long   I2C_Handler             /* 31: I2C Interrupt */
    .long   TIMER16_0_Handler              /* 32: TIMER16_0 Interrupt */
    .long   TIMER16_1_Handler              /* 33: TIMER16_1 Interrupt */
    .long   TIMER32_0_Handler          /* 34: TIMER32_0 ODD Interrupt */
    .long   TIMER32_1_Handler            /* 35: TIMER32_1 Interrupt */
    .long   SSP0_Handler         /* 36: SSP0 RX Interrupt */
    .long   UART_Handler         /* 37: UART Interrupt */
    .long   0x00000000                /* Reserved */
    .long   0x00000000                /* Reserved */
    .long   ADC_Handler               /* 40: ADC Interrupt */
    .long   WDT_Handler               /* 41: Watchgdog Interrupt */
    .long   BOD_Handler              /* 42:  BrownOutDetect Interrupt */
    .long   0x00000000                /* Reserved */
    .long   PIOINT3_Handler               /* 43: PIO INT3 Interrupt */
    .long   PIOINT2_Handler               /* 44: PIO INT2 Interrupt */
    .long   PIOINT1_Handler               /* 45: PIO INT1 Interrupt */
    .long   PIOINT0_Handler               /* 46: PIO INT0 Interrupt */


.thumb_func
.type Reset_Handler, %function
Reset_Handler:

    /* PLL init */
    bl SystemInit

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
WAKEUP_Handler:
    b        .

.thumb_func
CAN_Handler:
    b        .

.thumb_func
SSP1_Handler:
    b        .

.thumb_func
I2C_Handler:
    b        .

.thumb_func
TIMER16_0_Handler:
    b        .


.thumb_func
TIMER16_1_Handler:
    b        .

.thumb_func
TIMER32_0_Handler:
    b        .

.thumb_func
TIMER32_1_Handler:
    b        .

.thumb_func
SSP0_Handler:
    b        .

.thumb_func
UART_Handler:
    b        .

.thumb_func
ADC_Handler:
    b        .

.thumb_func
WDT_Handler:
    b        .

.thumb_func
BOD_Handler:
    b        .

.thumb_func
PIOINT3_Handler:
    b        .

.thumb_func
PIOINT2_Handler:
    b        .

.thumb_func
PIOINT1_Handler:
    b        .

.thumb_func
PIOINT0_Handler:
    b        .


    .end
