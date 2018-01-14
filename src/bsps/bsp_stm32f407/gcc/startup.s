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
    
    .weak     WWDG_IRQHandler           
    .weak     PVD_IRQHandler                
    .weak     TAMP_STAMP_IRQHandler         
    .weak     RTC_WKUP_IRQHandler           
    .weak     FLASH_IRQHandler              
    .weak     RCC_IRQHandler                
    .weak     EXTI0_IRQHandler              
    .weak     EXTI1_IRQHandler              
    .weak     EXTI2_IRQHandler              
    .weak     EXTI3_IRQHandler              
    .weak     EXTI4_IRQHandler              
    .weak     DMA1_Stream0_IRQHandler       
    .weak     DMA1_Stream1_IRQHandler       
    .weak     DMA1_Stream2_IRQHandler       
    .weak     DMA1_Stream3_IRQHandler       
    .weak     DMA1_Stream4_IRQHandler       
    .weak     DMA1_Stream5_IRQHandler       
    .weak     DMA1_Stream6_IRQHandler       
    .weak     ADC_IRQHandler                
    .weak     CAN1_TX_IRQHandler            
    .weak     CAN1_RX0_IRQHandler           
    .weak     CAN1_RX1_IRQHandler           
    .weak     CAN1_SCE_IRQHandler           
    .weak     EXTI9_5_IRQHandler            
    .weak     TIM1_BRK_TIM9_IRQHandler      
    .weak     TIM1_UP_TIM10_IRQHandler      
    .weak     TIM1_TRG_COM_TIM11_IRQHandler 
    .weak     TIM1_CC_IRQHandler            
    .weak     TIM2_IRQHandler 		  		
    .weak     TIM3_IRQHandler               
    .weak     TIM4_IRQHandler               
    .weak     I2C1_EV_IRQHandler            
    .weak     I2C1_ER_IRQHandler            
    .weak     I2C2_EV_IRQHandler            
    .weak     I2C2_ER_IRQHandler            
    .weak     SPI1_IRQHandler               
    .weak     SPI2_IRQHandler               
    .weak     USART1_IRQHandler             
    .weak     USART2_IRQHandler             
    .weak     USART3_IRQHandler           	
    .weak     EXTI15_10_IRQHandler          
    .weak     RTC_Alarm_IRQHandler          
    .weak     OTG_FS_WKUP_IRQHandler        
    .weak     TIM8_BRK_TIM12_IRQHandler     
    .weak     TIM8_UP_TIM13_IRQHandler      
    .weak     TIM8_TRG_COM_TIM14_IRQHandler 
    .weak     TIM8_CC_IRQHandler            
    .weak     DMA1_Stream7_IRQHandler       
    .weak     FSMC_IRQHandler               
    .weak     SDIO_IRQHandler               
    .weak     TIM5_IRQHandler               
    .weak     SPI3_IRQHandler               
    .weak     UART4_IRQHandler              
    .weak     UART5_IRQHandler              
    .weak     TIM6_DAC_IRQHandler           
    .weak     TIM7_IRQHandler               
    .weak     DMA2_Stream0_IRQHandler       
    .weak     DMA2_Stream1_IRQHandler       
    .weak     DMA2_Stream2_IRQHandler       
    .weak     DMA2_Stream3_IRQHandler       
    .weak     DMA2_Stream4_IRQHandler       
    .weak     ETH_IRQHandler                
    .weak     ETH_WKUP_IRQHandler           
    .weak     CAN2_TX_IRQHandler            
    .weak     CAN2_RX0_IRQHandler           
    .weak     CAN2_RX1_IRQHandler           
    .weak     CAN2_SCE_IRQHandler           
    .weak     OTG_FS_IRQHandler             
    .weak     DMA2_Stream5_IRQHandler       
    .weak     DMA2_Stream6_IRQHandler       
    .weak     DMA2_Stream7_IRQHandler       
    .weak     USART6_IRQHandler             
    .weak     I2C3_EV_IRQHandler            
    .weak     I2C3_ER_IRQHandler            
    .weak     OTG_HS_EP1_OUT_IRQHandler     
    .weak     OTG_HS_EP1_IN_IRQHandler      
    .weak     OTG_HS_WKUP_IRQHandler        
    .weak     OTG_HS_IRQHandler             
    .weak     DCMI_IRQHandler                                     
    .weak     HASH_RNG_IRQHandler           
    .weak     FPU_IRQHandler                



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
    .long     WWDG_IRQHandler                   /* Window WatchDog */                                       
    .long     PVD_IRQHandler                    /* PVD through EXTI Line detection */                       
    .long     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */            
    .long     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */                       
    .long     FLASH_IRQHandler                  /* FLASH */                                          
    .long     RCC_IRQHandler                    /* RCC */                                            
    .long     EXTI0_IRQHandler                  /* EXTI Line0 */                                            
    .long     EXTI1_IRQHandler                  /* EXTI Line1 */                                            
    .long     EXTI2_IRQHandler                  /* EXTI Line2 */                                            
    .long     EXTI3_IRQHandler                  /* EXTI Line3 */                                            
    .long     EXTI4_IRQHandler                  /* EXTI Line4 */                                            
    .long     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0 */                                  
    .long     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1 */                                  
    .long     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2 */                                  
    .long     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3 */                                  
    .long     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4 */                                  
    .long     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5 */                                  
    .long     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6 */                                  
    .long     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s */                            
    .long     CAN1_TX_IRQHandler                /* CAN1 TX */                                               
    .long     CAN1_RX0_IRQHandler               /* CAN1 RX0 */                                              
    .long     CAN1_RX1_IRQHandler               /* CAN1 RX1 */                                              
    .long     CAN1_SCE_IRQHandler               /* CAN1 SCE */                                              
    .long     EXTI9_5_IRQHandler                /* External Line[9:5]s */                                   
    .long     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9 */                  
    .long     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10 */                
    .long     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
    .long     TIM1_CC_IRQHandler                /* TIM1 Capture Compare */                                  
    .long     TIM2_IRQHandler 		  		  /* TIM2 was TIM2_IRQHandler */
    .long     TIM3_IRQHandler                   /* TIM3 */                                           
    .long     TIM4_IRQHandler                   /* TIM4 */                                           
    .long     I2C1_EV_IRQHandler                /* I2C1 Event */                                            
    .long     I2C1_ER_IRQHandler                /* I2C1 Error */                                            
    .long     I2C2_EV_IRQHandler                /* I2C2 Event */                                            
    .long     I2C2_ER_IRQHandler                /* I2C2 Error */                                              
    .long     SPI1_IRQHandler                   /* SPI1 */                                           
    .long     SPI2_IRQHandler                   /* SPI2 */                                           
    .long     USART1_IRQHandler                 /* USART1 */                                         
    .long     USART2_IRQHandler                 /* USART2 */                                         
    .long     USART3_IRQHandler           	  /* USART3 was USART3_IRQHandler */
    .long     EXTI15_10_IRQHandler              /* External Line[15:10]s */                                 
    .long     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */                 
    .long     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */                        
    .long     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12 */                 
    .long     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13 */                
    .long     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
    .long     TIM8_CC_IRQHandler                /* TIM8 Capture Compare */                                   
    .long     DMA1_Stream7_IRQHandler           /* DMA1 Stream7 */                                           
    .long     FSMC_IRQHandler                   /* FSMC */                                           
    .long     SDIO_IRQHandler                   /* SDIO */                                           
    .long     TIM5_IRQHandler                   /* TIM5 */                                           
    .long     SPI3_IRQHandler                   /* SPI3 */                                          
    .long     UART4_IRQHandler                  /* UART4 */                                          
    .long     UART5_IRQHandler                  /* UART5 */                                          
    .long     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */                  
    .long     TIM7_IRQHandler                   /* TIM7 */                  
    .long     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0 */                                  
    .long     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1 */                                  
    .long     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2 */                                  
    .long     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3 */                                  
    .long     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4 */                                  
    .long     ETH_IRQHandler                    /* Ethernet */                                       
    .long     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */                      
    .long     CAN2_TX_IRQHandler                /* CAN2 TX */                                               
    .long     CAN2_RX0_IRQHandler               /* CAN2 RX0 */                                              
    .long     CAN2_RX1_IRQHandler               /* CAN2 RX1 */                                              
    .long     CAN2_SCE_IRQHandler               /* CAN2 SCE */                                              
    .long     OTG_FS_IRQHandler                 /* USB OTG FS */                                     
    .long     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5 */                                  
    .long     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6 */                                  
    .long     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7 */                                  
    .long     USART6_IRQHandler                 /* USART6 */                                          
    .long     I2C3_EV_IRQHandler                /* I2C3 event */                                            
    .long     I2C3_ER_IRQHandler                /* I2C3 error */                                            
    .long     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out */                     
    .long     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In */                      
    .long     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */                       
    .long     OTG_HS_IRQHandler                 /* USB OTG HS */                                     
    .long     DCMI_IRQHandler                   /* DCMI */                                     
    .long     0                                 /* Reserved */                                     
    .long     HASH_RNG_IRQHandler               /* Hash and RNG */
    .long     FPU_IRQHandler                    /* FPU */


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
WWDG_IRQHandler:
    b        .
       
