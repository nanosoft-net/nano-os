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

	MODULE  ?cstartup

	;; Forward declaration of sections.
    SECTION CSTACK:DATA:NOROOT(3)

    SECTION .startup:CODE:NOROOT(2)
	THUMB

	PUBLIC _vectors
	PUBLIC Reset_Handler

    EXTERN pll_init
    EXTERN main
    EXTERN  __iar_program_start

    EXTERN NANO_OS_PORT_SvcHandler
	EXTERN NANO_OS_PORT_PendSvHandler
	EXTERN NANO_OS_PORT_SystickHandler


	DATA
_vectors
    /* Cortex M0+ internal interrupts */
    DCD   sfe(CSTACK)               /* Main stack pointer */
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


    /* Flash Configuration */

    DCD   0xFFFFFFFF
    DCD   0xFFFFFFFF
    DCD   0xFFFFFFFF
    DCD   0xFFFFFFFE



Reset_Handler:

    /* PLL init */
    bl pll_init

    /* C vars init and branch to main */
    bl __iar_program_start

    /* To prevent exiting from main */
    b       .
  
	PUBWEAK NMI_Handler
NMI_Handler:
    b       .
    
	PUBWEAK HardFault_Handler
HardFault_Handler:
    b       .

	PUBWEAK MPUFault_Handler
MPUFault_Handler:
    b       .

	PUBWEAK BusFault_Handler
BusFault_Handler:
    b       .

	PUBWEAK UsageFault_Handler
UsageFault_Handler:
    b       .

    

	PUBWEAK DMA0_Handler
DMA0_Handler:
    b       .
    
	PUBWEAK DMA1_Handler
DMA1_Handler:
    b       .

	PUBWEAK DMA2_Handler
DMA2_Handler:
    b       .
    
	PUBWEAK DMA3_Handler
DMA3_Handler:
    b       .
    
	PUBWEAK FTFA_Handler
FTFA_Handler:
    b       .
    
	PUBWEAK PMC_Handler
PMC_Handler:
    b       .

	PUBWEAK LLWU_Handler
LLWU_Handler:
    b       .
    
	PUBWEAK I2C0_Handler
I2C0_Handler:
    b       .

	PUBWEAK I2C1_Handler
I2C1_Handler:
    b       .
    
	PUBWEAK SPI0_Handler
SPI0_Handler:
    b       .
    
	PUBWEAK SPI1_Handler
SPI1_Handler:
    b       .
    
	PUBWEAK UART0_Handler
UART0_Handler:
    b       .
    
	PUBWEAK UART1_Handler
UART1_Handler:
    b       .
    
	PUBWEAK UART2_Handler
UART2_Handler:
    b       .

	PUBWEAK ADC_Handler
ADC_Handler:
    b       .
    
	PUBWEAK CMP_Handler
CMP_Handler:
    b       .
    
	PUBWEAK TPM0_Handler
TPM0_Handler:
    b       .
    
	PUBWEAK TPM1_Handler
TPM1_Handler:
    b       .

	PUBWEAK TPM2_Handler
TPM2_Handler:
    b       .
    
	PUBWEAK RTC_Alarm_Handler
RTC_Alarm_Handler:
    b       .

	PUBWEAK RTC_Seconds_Handler
RTC_Seconds_Handler:
    b       .

	PUBWEAK PIT_Handler
PIT_Handler:
    b       .
    
	PUBWEAK USB_OTG_Handler
USB_OTG_Handler:
    b       .
    
	PUBWEAK DAC_Handler
DAC_Handler:
    b       .
    
	PUBWEAK TSI_Handler
TSI_Handler:
    b       .
    
	PUBWEAK MCG_Handler
MCG_Handler:
    b       .

	PUBWEAK LPTMR_Handler
LPTMR_Handler:
    b       .
    
	PUBWEAK Pin_Detect_A_Handler
Pin_Detect_A_Handler:
    b       .
    
	PUBWEAK Pin_Detect_D_Handler
Pin_Detect_D_Handler:
    b       .
    
    
    
    END
