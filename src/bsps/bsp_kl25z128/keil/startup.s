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


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

	EXPORT __initial_sp

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

	EXPORT __heap_base
	EXPORT __heap_limit

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


    AREA |.startup|, CODE, READONLY, ALIGN=2
    THUMB
    REQUIRE8
    PRESERVE8

    EXPORT _vectors
    EXPORT Reset_Handler

    EXTERN pll_init
    EXTERN __main

    EXTERN NANO_OS_PORT_SvcHandler
	EXTERN NANO_OS_PORT_PendSvHandler
	EXTERN NANO_OS_PORT_SystickHandler



	EXPORT  NMI_Handler			[WEAK]
    EXPORT  HardFault_Handler	[WEAK]
    EXPORT  MPUFault_Handler	[WEAK]
    EXPORT  BusFault_Handler	[WEAK]
    EXPORT  UsageFault_Handler	[WEAK]

    EXPORT    DMA0_Handler  	[WEAK]
    EXPORT    DMA1_Handler  	[WEAK]
    EXPORT    DMA2_Handler  	[WEAK]
    EXPORT    DMA3_Handler  	[WEAK]
    EXPORT    FTFA_Handler  	[WEAK]
    EXPORT    PMC_Handler  	[WEAK]
    EXPORT    LLWU_Handler  	[WEAK]
    EXPORT    I2C0_Handler  	[WEAK]
    EXPORT    I2C1_Handler  	[WEAK]
    EXPORT    SPI0_Handler  	[WEAK]
    EXPORT    SPI1_Handler 	[WEAK]
    EXPORT    UART0_Handler 	[WEAK]
    EXPORT    UART1_Handler 	[WEAK]
    EXPORT    UART2_Handler         	[WEAK]
    EXPORT    ADC_Handler         	[WEAK]
    EXPORT    CMP_Handler         	[WEAK]
    EXPORT    TPM0_Handler        	[WEAK]
    EXPORT    TPM1_Handler        	[WEAK]
    EXPORT    TPM2_Handler        	[WEAK]
    EXPORT    RTC_Alarm_Handler   	[WEAK]
    EXPORT    RTC_Seconds_Handler 	[WEAK]
    EXPORT    PIT_Handler         	[WEAK]
    EXPORT    USB_OTG_Handler     	[WEAK]
    EXPORT    DAC_Handler       	[WEAK]
    EXPORT    TSI_Handler	[WEAK]
    EXPORT    MCG_Handler 	[WEAK]
    EXPORT    LPTMR_Handler	[WEAK]
    EXPORT    Pin_Detect_A_Handler	[WEAK]
    EXPORT    Pin_Detect_D_Handler	[WEAK]


    