.thumb_func
PVD_IRQHandler:
    b        .
                
.thumb_func
TAMP_STAMP_IRQHandler:
    b        .
         
.thumb_func
RTC_WKUP_IRQHandler:
    b        .
           
.thumb_func
FLASH_IRQHandler:
    b        .
              
.thumb_func
RCC_IRQHandler:
    b        .
                
.thumb_func
EXTI0_IRQHandler:
    b        .
              
.thumb_func
EXTI1_IRQHandler:
    b        .
              
.thumb_func
EXTI2_IRQHandler:
    b        .
              
.thumb_func
EXTI3_IRQHandler:
    b        .
              
.thumb_func
EXTI4_IRQHandler:
    b        .
              
.thumb_func
DMA1_Stream0_IRQHandler:
    b        .
       
.thumb_func
DMA1_Stream1_IRQHandler:
    b        .
       
.thumb_func
DMA1_Stream2_IRQHandler:
    b        .
       
.thumb_func
DMA1_Stream3_IRQHandler:
    b        .
       
.thumb_func
DMA1_Stream4_IRQHandler:
    b        .
       
.thumb_func
DMA1_Stream5_IRQHandler:
    b        .
       
.thumb_func
DMA1_Stream6_IRQHandler:
    b        .
       
.thumb_func
ADC_IRQHandler:
    b        .
                
