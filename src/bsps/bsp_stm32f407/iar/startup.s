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
    /* Cortex M4 internal interrupts */
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
    DCD     WWDG_IRQHandler                   /* Window WatchDog */                                       
    DCD     PVD_IRQHandler                    /* PVD through EXTI Line detection */                       
    DCD     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */            
    DCD     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */                       
    DCD     FLASH_IRQHandler                  /* FLASH */                                          
    DCD     RCC_IRQHandler                    /* RCC */                                            
    DCD     EXTI0_IRQHandler                  /* EXTI Line0 */                                            
    DCD     EXTI1_IRQHandler                  /* EXTI Line1 */                                            
    DCD     EXTI2_IRQHandler                  /* EXTI Line2 */                                            
    DCD     EXTI3_IRQHandler                  /* EXTI Line3 */                                            
    DCD     EXTI4_IRQHandler                  /* EXTI Line4 */                                            
    DCD     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0 */                                  
    DCD     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1 */                                  
    DCD     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2 */                                  
    DCD     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3 */                                  
    DCD     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4 */                                  
    DCD     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5 */                                  
    DCD     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6 */                                  
    DCD     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s */                            
    DCD     CAN1_TX_IRQHandler                /* CAN1 TX */                                               
    DCD     CAN1_RX0_IRQHandler               /* CAN1 RX0 */                                              
    DCD     CAN1_RX1_IRQHandler               /* CAN1 RX1 */                                              
    DCD     CAN1_SCE_IRQHandler               /* CAN1 SCE */                                              
    DCD     EXTI9_5_IRQHandler                /* External Line[9:5]s */                                   
    DCD     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9 */                  
    DCD     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10 */                
    DCD     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
    DCD     TIM1_CC_IRQHandler                /* TIM1 Capture Compare */                                  
    DCD     TIM2_IRQHandler 		  		  /* TIM2 was TIM2_IRQHandler */
    DCD     TIM3_IRQHandler                   /* TIM3 */                                           
    DCD     TIM4_IRQHandler                   /* TIM4 */                                           
    DCD     I2C1_EV_IRQHandler                /* I2C1 Event */                                            
    DCD     I2C1_ER_IRQHandler                /* I2C1 Error */                                            
    DCD     I2C2_EV_IRQHandler                /* I2C2 Event */                                            
    DCD     I2C2_ER_IRQHandler                /* I2C2 Error */                                              
    DCD     SPI1_IRQHandler                   /* SPI1 */                                           
    DCD     SPI2_IRQHandler                   /* SPI2 */                                           
    DCD     USART1_IRQHandler                 /* USART1 */                                         
    DCD     USART2_IRQHandler                 /* USART2 */                                         
    DCD     USART3_IRQHandler           	  /* USART3 was USART3_IRQHandler */
    DCD     EXTI15_10_IRQHandler              /* External Line[15:10]s */                                 
    DCD     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */                 
    DCD     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */                        
    DCD     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12 */                 
    DCD     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13 */                
    DCD     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
    DCD     TIM8_CC_IRQHandler                /* TIM8 Capture Compare */                                   
    DCD     DMA1_Stream7_IRQHandler           /* DMA1 Stream7 */                                           
    DCD     FSMC_IRQHandler                   /* FSMC */                                           
    DCD     SDIO_IRQHandler                   /* SDIO */                                           
    DCD     TIM5_IRQHandler                   /* TIM5 */                                           
    DCD     SPI3_IRQHandler                   /* SPI3 */                                          
    DCD     UART4_IRQHandler                  /* UART4 */                                          
    DCD     UART5_IRQHandler                  /* UART5 */                                          
    DCD     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */                  
    DCD     TIM7_IRQHandler                   /* TIM7 */                  
    DCD     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0 */                                  
    DCD     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1 */                                  
    DCD     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2 */                                  
    DCD     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3 */                                  
    DCD     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4 */                                  
    DCD     ETH_IRQHandler                    /* Ethernet */                                       
    DCD     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */                      
    DCD     CAN2_TX_IRQHandler                /* CAN2 TX */                                               
    DCD     CAN2_RX0_IRQHandler               /* CAN2 RX0 */                                              
    DCD     CAN2_RX1_IRQHandler               /* CAN2 RX1 */                                              
    DCD     CAN2_SCE_IRQHandler               /* CAN2 SCE */                                              
    DCD     OTG_FS_IRQHandler                 /* USB OTG FS */                                     
    DCD     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5 */                                  
    DCD     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6 */                                  
    DCD     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7 */                                  
    DCD     USART6_IRQHandler                 /* USART6 */                                          
    DCD     I2C3_EV_IRQHandler                /* I2C3 event */                                            
    DCD     I2C3_ER_IRQHandler                /* I2C3 error */                                            
    DCD     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out */                     
    DCD     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In */                      
    DCD     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */                       
    DCD     OTG_HS_IRQHandler                 /* USB OTG HS */                                     
    DCD     DCMI_IRQHandler                   /* DCMI */                                     
    DCD     0                                 /* Reserved */                                     
    DCD     HASH_RNG_IRQHandler               /* Hash and RNG */
    DCD     FPU_IRQHandler                    /* FPU */


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

    

	PUBWEAK WWDG_IRQHandler
