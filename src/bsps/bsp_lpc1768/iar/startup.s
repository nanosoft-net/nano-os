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
    /* Cortex M3 internal interrupts */
    DCD   sfe(CSTACK)               /* Main stack pointer */
    DCD   Reset_Handler             /* Reset Handler */
    DCD   NMI_Handler               /* NMI Handler */
    DCD   HardFault_Handler         /* Hard Fault Handler */
    DCD   MPUFault_Handler          /* MPU Fault Handler */
    DCD   BusFault_Handler          /* Bus Fault Handler */
    DCD   UsageFault_Handler        /* Usage Fault Handler */
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
    DCD   WatchdogTimer_Handler     /* 16: Watchdog Timer */
    DCD   Timer0_Handler            /* 17: Timer0 */
    DCD   Timer1_Handler            /* 18: Timer1 */
    DCD   Timer2_Handler            /* 19: Timer2 */
    DCD   Timer3_Handler            /* 20: Timer3 */
    DCD   UART0_Handler             /* 21: UART0 */
    DCD   UART1_Handler             /* 22: UART1 */
    DCD   UART2_Handler             /* 23: UART2 */
    DCD   UART3_Handler             /* 24: UART3 */
    DCD   PWM1_Handler              /* 25: PWM1 */
    DCD   I2C0_Handler              /* 26: I2C0 */
    DCD   I2C1_Handler              /* 27: I2C1 */
    DCD   I2C2_Handler              /* 28: I2C2 */
    DCD   SPI_Handler               /* 29: SPI */
    DCD   SSP0_Handler              /* 30: SSP0 */
    DCD   SSP1_Handler              /* 31: SSP1 */
    DCD   PLL0_Handler              /* 32: PLL0 Lock (Main PLL) */
    DCD   RTC_Handler               /* 33: Real Time Clock */
    DCD   ExternalInt0_Handler      /* 34: External Interrupt 0 */
    DCD   ExternalInt1_Handler      /* 35: External Interrupt 1 */
    DCD   ExternalInt2_Handler      /* 36: External Interrupt 2 */
    DCD   ExternalInt3_Handler      /* 37: External Interrupt 3 */
    DCD   ADC_Handler               /* 38: A/D Converter */
    DCD   BOD_Handler               /* 39: Brown-Out Detect */
    DCD   USB_Handler               /* 40: USB */
    DCD   CAN_Handler               /* 41: CAN */
    DCD   GPDMA_Handler             /* 42: General Purpose DMA */
    DCD   I2S_Handler               /* 43: I2S */
    DCD   Ethernet_Handler          /* 44: Ethernet */
    DCD   RIT_Handler               /* 45: Repetitive Interrupt Timer */
    DCD   MotorControl_Handler      /* 46: Motor Control PWM */
    DCD   QEI_Handler               /* 47: Quadrature Encoder Interface */
    DCD   PLL1_Handler              /* 48: PLL1 Lock (USB PLL) */
    DCD   USBActivity_Handler       /* 49: USB Activity interrupt to wakeup */
    DCD   CANActivity_Handler       /* 50: CAN Activity interrupt to wakeup */


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

    

	PUBWEAK WatchdogTimer_Handler
WatchdogTimer_Handler:
    b       .
    
	PUBWEAK Timer0_Handler
Timer0_Handler:
    b       .
   
	PUBWEAK Timer1_Handler
Timer1_Handler:
    b       .
   
	PUBWEAK Timer2_Handler
Timer2_Handler:
    b       .
   
	PUBWEAK Timer3_Handler
Timer3_Handler:
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
   
	PUBWEAK UART3_Handler
UART3_Handler:
    b       .   
     
	PUBWEAK PWM1_Handler
PWM1_Handler:
    b       .
        
	PUBWEAK I2C0_Handler
I2C0_Handler:
    b       .
        
	PUBWEAK I2C1_Handler
I2C1_Handler:
    b       .
       
	PUBWEAK I2C2_Handler
I2C2_Handler:
    b       .

	PUBWEAK SPI_Handler
SPI_Handler:
    b       .     
       
	PUBWEAK SSP0_Handler
SSP0_Handler:
    b       .
      
	PUBWEAK SSP1_Handler
SSP1_Handler:
    b       .
        
	PUBWEAK PLL0_Handler
PLL0_Handler:
    b       .
     
	PUBWEAK RTC_Handler
RTC_Handler:
    b       .

	PUBWEAK ExternalInt0_Handler
ExternalInt0_Handler:
    b       .

	PUBWEAK ExternalInt1_Handler
ExternalInt1_Handler:
    b       .

	PUBWEAK ExternalInt2_Handler
ExternalInt2_Handler:
    b       .

	PUBWEAK ExternalInt3_Handler
ExternalInt3_Handler:
    b       .
     
	PUBWEAK ADC_Handler
ADC_Handler:
    b       .

	PUBWEAK BOD_Handler
BOD_Handler:
    b       .       
        
	PUBWEAK USB_Handler
USB_Handler:
    b       .
        
	PUBWEAK CAN_Handler
CAN_Handler:
    b       .
      
	PUBWEAK GPDMA_Handler
GPDMA_Handler:
    b       .
        
	PUBWEAK I2S_Handler
I2S_Handler:
    b       .
   
	PUBWEAK Ethernet_Handler
Ethernet_Handler:
    b       .
       
	PUBWEAK RIT_Handler
RIT_Handler:
    b       .

	PUBWEAK MotorControl_Handler
MotorControl_Handler:
    b       .
        
	PUBWEAK QEI_Handler
QEI_Handler:
    b       .
       
	PUBWEAK PLL1_Handler
PLL1_Handler:
    b       .
  
	PUBWEAK USBActivity_Handler
USBActivity_Handler:
    b       .

	PUBWEAK CANActivity_Handler
CANActivity_Handler:
    b       .
    
    
    END