.thumb_func
CAN1_TX_IRQHandler:
    b        .
            
.thumb_func
CAN1_RX0_IRQHandler:
    b        .
           
.thumb_func
CAN1_RX1_IRQHandler:
    b        .
           
.thumb_func
CAN1_SCE_IRQHandler:
    b        .
           
.thumb_func
EXTI9_5_IRQHandler:
    b        .
            
.thumb_func
TIM1_BRK_TIM9_IRQHandler:
    b        .
      
.thumb_func
TIM1_UP_TIM10_IRQHandler:
    b        .
      
.thumb_func
TIM1_TRG_COM_TIM11_IRQHandler:
    b        .
 
.thumb_func
TIM1_CC_IRQHandler:
    b        .
            
.thumb_func
TIM2_IRQHandler:
    b        .
 		  		
.thumb_func
TIM3_IRQHandler:
    b        .
               
.thumb_func
TIM4_IRQHandler:
    b        .
               
.thumb_func
I2C1_EV_IRQHandler:
    b        .
            
.thumb_func
I2C1_ER_IRQHandler:
    b        .
            
.thumb_func
I2C2_EV_IRQHandler:
    b        .
            
.thumb_func
I2C2_ER_IRQHandler:
    b        .
            
.thumb_func
SPI1_IRQHandler:
    b        .
               