WWDG_IRQHandler:
    b        .
       
	PUBWEAK PVD_IRQHandler
PVD_IRQHandler:
    b        .
                
	PUBWEAK TAMP_STAMP_IRQHandler
TAMP_STAMP_IRQHandler:
    b        .
         
	PUBWEAK RTC_WKUP_IRQHandler
RTC_WKUP_IRQHandler:
    b        .
           
	PUBWEAK FLASH_IRQHandler
FLASH_IRQHandler:
    b        .
              
	PUBWEAK RCC_IRQHandler
RCC_IRQHandler:
    b        .
                
	PUBWEAK EXTI0_IRQHandler
EXTI0_IRQHandler:
    b        .
              
	PUBWEAK EXTI1_IRQHandler
EXTI1_IRQHandler:
    b        .
              
	PUBWEAK EXTI2_IRQHandler
EXTI2_IRQHandler:
    b        .
              
	PUBWEAK EXTI3_IRQHandler
EXTI3_IRQHandler:
    b        .
              
	PUBWEAK EXTI4_IRQHandler
EXTI4_IRQHandler:
    b        .
              
	PUBWEAK DMA1_Stream0_IRQHandler
DMA1_Stream0_IRQHandler:
    b        .
       
	PUBWEAK DMA1_Stream1_IRQHandler
DMA1_Stream1_IRQHandler:
    b        .
       
	PUBWEAK DMA1_Stream2_IRQHandler
DMA1_Stream2_IRQHandler:
    b        .
       
	PUBWEAK DMA1_Stream3_IRQHandler
DMA1_Stream3_IRQHandler:
    b        .
       
	PUBWEAK DMA1_Stream4_IRQHandler
DMA1_Stream4_IRQHandler:
    b        .
       
	PUBWEAK DMA1_Stream5_IRQHandler
DMA1_Stream5_IRQHandler:
    b        .
       
	PUBWEAK DMA1_Stream6_IRQHandler
DMA1_Stream6_IRQHandler:
    b        .
       
	PUBWEAK ADC_IRQHandler
ADC_IRQHandler:
    b        .
                
	PUBWEAK CAN1_TX_IRQHandler
CAN1_TX_IRQHandler:
    b        .
            
	PUBWEAK CAN1_RX0_IRQHandler
CAN1_RX0_IRQHandler:
    b        .
           
	PUBWEAK CAN1_RX1_IRQHandler
CAN1_RX1_IRQHandler:
    b        .
           
	PUBWEAK CAN1_SCE_IRQHandler
CAN1_SCE_IRQHandler:
    b        .
           
	PUBWEAK EXTI9_5_IRQHandler
EXTI9_5_IRQHandler:
    b        .
            
	PUBWEAK TIM1_BRK_TIM9_IRQHandler
TIM1_BRK_TIM9_IRQHandler:
    b        .
      
	PUBWEAK TIM1_UP_TIM10_IRQHandler
TIM1_UP_TIM10_IRQHandler:
    b        .
      
	PUBWEAK TIM1_TRG_COM_TIM11_IRQHandler
TIM1_TRG_COM_TIM11_IRQHandler:
    b        .
 
	PUBWEAK TIM1_CC_IRQHandler
TIM1_CC_IRQHandler:
    b        .
            
	PUBWEAK TIM2_IRQHandler
TIM2_IRQHandler:
    b        .
 		  		
	PUBWEAK TIM3_IRQHandler
TIM3_IRQHandler:
    b        .
               
	PUBWEAK TIM4_IRQHandler
TIM4_IRQHandler:
    b        .
               
	PUBWEAK I2C1_EV_IRQHandler
I2C1_EV_IRQHandler:
    b        .
            
	PUBWEAK I2C1_ER_IRQHandler
I2C1_ER_IRQHandler:
    b        .
            
	PUBWEAK I2C2_EV_IRQHandler
I2C2_EV_IRQHandler:
    b        .
            
	PUBWEAK I2C2_ER_IRQHandler
I2C2_ER_IRQHandler:
    b        .
            
	PUBWEAK SPI1_IRQHandler
SPI1_IRQHandler:
    b        .
               
	PUBWEAK SPI2_IRQHandler
SPI2_IRQHandler:
    b        .
               
	PUBWEAK USART1_IRQHandler
USART1_IRQHandler:
    b        .
             
	PUBWEAK USART2_IRQHandler
USART2_IRQHandler:
    b        .
             
	PUBWEAK USART3_IRQHandler
