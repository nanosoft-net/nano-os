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
    .cpu cortex-m7
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
   
   
_start:
_vectors:
    /* Cortex M7 internal interrupts */
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
    .long   SUPC_Handler   /* 0  Supply Controller */
    .long   RSTC_Handler   /* 1  Reset Controller */
    .long   RTC_Handler    /* 2  Real Time Clock */
    .long   RTT_Handler    /* 3  Real Time Timer */
    .long   WDT_Handler    /* 4  Watchdog Timer */
    .long   PMC_Handler    /* 5  Power Management Controller */
    .long   EFC_Handler    /* 6  Enhanced Embedded Flash Controller */
    .long   UART0_Handler  /* 7  UART 0 */
    .long   UART1_Handler  /* 8  UART 1 */
    .long   0x00000000         /* 9  Reserved */
    .long   PIOA_Handler   /* 10 Parallel I/O Controller A */
    .long   PIOB_Handler   /* 11 Parallel I/O Controller B */
    .long   PIOC_Handler   /* 12 Parallel I/O Controller C */
    .long   USART0_Handler  /* 13 USART 0 */
    .long   USART1_Handler  /* 14 USART 1 */
    .long   USART2_Handler  /* 15 USART 2 */
    .long   PIOD_Handler   /* 16 Parallel I/O Controller D */
    .long   PIOE_Handler   /* 17 Parallel I/O Controller E */
    .long   HSMCI_Handler  /* 18 Multimedia Card Interface */
    .long   TWIHS0_Handler  /* 19 Two Wire Interface 0 HS */
    .long   TWIHS1_Handler  /* 20 Two Wire Interface 1 HS */
    .long   SPI0_Handler   /* 21 Serial Peripheral Interface 0 */
    .long   SSC_Handler    /* 22 Synchronous Serial Controller */
    .long   TC0_Handler    /* 23 Timer/Counter 0 */
    .long   TC1_Handler    /* 24 Timer/Counter 1 */
    .long   TC2_Handler    /* 25 Timer/Counter 2 */
    .long   TC3_Handler    /* 26 Timer/Counter 3 */
    .long   TC4_Handler    /* 27 Timer/Counter 4 */
    .long   TC5_Handler    /* 28 Timer/Counter 5 */
    .long   AFEC0_Handler  /* 29 Analog Front End 0 */
    .long   DACC_Handler   /* 30 Digital To Analog Converter */
    .long   PWM0_Handler   /* 31 Pulse Width Modulation 0 */
    .long   ICM_Handler    /* 32 Integrity Check Monitor */
    .long   ACC_Handler    /* 33 Analog Comparator */
    .long   USBHS_Handler  /* 34 USB Host / Device Controller */
    .long   MCAN0_Handler  /* 35 MCAN Controller 0 */
    .long   MCAN0_Line1_Handler  /* 36 CAN Controller 0 - Line 1  */
    .long   MCAN1_Handler  /* 37 MCAN Controller 1 */
    .long   MCAN1_Line1_Handler  /* 38 CAN Controller 1 - Line 1  */
    .long   GMAC_Handler   /* 39 Ethernet MAC */
    .long   AFEC1_Handler  /* 40 Analog Front End 1 */
    .long   TWIHS2_Handler  /* 41 Two Wire Interface 2 HS */
    .long   SPI1_Handler   /* 42 Serial Peripheral Interface 1 */
    .long   QSPI_Handler   /* 43 Quad I/O Serial Peripheral Interface */
    .long   UART2_Handler  /* 44 UART 2 */
    .long   UART3_Handler  /* 45 UART 3 */
    .long   UART4_Handler  /* 46 UART 4 */
    .long   TC6_Handler    /* 47 Timer/Counter 6 */
    .long   TC7_Handler    /* 48 Timer/Counter 7 */
    .long   TC8_Handler    /* 49 Timer/Counter 8 */
    .long   TC9_Handler    /* 50 Timer/Counter 9 */
    .long   TC10_Handler   /* 51 Timer/Counter 10 */
    .long   TC11_Handler   /* 52 Timer/Counter 11 */
    .long   0x00000000         /* 53 Reserved */
    .long   0x00000000         /* 54 Reserved */
    .long   0x00000000         /* 55 Reserved */
    .long   AES_Handler    /* 56 AES */
    .long   TRNG_Handler   /* 57 True Random Generator */
    .long   XDMAC_Handler  /* 58 DMA */
    .long   ISI_Handler    /* 59 Camera Interface */
    .long   PWM1_Handler   /* 60 Pulse Width Modulation 1 */
    .long   0x00000000         /* 61 Reserved */
    .long   SDRAMC_Handler  /* 62 SDRAM Controller */
    .long   RSWDT_Handler  /* 63 Reinforced Secure Watchdog Timer */   

  
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
    b  .

