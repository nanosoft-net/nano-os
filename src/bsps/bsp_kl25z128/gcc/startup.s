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
    
    .weak   DMA0_Handler  
    .weak   DMA1_Handler  
    .weak   DMA2_Handler  
    .weak   DMA3_Handler  
    .weak   FTFA_Handler  
    .weak   PMC_Handler  
    .weak   LLWU_Handler  
    .weak   I2C0_Handler  
    .weak   I2C1_Handler  
    .weak   SPI0_Handler  
    .weak   SPI1_Handler 
    .weak   UART0_Handler 
    .weak   UART1_Handler 
    .weak   UART2_Handler         
    .weak   ADC_Handler         
    .weak   CMP_Handler         
    .weak   TPM0_Handler        
    .weak   TPM1_Handler        
    .weak   TPM2_Handler        
    .weak   RTC_Alarm_Handler   
    .weak   RTC_Seconds_Handler 
    .weak   PIT_Handler         
    .weak   USB_OTG_Handler     
    .weak   DAC_Handler         
    .weak   TSI_Handler         
    .weak   MCG_Handler         
    .weak   LPTMR_Handler
    .weak   Pin_Detect_A_Handler
    .weak   Pin_Detect_D_Handler

    .section .text.vector_table, "x"
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
    .long   DMA0_Handler              /* 16: DMA Channel 0 Interrupt */
    .long   DMA1_Handler              /* 17: DMA Channel 1 Interrupt */
    .long   DMA2_Handler              /* 18: DMA Channel 2 Interrupt */
    .long   DMA3_Handler              /* 19: DMA Channel 3 Interrupt */
    .long   0x00000000                /* 20: Reserved Interrupt */
    .long   FTFA_Handler              /* 21: FTFA Interrupt */
    .long   PMC_Handler               /* 22: PMC Interrupt */
    .long   LLWU_Handler              /* 23: LLWU Interrupt */
    .long   I2C0_Handler              /* 24: I2C0 Interrupt */
    .long   I2C1_Handler              /* 25: I2C1 Interrupt */
    .long   SPI0_Handler              /* 26: SPI0 Interrupt */
    .long   SPI1_Handler              /* 27: SPI1 Interrupt */
    .long   UART0_Handler             /* 28: UART0 Interrupt */
    .long   UART1_Handler             /* 29: UART1 Interrupt */
    .long   UART2_Handler             /* 30: UART2 Interrupt */
    .long   ADC_Handler               /* 31: ADC Interrupt */
    .long   CMP_Handler               /* 32: CMP Interrupt */
    .long   TPM0_Handler              /* 33: TPM0 Interrupt */
    .long   TPM1_Handler              /* 34: TPM1 Interrupt */
    .long   TPM2_Handler              /* 35: TPM2 Interrupt */
    .long   RTC_Alarm_Handler         /* 36: RTC Alarm Interrupt */
    .long   RTC_Seconds_Handler       /* 37: RTC Seconds Interrupt */
    .long   PIT_Handler               /* 38: PIT Interrupt */
    .long   0x00000000                /* 39: Reserved Interrupt */
    .long   USB_OTG_Handler           /* 40: USB OTG Interrupt */
    .long   DAC_Handler               /* 41: DAC Interrupt */
    .long   TSI_Handler               /* 42: TSI Interrupt */
    .long   MCG_Handler               /* 43: MCG Interrupt */
    .long   LPTMR_Handler             /* 44: LPTMR Interrupt */
    .long   0x00000000                /* 45: Reserved Interrupt */
    .long   Pin_Detect_A_Handler      /* 46: Pin detect (PORTA) Interrupt */
    .long   Pin_Detect_D_Handler      /* 47: Pin detect (PORTD) Interrupt */

    .section .text.cfm_protect, "x"


    /* Flash Configuration */

    .long   0xFFFFFFFF
    .long   0xFFFFFFFF
    .long   0xFFFFFFFF
    .long   0xFFFFFFFE


    .section .text.startup

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
DMA0_Handler:
    b       .
    
.thumb_func
DMA1_Handler:
    b       .

.thumb_func
DMA2_Handler:
    b       .
    
.thumb_func
DMA3_Handler:
    b       .
    
.thumb_func
FTFA_Handler:
    b       .
    
.thumb_func
PMC_Handler:
    b       .

.thumb_func
LLWU_Handler:
    b       .
    
.thumb_func
I2C0_Handler:
    b       .

.thumb_func
I2C1_Handler:
    b       .
    
.thumb_func
SPI0_Handler:
    b       .
    
.thumb_func
SPI1_Handler:
    b       .
    
.thumb_func
UART0_Handler:
    b       .
    
.thumb_func
UART1_Handler:
    b       .
    
.thumb_func
UART2_Handler:
    b       .

.thumb_func
ADC_Handler:
    b       .
    
.thumb_func
CMP_Handler:
    b       .
    
.thumb_func
TPM0_Handler:
    b       .
    
.thumb_func
TPM1_Handler:
    b       .

.thumb_func
TPM2_Handler:
    b       .
    
.thumb_func
RTC_Alarm_Handler:
    b       .

.thumb_func
RTC_Seconds_Handler:
    b       .

.thumb_func
PIT_Handler:
    b       .
    
.thumb_func
USB_OTG_Handler:
    b       .
    
.thumb_func
DAC_Handler:
    b       .
    
.thumb_func
TSI_Handler:
    b       .
    
.thumb_func
MCG_Handler:
    b       .

.thumb_func
LPTMR_Handler:
    b       .
    
.thumb_func
Pin_Detect_A_Handler:
    b       .
    
.thumb_func
Pin_Detect_D_Handler:
    b       .
    
    
    
    .end