USART3_IRQHandler:
    b        .
           	
	PUBWEAK EXTI15_10_IRQHandler
EXTI15_10_IRQHandler:
    b        .
          
	PUBWEAK RTC_Alarm_IRQHandler
RTC_Alarm_IRQHandler:
    b        .
          
	PUBWEAK OTG_FS_WKUP_IRQHandler
OTG_FS_WKUP_IRQHandler:
    b        .
        
	PUBWEAK TIM8_BRK_TIM12_IRQHandler
TIM8_BRK_TIM12_IRQHandler:
    b        .
     
	PUBWEAK TIM8_UP_TIM13_IRQHandler
TIM8_UP_TIM13_IRQHandler:
    b        .
      
	PUBWEAK TIM8_TRG_COM_TIM14_IRQHandler
TIM8_TRG_COM_TIM14_IRQHandler:
    b        .
 
	PUBWEAK TIM8_CC_IRQHandler
TIM8_CC_IRQHandler:
    b        .
            
	PUBWEAK DMA1_Stream7_IRQHandler
DMA1_Stream7_IRQHandler:
    b        .
       
	PUBWEAK FSMC_IRQHandler
FSMC_IRQHandler:
    b        .
               
	PUBWEAK SDIO_IRQHandler
SDIO_IRQHandler:
    b        .
               
	PUBWEAK TIM5_IRQHandler
TIM5_IRQHandler:
    b        .
               
	PUBWEAK SPI3_IRQHandler
SPI3_IRQHandler:
    b        .
               
	PUBWEAK UART4_IRQHandler
UART4_IRQHandler:
    b        .
              
	PUBWEAK UART5_IRQHandler
UART5_IRQHandler:
    b        .
              
	PUBWEAK TIM6_DAC_IRQHandler
TIM6_DAC_IRQHandler:
    b        .
           
	PUBWEAK TIM7_IRQHandler
TIM7_IRQHandler:
    b        .
               
	PUBWEAK DMA2_Stream0_IRQHandler
DMA2_Stream0_IRQHandler:
    b        .
       
	PUBWEAK DMA2_Stream1_IRQHandler
DMA2_Stream1_IRQHandler:
    b        .
       
	PUBWEAK DMA2_Stream2_IRQHandler
DMA2_Stream2_IRQHandler:
    b        .
       
	PUBWEAK DMA2_Stream3_IRQHandler
DMA2_Stream3_IRQHandler:
    b        .
       
	PUBWEAK DMA2_Stream4_IRQHandler
DMA2_Stream4_IRQHandler:
    b        .
       
	PUBWEAK ETH_IRQHandler
ETH_IRQHandler:
    b        .
                
	PUBWEAK ETH_WKUP_IRQHandler
ETH_WKUP_IRQHandler:
    b        .
           
	PUBWEAK CAN2_TX_IRQHandler
CAN2_TX_IRQHandler:
    b        .
            
	PUBWEAK CAN2_RX0_IRQHandler
CAN2_RX0_IRQHandler:
    b        .
           
	PUBWEAK CAN2_RX1_IRQHandler
CAN2_RX1_IRQHandler:
    b        .
           
	PUBWEAK CAN2_SCE_IRQHandler
CAN2_SCE_IRQHandler:
    b        .
           
	PUBWEAK OTG_FS_IRQHandler
OTG_FS_IRQHandler:
    b        .
             
	PUBWEAK DMA2_Stream5_IRQHandler
DMA2_Stream5_IRQHandler:
    b        .
       
	PUBWEAK DMA2_Stream6_IRQHandler
DMA2_Stream6_IRQHandler:
    b        .
       
	PUBWEAK DMA2_Stream7_IRQHandler
DMA2_Stream7_IRQHandler:
    b        .
       
	PUBWEAK USART6_IRQHandler
USART6_IRQHandler:
    b        .
             
	PUBWEAK I2C3_EV_IRQHandler
I2C3_EV_IRQHandler:
    b        .
            
	PUBWEAK I2C3_ER_IRQHandler
I2C3_ER_IRQHandler:
    b        .
            
	PUBWEAK OTG_HS_EP1_OUT_IRQHandler
OTG_HS_EP1_OUT_IRQHandler:
    b        .
     
	PUBWEAK OTG_HS_EP1_IN_IRQHandler
OTG_HS_EP1_IN_IRQHandler:
    b        .
      
	PUBWEAK OTG_HS_WKUP_IRQHandler
OTG_HS_WKUP_IRQHandler:
    b        .
        
	PUBWEAK OTG_HS_IRQHandler
OTG_HS_IRQHandler:
    b        .
             
	PUBWEAK DCMI_IRQHandler
DCMI_IRQHandler:
    b        .
                                     
	PUBWEAK HASH_RNG_IRQHandler
HASH_RNG_IRQHandler:
    b        .
           
	PUBWEAK FPU_IRQHandler
FPU_IRQHandler:
    b        .
                
    
    
    END
