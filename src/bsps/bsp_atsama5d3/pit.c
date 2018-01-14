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
#include "pit.h"
#include "pmc.h"
#include "aic.h"


static fp_nano_os_system_timer_callback_func_t s_system_timer_irq_callback;
static void PIT_Handler(void);


/** \brief Initialize the PIT timer */
void PIT_Init(const fp_nano_os_system_timer_callback_func_t system_timer_irq_callback)
{
   /* Enable PIT clock */
   PMC_EnablePeripheral(ID_PIT);
   
   /* Disable PIT */
   PIT->PIT_MR = 0;
   
   /* Save callback */
   s_system_timer_irq_callback = system_timer_irq_callback;

   /* Configure period to 10ms(100Hz) : fPIT = MCK / 16 = 8250kHz
      => PIV = 82500 */
   PIT->PIT_MR &= ~(0x000FFFFF);
   PIT->PIT_MR = 82500;

   /* Install interrupt handler */
   AIC_HandlerInstall(PIT_IRQn, PIT_Handler, INTP_LOWEST);
   //PIT_Handler();

   /* Enable interrupt */
   PIT->PIT_MR |= (0x01 << 25);
   AIC_EnableInterrupt(PIT_IRQn);
}

/** \brief Start the PIT timer */
void PIT_Start()
{
    /* Enable PIT */
    PIT->PIT_MR |= (0x01 << 24);
}


/** \brief Interrupt handle for the PIT timer */
static void PIT_Handler(void)
{
    uint32_t unused_but_necessary;

    /* Acknowledge interrupt */
    unused_but_necessary = PIT->PIT_PIVR;
    NANO_OS_UNUSED(unused_but_necessary);

    /* Nano OS system timer callback */
    s_system_timer_irq_callback();
}