_vectors
    /* Cortex M0+ internal interrupts */
    DCD   __initial_sp              /* Main stack pointer */
    DCD   Reset_Handler             /* Reset Handler */
    DCD   NMI_Handler               /* NMI Handler */
    DCD   HardFault_Handler         /* Hard Fault Handler */
    DCD   0x00000000                /* Reserved */
    DCD   0x00000000                /* Reserved */
    DCD   0x00000000                /* Reserved */
    DCD   0x00000000                /* Reserved */
    DCD   0x00000000                /* Reserved */
    DCD   0x00000000                /* Reserved */
    DCD   0x00000000                /* Reserved */
    DCD   NANO_OS_PORT_SvcHandler     /* SVCCall Handler */
    DCD   0x00000000                /* Reserved */
    DCD   0x00000000                /* Reserved */
    DCD   NANO_OS_PORT_PendSvHandler  /* PendSV Handler */
    DCD   NANO_OS_PORT_SystickHandler /*SysTick Handler */

    /* External Interrupts */
    DCD   DMA0_Handler              /* 16: DMA Channel 0 Interrupt */
    DCD   DMA1_Handler              /* 17: DMA Channel 1 Interrupt */
    DCD   DMA2_Handler              /* 18: DMA Channel 2 Interrupt */
    DCD   DMA3_Handler              /* 19: DMA Channel 3 Interrupt */
    DCD   0x00000000                /* 20: Reserved Interrupt */
    DCD   FTFA_Handler              /* 21: FTFA Interrupt */
    DCD   PMC_Handler               /* 22: PMC Interrupt */
    DCD   LLWU_Handler              /* 23: LLWU Interrupt */
    DCD   I2C0_Handler              /* 24: I2C0 Interrupt */
    DCD   I2C1_Handler              /* 25: I2C1 Interrupt */
    DCD   SPI0_Handler              /* 26: SPI0 Interrupt */
    DCD   SPI1_Handler              /* 27: SPI1 Interrupt */
    DCD   UART0_Handler             /* 28: UART0 Interrupt */
    DCD   UART1_Handler             /* 29: UART1 Interrupt */
    DCD   UART2_Handler             /* 30: UART2 Interrupt */
    DCD   ADC_Handler               /* 31: ADC Interrupt */
    DCD   CMP_Handler               /* 32: CMP Interrupt */
    DCD   TPM0_Handler              /* 33: TPM0 Interrupt */
    DCD   TPM1_Handler              /* 34: TPM1 Interrupt */
    DCD   TPM2_Handler              /* 35: TPM2 Interrupt */
    DCD   RTC_Alarm_Handler         /* 36: RTC Alarm Interrupt */
    DCD   RTC_Seconds_Handler       /* 37: RTC Seconds Interrupt */
    DCD   PIT_Handler               /* 38: PIT Interrupt */
    DCD   0x00000000                /* 39: Reserved Interrupt */
    DCD   USB_OTG_Handler           /* 40: USB OTG Interrupt */
    DCD   DAC_Handler               /* 41: DAC Interrupt */
    DCD   TSI_Handler               /* 42: TSI Interrupt */
    DCD   MCG_Handler               /* 43: MCG Interrupt */
    DCD   LPTMR_Handler             /* 44: LPTMR Interrupt */
    DCD   0x00000000                /* 45: Reserved Interrupt */
    DCD   Pin_Detect_A_Handler      /* 46: Pin detect (PORTA) Interrupt */
    DCD   Pin_Detect_D_Handler      /* 47: Pin detect (PORTD) Interrupt */


	AREA |.cfm_protect|, CODE, READONLY, ALIGN=2

    /* Flash Configuration */

    DCD   0xFFFFFFFF
    DCD   0xFFFFFFFF
    DCD   0xFFFFFFFF
    DCD   0xFFFFFFFE

	AREA |.text|, CODE, READONLY, ALIGN=2
	THUMB

Reset_Handler

    /* PLL init */
    bl pll_init

    /* Branch to main */
    bl __main

    /* To prevent exiting from main */
    b       .
  

NMI_Handler
    b       .
    

HardFault_Handler
    b       .


MPUFault_Handler
    b       .


BusFault_Handler
    b       .


UsageFault_Handler
    b       .

    


DMA0_Handler
    b       .
    

DMA1_Handler
    b       .


DMA2_Handler
    b       .
    

DMA3_Handler
    b       .
    

FTFA_Handler
    b       .
    

PMC_Handler
    b       .


LLWU_Handler
    b       .
    

I2C0_Handler
    b       .


I2C1_Handler
    b       .
    

SPI0_Handler
    b       .
    

SPI1_Handler
    b       .
    

UART0_Handler
    b       .
    

UART1_Handler
    b       .
    

UART2_Handler
    b       .


ADC_Handler
    b       .
    

CMP_Handler
    b       .
    

TPM0_Handler
    b       .
    

TPM1_Handler
    b       .


TPM2_Handler
    b       .
    

RTC_Alarm_Handler
    b       .


RTC_Seconds_Handler
    b       .


PIT_Handler
    b       .
    

USB_OTG_Handler
    b       .
    

DAC_Handler
    b       .
    

TSI_Handler
    b       .
    

MCG_Handler
    b       .


LPTMR_Handler
    b       .
    

Pin_Detect_A_Handler
    b       .
    

Pin_Detect_D_Handler
    b       .
    
    
    
    END