.thumb_func
.type Default_Handler, %function
Default_Handler:
	/* Default handler for unused interrupts */
	b  .


  
   .weak      NMI_Handler
   .thumb_set NMI_Handler,Default_Handler
  
   .weak      HardFault_Handler
   .thumb_set HardFault_Handler,Default_Handler
  
   .weak      MPUFault_Handler
   .thumb_set MPUFault_Handler,Default_Handler
  
   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      ACC_Handler
   .thumb_set ACC_Handler,Default_Handler

   .weak      AES_Handler
   .thumb_set AES_Handler,Default_Handler

   .weak      AFEC0_Handler
   .thumb_set AFEC0_Handler,Default_Handler

   .weak      AFEC1_Handler
   .thumb_set AFEC1_Handler,Default_Handler

   .weak      DACC_Handler
   .thumb_set DACC_Handler,Default_Handler

   .weak      EFC_Handler
   .thumb_set EFC_Handler,Default_Handler

   .weak      GMAC_Handler
   .thumb_set GMAC_Handler,Default_Handler

   .weak      HSMCI_Handler
   .thumb_set HSMCI_Handler,Default_Handler

   .weak      ICM_Handler
   .thumb_set ICM_Handler,Default_Handler

   .weak      ISI_Handler
   .thumb_set ISI_Handler,Default_Handler

   .weak      MCAN0_Handler
   .thumb_set MCAN0_Handler,Default_Handler

   .weak      MCAN0_Line1_Handler
   .thumb_set MCAN0_Line1_Handler,Default_Handler
   
   .weak      MCAN1_Handler
   .thumb_set MCAN1_Handler,Default_Handler

   .weak      MCAN1_Line1_Handler
   .thumb_set MCAN1_Line1_Handler,Default_Handler	
	
   .weak      PIOA_Handler
   .thumb_set PIOA_Handler,Default_Handler

   .weak      PIOB_Handler
   .thumb_set PIOB_Handler,Default_Handler

   .weak      PIOC_Handler
   .thumb_set PIOC_Handler,Default_Handler

   .weak      PIOD_Handler
   .thumb_set PIOD_Handler,Default_Handler

   .weak      PIOE_Handler
   .thumb_set PIOE_Handler,Default_Handler

   .weak      PMC_Handler
   .thumb_set PMC_Handler,Default_Handler

   .weak      PWM0_Handler
   .thumb_set PWM0_Handler,Default_Handler

   .weak      PWM1_Handler
   .thumb_set PWM1_Handler,Default_Handler

   .weak      QSPI_Handler
   .thumb_set QSPI_Handler,Default_Handler

   .weak      RSTC_Handler
   .thumb_set RSTC_Handler,Default_Handler

   .weak      RSWDT_Handler
   .thumb_set RSWDT_Handler,Default_Handler

   .weak      RTC_Handler
   .thumb_set RTC_Handler,Default_Handler

   .weak      RTT_Handler
   .thumb_set RTT_Handler,Default_Handler

   .weak      SDRAMC_Handler
   .thumb_set SDRAMC_Handler,Default_Handler

   .weak      SPI0_Handler
   .thumb_set SPI0_Handler,Default_Handler

   .weak      SPI1_Handler
   .thumb_set SPI1_Handler,Default_Handler

   .weak      SSC_Handler
   .thumb_set SSC_Handler,Default_Handler

   .weak      SUPC_Handler
   .thumb_set SUPC_Handler,Default_Handler

   .weak      TC0_Handler
   .thumb_set TC0_Handler,Default_Handler

   .weak      TC1_Handler
   .thumb_set TC1_Handler,Default_Handler

   .weak      TC2_Handler
   .thumb_set TC2_Handler,Default_Handler

   .weak      TC3_Handler
   .thumb_set TC3_Handler,Default_Handler

   .weak      TC4_Handler
   .thumb_set TC4_Handler,Default_Handler

   .weak      TC5_Handler
   .thumb_set TC5_Handler,Default_Handler

   .weak      TC6_Handler
   .thumb_set TC6_Handler,Default_Handler

   .weak      TC7_Handler
   .thumb_set TC7_Handler,Default_Handler

   .weak      TC8_Handler
   .thumb_set TC8_Handler,Default_Handler

   .weak      TC9_Handler
   .thumb_set TC9_Handler,Default_Handler

   .weak      TC10_Handler
   .thumb_set TC10_Handler,Default_Handler

   .weak      TC11_Handler
   .thumb_set TC11_Handler,Default_Handler

   .weak      TRNG_Handler
   .thumb_set TRNG_Handler,Default_Handler

   .weak      TWIHS0_Handler
   .thumb_set TWIHS0_Handler,Default_Handler

   .weak      TWIHS1_Handler
   .thumb_set TWIHS1_Handler,Default_Handler

   .weak      TWIHS2_Handler
   .thumb_set TWIHS2_Handler,Default_Handler

   .weak      UART0_Handler
   .thumb_set UART0_Handler,Default_Handler

   .weak      UART1_Handler
   .thumb_set UART1_Handler,Default_Handler

   .weak      UART2_Handler
   .thumb_set UART2_Handler,Default_Handler

   .weak      UART3_Handler
   .thumb_set UART3_Handler,Default_Handler

   .weak      UART4_Handler
   .thumb_set UART4_Handler,Default_Handler

   .weak      USART0_Handler
   .thumb_set USART0_Handler,Default_Handler

   .weak      USART1_Handler
   .thumb_set USART1_Handler,Default_Handler

   .weak      USART2_Handler
   .thumb_set USART2_Handler,Default_Handler

   .weak      USBHS_Handler
   .thumb_set USBHS_Handler,Default_Handler

   .weak      WDT_Handler
   .thumb_set WDT_Handler,Default_Handler

   .weak      XDMAC_Handler
   .thumb_set XDMAC_Handler,Default_Handler

   
   .end
   
