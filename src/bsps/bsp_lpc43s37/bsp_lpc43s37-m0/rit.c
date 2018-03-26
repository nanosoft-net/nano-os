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

#include "rit.h"
#include "chip_lpc43xx.h"
#include "nano_os_api.h"
#include "nano_os_port_user.h"

/** \brief Nano OS callback */
static fp_nano_os_system_timer_callback_func_t s_system_timer_irq_callback;


/** \brief Initialize RIT */
void RIT_Init(const fp_nano_os_system_timer_callback_func_t system_timer_irq_callback)
{
    const uint32_t systick_input_freq_hz = NANO_OS_PORT_USER_GetSystickInputClockFreq();

    /* Turn on power and clock for the RIT => RITCLK = BASE_M4_CLOCK */
    LPC_CCU1->CLKCCU[CLK_MX_RITIMER].CFG = (1u << 0u);
    LPC_RGU->RESET_EXT_STAT[RGU_RITIMER_RST] = 0u;

    /* Configure timer:
     * - clear on match (RITENCLR bit)
     * - stop on debug (RITENBR bit)
     * - frequency = NANO_OS_TICK_RATE_HZ
     */
    LPC_RITIMER->CTRL = (1u << 1u) | (1u << 2u);
    LPC_RITIMER->COMPVAL = (systick_input_freq_hz / NANO_OS_TICK_RATE_HZ) - 1u;

    /* Save callback */
    s_system_timer_irq_callback = system_timer_irq_callback;

    /* Enable interrupt */
    NVIC_EnableIRQ(RITIMER_IRQn);

    return;
}

/** \brief Start RIT */
void RIT_Start(void)
{
    LPC_RITIMER->COUNTER = 0u;
    LPC_RITIMER->CTRL |= (1u << 3u); /* RITEN bit */
}

/** \brief Get the timestamp in µs */
uint32_t RIT_GetTimestampInUs(void)
{
    uint32_t us_timestamp;
    uint32_t counter_value;
    uint32_t tick_count;
    uint32_t tick_count2;
    const uint32_t load_reg = LPC_RITIMER->COMPVAL;

    /* Read the current tick count and timer value */
    do
    {
        NANO_OS_GetTickCount(&tick_count);
        counter_value = LPC_RITIMER->COUNTER;
        NANO_OS_GetTickCount(&tick_count2);
    }
    /* If an interrupt occurred between reading the timer counter and
       reading the tick count, we do another reading.
     */
    while (tick_count != tick_count2);

    /* Compute the timestamp in µs */
    us_timestamp = (counter_value * (1000000u / NANO_OS_TICK_RATE_HZ)) / load_reg;
    us_timestamp += (NANO_OS_TICKS_TO_MS(tick_count) * 1000u);

    return us_timestamp;
}


/** \brief Interrupt handler */
void RIT_Handler(void)
{
    /* Signal OS that we are in an interrupt handler */
    NANO_OS_INTERRUPT_Enter();

    /* Call Nano OS callback */
    s_system_timer_irq_callback();

    /* Acknowledge interrupt (RITINT bit) */
    LPC_RITIMER->CTRL |= (1u << 0u);

    /* Signal OS that we exit the interrupt handler */
    NANO_OS_INTERRUPT_Exit();
}

