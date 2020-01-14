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


#include "sama5d3x.h"
#include "aic.h"


/** \brief Enable the write protection on AIC registers */
static void AIC_EnableWriteProtection();

/** \brief Disable the write protection on AIC registers */
static void AIC_DisableWriteProtection();




/** \brief Initialize the interrupt controller */
void AIC_Init()
{
   /* Set PROT bit to remove debugger side effects */
   AIC->AIC_DCR |= (1 << 0);

   /* Enable write protection */
   AIC_EnableWriteProtection();
}

/** \brief Install an interrupt handler */
void AIC_HandlerInstall(const uint32_t source, const fp_interrupt_handler_t handler, const interrupt_priority_t priority)
{
    /* Disable interrupt */
    AIC_DisableInterrupt(source);
    
    /* Disable write protection */
    AIC_DisableWriteProtection();
   
    /* Install handler */
    AIC->AIC_SVR = (uint32_t)handler;
    
    /* Configure priority */
    AIC->AIC_SMR &= ~(0x07 << 0);
    AIC->AIC_SMR |= ((priority & 0x07) << 0);
    
    /* Clear interrupt */
    AIC_ClearInterrupt(source);

    /* Enable write protection */
    AIC_EnableWriteProtection();
}

/** \brief ENable an interrupt */
void AIC_EnableInterrupt(const uint32_t source)
{
    /* Select interrupt */
    AIC->AIC_SSR = source;

    /* Disable interrupt */
    AIC->AIC_IECR = (1 << 0);
}

/** \brief Disable an interrupt */
void AIC_DisableInterrupt(const uint32_t source)
{
    /* Select interrupt */
    AIC->AIC_SSR = source;
    
    /* Disable interrupt */
    AIC->AIC_IDCR = (1 << 0);
}

/** \brief Clear an interrupt */
void AIC_ClearInterrupt(const uint32_t source)
{
    /* Select interrupt */
    AIC->AIC_SSR = source;

    /* Clear interrupt */
    AIC->AIC_ICCR = (1 << 0);
}


/** \brief Enable the write protection on AIC registers */
static void AIC_EnableWriteProtection()
{
    AIC->AIC_WPMR = (0x414943 << 8) | (1 << 0);
}

/** \brief Disable the write protection on AIC registers */
static void AIC_DisableWriteProtection()
{
    AIC->AIC_WPMR = (0x414943 << 8);
}
