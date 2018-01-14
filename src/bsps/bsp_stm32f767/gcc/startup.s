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
    .long   MemManage_Handler         /* MPU Fault Handler */
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
    .long   WWDG_IRQHandler                   /* Window WatchDog              */
    .long   PVD_IRQHandler                    /* PVD through EXTI Line detection */
    .long   TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
    .long   RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
    .long   FLASH_IRQHandler                  /* FLASH                        */
    .long   RCC_IRQHandler                    /* RCC                          */
    .long   EXTI0_IRQHandler                  /* EXTI Line0                   */
    .long   EXTI1_IRQHandler                  /* EXTI Line1                   */
    .long   EXTI2_IRQHandler                  /* EXTI Line2                   */
    .long   EXTI3_IRQHandler                  /* EXTI Line3                   */
    .long   EXTI4_IRQHandler                  /* EXTI Line4                   */
    .long   DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
    .long   DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
    .long   DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
    .long   DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
    .long   DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
    .long   DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
    .long   DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
    .long   ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
    .long   CAN1_TX_IRQHandler                /* CAN1 TX                      */
    .long   CAN1_RX0_IRQHandler               /* CAN1 RX0                     */
    .long   CAN1_RX1_IRQHandler               /* CAN1 RX1                     */
    .long   CAN1_SCE_IRQHandler               /* CAN1 SCE                     */
    .long   EXTI9_5_IRQHandler                /* External Line[9:5]s          */
    .long   TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9          */
    .long   TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10        */
    .long   TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
    .long   TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
    .long   TIM2_IRQHandler                   /* TIM2                         */
    .long   TIM3_IRQHandler                   /* TIM3                         */
    .long   TIM4_IRQHandler                   /* TIM4                         */
    .long   I2C1_EV_IRQHandler                /* I2C1 Event                   */
    .long   I2C1_ER_IRQHandler                /* I2C1 Error                   */
    .long   I2C2_EV_IRQHandler                /* I2C2 Event                   */
    .long   I2C2_ER_IRQHandler                /* I2C2 Error                   */
    .long   SPI1_IRQHandler                   /* SPI1                         */
    .long   SPI2_IRQHandler                   /* SPI2                         */
    .long   USART1_IRQHandler                 /* USART1                       */
    .long   USART2_IRQHandler                 /* USART2                       */
    .long   USART3_IRQHandler                 /* USART3                       */
    .long   EXTI15_10_IRQHandler              /* External Line[15:10]s        */
    .long   RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
    .long   OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */
    .long   TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
    .long   TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
    .long   TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
    .long   TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
    .long   DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
    .long   FMC_IRQHandler                    /* FMC                          */
    .long   SDMMC1_IRQHandler                 /* SDMMC1                       */
    .long   TIM5_IRQHandler                   /* TIM5                         */
    .long   SPI3_IRQHandler                   /* SPI3                         */
    .long   UART4_IRQHandler                  /* UART4                        */
    .long   UART5_IRQHandler                  /* UART5                        */
    .long   TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
    .long   TIM7_IRQHandler                   /* TIM7                         */
    .long   DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
    .long   DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
    .long   DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
    .long   DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
    .long   DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
    .long   ETH_IRQHandler                    /* Ethernet                     */
    .long   ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
    .long   CAN2_TX_IRQHandler                /* CAN2 TX                      */
    .long   CAN2_RX0_IRQHandler               /* CAN2 RX0                     */
    .long   CAN2_RX1_IRQHandler               /* CAN2 RX1                     */
    .long   CAN2_SCE_IRQHandler               /* CAN2 SCE                     */
    .long   OTG_FS_IRQHandler                 /* USB OTG FS                   */
    .long   DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
    .long   DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
    .long   DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
    .long   USART6_IRQHandler                 /* USART6                       */
    .long   I2C3_EV_IRQHandler                /* I2C3 event                   */
    .long   I2C3_ER_IRQHandler                /* I2C3 error                   */
    .long   OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
    .long   OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
    .long   OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
    .long   OTG_HS_IRQHandler                 /* USB OTG HS                   */
    .long   DCMI_IRQHandler                   /* DCMI                         */
    .long   0                                 /* Reserved                     */
    .long   RNG_IRQHandler                    /* RNG                          */
    .long   FPU_IRQHandler                    /* FPU                          */
    .long   UART7_IRQHandler                  /* UART7                        */
    .long   UART8_IRQHandler                  /* UART8                        */
    .long   SPI4_IRQHandler                   /* SPI4                         */
    .long   SPI5_IRQHandler                   /* SPI5                         */
    .long   SPI6_IRQHandler                   /* SPI6                         */
    .long   SAI1_IRQHandler                   /* SAI1                         */
    .long   LTDC_IRQHandler                   /* LTDC                         */
    .long   LTDC_ER_IRQHandler                /* LTDC error                   */
    .long   DMA2D_IRQHandler                  /* DMA2D                        */
    .long   SAI2_IRQHandler                   /* SAI2                         */
    .long   QUADSPI_IRQHandler                /* QUADSPI                      */
    .long   LPTIM1_IRQHandler                 /* LPTIM1                       */
    .long   CEC_IRQHandler                    /* HDMI_CEC                     */
    .long   I2C4_EV_IRQHandler                /* I2C4 Event                   */
    .long   I2C4_ER_IRQHandler                /* I2C4 Error                   */
    .long   SPDIF_RX_IRQHandler               /* SPDIF_RX                     */
    .long   0                                 /* Reserved                     */
    .long   DFSDM1_FLT0_IRQHandler            /* DFSDM1 Filter 0 global Interrupt */
    .long   DFSDM1_FLT1_IRQHandler            /* DFSDM1 Filter 1 global Interrupt */
    .long   DFSDM1_FLT2_IRQHandler            /* DFSDM1 Filter 2 global Interrupt */
    .long   DFSDM1_FLT3_IRQHandler            /* DFSDM1 Filter 3 global Interrupt */
    .long   SDMMC2_IRQHandler                 /* SDMMC2                       */
    .long   CAN3_TX_IRQHandler                /* CAN3 TX                      */
    .long   CAN3_RX0_IRQHandler               /* CAN3 RX0                     */
    .long   CAN3_RX1_IRQHandler               /* CAN3 RX1                     */
    .long   CAN3_SCE_IRQHandler               /* CAN3 SCE                     */
    .long   JPEG_IRQHandler                   /* JPEG                         */
    .long   MDIOS_IRQHandler                  /* MDIOS                        */
  
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
  
   .weak      MemManage_Handler
   .thumb_set MemManage_Handler,Default_Handler
  
   .weak      BusFault_Handler
   .thumb_set BusFault_Handler,Default_Handler

   .weak      UsageFault_Handler
   .thumb_set UsageFault_Handler,Default_Handler

   .weak      DebugMon_Handler
   .thumb_set DebugMon_Handler,Default_Handler

   .weak      WWDG_IRQHandler                   
   .thumb_set WWDG_IRQHandler,Default_Handler      
                  
   .weak      PVD_IRQHandler      
   .thumb_set PVD_IRQHandler,Default_Handler
               
   .weak      TAMP_STAMP_IRQHandler            
   .thumb_set TAMP_STAMP_IRQHandler,Default_Handler
            
   .weak      RTC_WKUP_IRQHandler                  
   .thumb_set RTC_WKUP_IRQHandler,Default_Handler
            
   .weak      FLASH_IRQHandler         
   .thumb_set FLASH_IRQHandler,Default_Handler
                  
   .weak      RCC_IRQHandler      
   .thumb_set RCC_IRQHandler,Default_Handler
                  
   .weak      EXTI0_IRQHandler         
   .thumb_set EXTI0_IRQHandler,Default_Handler
                  
   .weak      EXTI1_IRQHandler         
   .thumb_set EXTI1_IRQHandler,Default_Handler
                     
   .weak      EXTI2_IRQHandler         
   .thumb_set EXTI2_IRQHandler,Default_Handler 
                 
   .weak      EXTI3_IRQHandler         
   .thumb_set EXTI3_IRQHandler,Default_Handler
                        
   .weak      EXTI4_IRQHandler         
   .thumb_set EXTI4_IRQHandler,Default_Handler
                  
   .weak      DMA1_Stream0_IRQHandler               
   .thumb_set DMA1_Stream0_IRQHandler,Default_Handler
         
   .weak      DMA1_Stream1_IRQHandler               
   .thumb_set DMA1_Stream1_IRQHandler,Default_Handler
                  
   .weak      DMA1_Stream2_IRQHandler               
   .thumb_set DMA1_Stream2_IRQHandler,Default_Handler
                  
   .weak      DMA1_Stream3_IRQHandler               
   .thumb_set DMA1_Stream3_IRQHandler,Default_Handler 
                 
   .weak      DMA1_Stream4_IRQHandler              
   .thumb_set DMA1_Stream4_IRQHandler,Default_Handler
                  
   .weak      DMA1_Stream5_IRQHandler               
   .thumb_set DMA1_Stream5_IRQHandler,Default_Handler
                  
   .weak      DMA1_Stream6_IRQHandler               
   .thumb_set DMA1_Stream6_IRQHandler,Default_Handler
                  
   .weak      ADC_IRQHandler      
   .thumb_set ADC_IRQHandler,Default_Handler
               
   .weak      CAN1_TX_IRQHandler   
   .thumb_set CAN1_TX_IRQHandler,Default_Handler
            
   .weak      CAN1_RX0_IRQHandler                  
   .thumb_set CAN1_RX0_IRQHandler,Default_Handler
                           
   .weak      CAN1_RX1_IRQHandler                  
   .thumb_set CAN1_RX1_IRQHandler,Default_Handler
            
   .weak      CAN1_SCE_IRQHandler                  
   .thumb_set CAN1_SCE_IRQHandler,Default_Handler
            
   .weak      EXTI9_5_IRQHandler   
   .thumb_set EXTI9_5_IRQHandler,Default_Handler
            
   .weak      TIM1_BRK_TIM9_IRQHandler            
   .thumb_set TIM1_BRK_TIM9_IRQHandler,Default_Handler
            
   .weak      TIM1_UP_TIM10_IRQHandler            
   .thumb_set TIM1_UP_TIM10_IRQHandler,Default_Handler

   .weak      TIM1_TRG_COM_TIM11_IRQHandler      
   .thumb_set TIM1_TRG_COM_TIM11_IRQHandler,Default_Handler
      
   .weak      TIM1_CC_IRQHandler   
   .thumb_set TIM1_CC_IRQHandler,Default_Handler
                  
   .weak      TIM2_IRQHandler            
   .thumb_set TIM2_IRQHandler,Default_Handler
                  
   .weak      TIM3_IRQHandler            
   .thumb_set TIM3_IRQHandler,Default_Handler
                  
   .weak      TIM4_IRQHandler            
   .thumb_set TIM4_IRQHandler,Default_Handler
                  
   .weak      I2C1_EV_IRQHandler   
   .thumb_set I2C1_EV_IRQHandler,Default_Handler
                     
   .weak      I2C1_ER_IRQHandler   
   .thumb_set I2C1_ER_IRQHandler,Default_Handler
                     
   .weak      I2C2_EV_IRQHandler   
   .thumb_set I2C2_EV_IRQHandler,Default_Handler
                  
   .weak      I2C2_ER_IRQHandler   
   .thumb_set I2C2_ER_IRQHandler,Default_Handler
                           
   .weak      SPI1_IRQHandler            
   .thumb_set SPI1_IRQHandler,Default_Handler
                        
   .weak      SPI2_IRQHandler            
   .thumb_set SPI2_IRQHandler,Default_Handler
                  
   .weak      USART1_IRQHandler      
   .thumb_set USART1_IRQHandler,Default_Handler
                     
   .weak      USART2_IRQHandler      
   .thumb_set USART2_IRQHandler,Default_Handler
                     
   .weak      USART3_IRQHandler      
   .thumb_set USART3_IRQHandler,Default_Handler
                  
   .weak      EXTI15_10_IRQHandler               
   .thumb_set EXTI15_10_IRQHandler,Default_Handler
               
   .weak      RTC_Alarm_IRQHandler               
   .thumb_set RTC_Alarm_IRQHandler,Default_Handler
            
   .weak      OTG_FS_WKUP_IRQHandler         
   .thumb_set OTG_FS_WKUP_IRQHandler,Default_Handler
            
   .weak      TIM8_BRK_TIM12_IRQHandler         
   .thumb_set TIM8_BRK_TIM12_IRQHandler,Default_Handler
         
   .weak      TIM8_UP_TIM13_IRQHandler            
   .thumb_set TIM8_UP_TIM13_IRQHandler,Default_Handler
         
   .weak      TIM8_TRG_COM_TIM14_IRQHandler      
   .thumb_set TIM8_TRG_COM_TIM14_IRQHandler,Default_Handler
      
   .weak      TIM8_CC_IRQHandler   
   .thumb_set TIM8_CC_IRQHandler,Default_Handler
                  
   .weak      DMA1_Stream7_IRQHandler               
   .thumb_set DMA1_Stream7_IRQHandler,Default_Handler
                     
   .weak      FMC_IRQHandler            
   .thumb_set FMC_IRQHandler,Default_Handler
                     
   .weak      SDMMC1_IRQHandler            
   .thumb_set SDMMC1_IRQHandler,Default_Handler
                     
   .weak      TIM5_IRQHandler            
   .thumb_set TIM5_IRQHandler,Default_Handler
                     
   .weak      SPI3_IRQHandler            
   .thumb_set SPI3_IRQHandler,Default_Handler
                     
   .weak      UART4_IRQHandler         
   .thumb_set UART4_IRQHandler,Default_Handler
                  
   .weak      UART5_IRQHandler         
   .thumb_set UART5_IRQHandler,Default_Handler
                  
   .weak      TIM6_DAC_IRQHandler                  
   .thumb_set TIM6_DAC_IRQHandler,Default_Handler
               
   .weak      TIM7_IRQHandler            
   .thumb_set TIM7_IRQHandler,Default_Handler
         
   .weak      DMA2_Stream0_IRQHandler               
   .thumb_set DMA2_Stream0_IRQHandler,Default_Handler
               
   .weak      DMA2_Stream1_IRQHandler               
   .thumb_set DMA2_Stream1_IRQHandler,Default_Handler
                  
   .weak      DMA2_Stream2_IRQHandler               
   .thumb_set DMA2_Stream2_IRQHandler,Default_Handler
            
   .weak      DMA2_Stream3_IRQHandler               
   .thumb_set DMA2_Stream3_IRQHandler,Default_Handler
            
   .weak      DMA2_Stream4_IRQHandler               
   .thumb_set DMA2_Stream4_IRQHandler,Default_Handler
   
   .weak      DMA2_Stream4_IRQHandler               
   .thumb_set DMA2_Stream4_IRQHandler,Default_Handler   

   .weak      ETH_IRQHandler   
   .thumb_set ETH_IRQHandler,Default_Handler
   
   .weak      ETH_WKUP_IRQHandler   
   .thumb_set ETH_WKUP_IRQHandler,Default_Handler

   .weak      CAN2_TX_IRQHandler   
   .thumb_set CAN2_TX_IRQHandler,Default_Handler   
                           
   .weak      CAN2_RX0_IRQHandler                  
   .thumb_set CAN2_RX0_IRQHandler,Default_Handler
                           
   .weak      CAN2_RX1_IRQHandler                  
   .thumb_set CAN2_RX1_IRQHandler,Default_Handler
                           
   .weak      CAN2_SCE_IRQHandler                  
   .thumb_set CAN2_SCE_IRQHandler,Default_Handler
                           
   .weak      OTG_FS_IRQHandler      
   .thumb_set OTG_FS_IRQHandler,Default_Handler
                     
   .weak      DMA2_Stream5_IRQHandler               
   .thumb_set DMA2_Stream5_IRQHandler,Default_Handler
                  
   .weak      DMA2_Stream6_IRQHandler               
   .thumb_set DMA2_Stream6_IRQHandler,Default_Handler
                  
   .weak      DMA2_Stream7_IRQHandler               
   .thumb_set DMA2_Stream7_IRQHandler,Default_Handler
                  
   .weak      USART6_IRQHandler      
   .thumb_set USART6_IRQHandler,Default_Handler
                        
   .weak      I2C3_EV_IRQHandler   
   .thumb_set I2C3_EV_IRQHandler,Default_Handler
                        
   .weak      I2C3_ER_IRQHandler   
   .thumb_set I2C3_ER_IRQHandler,Default_Handler
                        
   .weak      OTG_HS_EP1_OUT_IRQHandler         
   .thumb_set OTG_HS_EP1_OUT_IRQHandler,Default_Handler
               
   .weak      OTG_HS_EP1_IN_IRQHandler            
   .thumb_set OTG_HS_EP1_IN_IRQHandler,Default_Handler
               
   .weak      OTG_HS_WKUP_IRQHandler         
   .thumb_set OTG_HS_WKUP_IRQHandler,Default_Handler
            
   .weak      OTG_HS_IRQHandler      
   .thumb_set OTG_HS_IRQHandler,Default_Handler
                  
   .weak      DCMI_IRQHandler            
   .thumb_set DCMI_IRQHandler,Default_Handler

   .weak      RNG_IRQHandler            
   .thumb_set RNG_IRQHandler,Default_Handler   

   .weak      FPU_IRQHandler                  
   .thumb_set FPU_IRQHandler,Default_Handler

   .weak      UART7_IRQHandler                  
   .thumb_set UART7_IRQHandler,Default_Handler

   .weak      UART8_IRQHandler                  
   .thumb_set UART8_IRQHandler,Default_Handler   

   .weak      SPI4_IRQHandler            
   .thumb_set SPI4_IRQHandler,Default_Handler
   
   .weak      SPI5_IRQHandler            
   .thumb_set SPI5_IRQHandler,Default_Handler

   .weak      SPI6_IRQHandler            
   .thumb_set SPI6_IRQHandler,Default_Handler   

   .weak      SAI1_IRQHandler            
   .thumb_set SAI1_IRQHandler,Default_Handler
   
   .weak      LTDC_IRQHandler            
   .thumb_set LTDC_IRQHandler,Default_Handler

   .weak      LTDC_ER_IRQHandler            
   .thumb_set LTDC_ER_IRQHandler,Default_Handler

   .weak      DMA2D_IRQHandler            
   .thumb_set DMA2D_IRQHandler,Default_Handler   

   .weak      SAI2_IRQHandler            
   .thumb_set SAI2_IRQHandler,Default_Handler
   
   .weak      QUADSPI_IRQHandler            
   .thumb_set QUADSPI_IRQHandler,Default_Handler
 
   .weak      LPTIM1_IRQHandler            
   .thumb_set LPTIM1_IRQHandler,Default_Handler

   .weak      CEC_IRQHandler            
   .thumb_set CEC_IRQHandler,Default_Handler
   
   .weak      I2C4_EV_IRQHandler            
   .thumb_set I2C4_EV_IRQHandler,Default_Handler 
 
   .weak      I2C4_ER_IRQHandler            
   .thumb_set I2C4_ER_IRQHandler,Default_Handler
   
   .weak      SPDIF_RX_IRQHandler            
   .thumb_set SPDIF_RX_IRQHandler,Default_Handler

   .weak      DFSDM1_FLT0_IRQHandler            
   .thumb_set DFSDM1_FLT0_IRQHandler,Default_Handler

   .weak      DFSDM1_FLT1_IRQHandler            
   .thumb_set DFSDM1_FLT1_IRQHandler,Default_Handler

   .weak      DFSDM1_FLT2_IRQHandler            
   .thumb_set DFSDM1_FLT2_IRQHandler,Default_Handler

   .weak      DFSDM1_FLT3_IRQHandler            
   .thumb_set DFSDM1_FLT3_IRQHandler,Default_Handler

   .weak      SDMMC2_IRQHandler            
   .thumb_set SDMMC2_IRQHandler,Default_Handler

   .weak      CAN3_TX_IRQHandler            
   .thumb_set CAN3_TX_IRQHandler,Default_Handler

   .weak      CAN3_RX0_IRQHandler            
   .thumb_set CAN3_RX0_IRQHandler,Default_Handler

   .weak      CAN3_RX1_IRQHandler            
   .thumb_set CAN3_RX1_IRQHandler,Default_Handler

   .weak      CAN3_SCE_IRQHandler            
   .thumb_set CAN3_SCE_IRQHandler,Default_Handler

   .weak      JPEG_IRQHandler            
   .thumb_set JPEG_IRQHandler,Default_Handler

   .weak      MDIOS_IRQHandler            
   .thumb_set MDIOS_IRQHandler,Default_Handler   

   
   .end
   