.thumb_func
SPI2_IRQHandler:
    b        .
               
.thumb_func
USART1_IRQHandler:
    b        .
             
.thumb_func
USART2_IRQHandler:
    b        .
             
.thumb_func
USART3_IRQHandler:
    b        .
           	
.thumb_func
EXTI15_10_IRQHandler:
    b        .
          
.thumb_func
RTC_Alarm_IRQHandler:
    b        .
          
.thumb_func
OTG_FS_WKUP_IRQHandler:
    b        .
        
.thumb_func
TIM8_BRK_TIM12_IRQHandler:
    b        .
     
.thumb_func
TIM8_UP_TIM13_IRQHandler:
    b        .
      
.thumb_func
TIM8_TRG_COM_TIM14_IRQHandler:
    b        .
 
.thumb_func
TIM8_CC_IRQHandler:
    b        .
            
.thumb_func
DMA1_Stream7_IRQHandler:
    b        .
       
.thumb_func
FSMC_IRQHandler:
    b        .
               
.thumb_func
SDIO_IRQHandler:
    b        .
               
.thumb_func
TIM5_IRQHandler:
    b        .
               
.thumb_func
SPI3_IRQHandler:
    b        .
               
.thumb_func
UART4_IRQHandler:
    b        .
              
.thumb_func
UART5_IRQHandler:
    b        .
              
.thumb_func
TIM6_DAC_IRQHandler:
    b        .
           
.thumb_func
TIM7_IRQHandler:
    b        .
               
.thumb_func
DMA2_Stream0_IRQHandler:
    b        .
       
.thumb_func
DMA2_Stream1_IRQHandler:
    b        .
       
.thumb_func
DMA2_Stream2_IRQHandler:
    b        .
       
.thumb_func
DMA2_Stream3_IRQHandler:
    b        .
       
.thumb_func
DMA2_Stream4_IRQHandler:
    b        .
       
.thumb_func
ETH_IRQHandler:
    b        .
                
.thumb_func
ETH_WKUP_IRQHandler:
    b        .
           
.thumb_func
CAN2_TX_IRQHandler:
    b        .
            
.thumb_func
CAN2_RX0_IRQHandler:
    b        .
           
.thumb_func
CAN2_RX1_IRQHandler:
    b        .
           
.thumb_func
CAN2_SCE_IRQHandler:
    b        .
           
.thumb_func
OTG_FS_IRQHandler:
    b        .
             
.thumb_func
DMA2_Stream5_IRQHandler:
    b        .
       
.thumb_func
DMA2_Stream6_IRQHandler:
    b        .
       
.thumb_func
DMA2_Stream7_IRQHandler:
    b        .
       
.thumb_func
USART6_IRQHandler:
    b        .
             
.thumb_func
I2C3_EV_IRQHandler:
    b        .
            
.thumb_func
I2C3_ER_IRQHandler:
    b        .
            
.thumb_func
OTG_HS_EP1_OUT_IRQHandler:
    b        .
     
.thumb_func
OTG_HS_EP1_IN_IRQHandler:
    b        .
      
.thumb_func
OTG_HS_WKUP_IRQHandler:
    b        .
        
.thumb_func
OTG_HS_IRQHandler:
    b        .
             
.thumb_func
DCMI_IRQHandler:
    b        .
                                     
.thumb_func
HASH_RNG_IRQHandler:
    b        .
           
.thumb_func
FPU_IRQHandler:
    b        .
                
    
    
    
    
    .end
