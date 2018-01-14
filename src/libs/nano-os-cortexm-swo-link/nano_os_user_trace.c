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

#include "nano_os_cortexm_swo_link_cfg.h"



#include "nano_os_user.h"
#include "nano_os_port.h"


/** \brief Address of the Debug Exception and Monitor Control Register */
#define DEBUG_DEMCR_REG         (* NANO_OS_CAST(volatile uint32_t*, 0xe000edfc))

/** \brief Address of the Asynchronous Clock Prescaler Register */
#define TPIU_ACPR_REG           (* NANO_OS_CAST(volatile uint32_t*, 0xe0040010))

/** \brief Address of the Selected Pin Protocol Register */
#define TPIU_SPPR_REG           (* NANO_OS_CAST(volatile uint32_t*, 0xe00400f0))

/** \brief Address of the Stimulus Port Registers */
#define ITM_STIM_REG            (NANO_OS_CAST(volatile uint32_t*, 0xe0000000))

/** \brief Address of the Trace Enable Registers */
#define ITM_TER_REG             (NANO_OS_CAST(volatile uint32_t*, 0xe0000e00))

/** \brief Address of the Trace Privilege Register */
#define ITM_TPR_REG             (* NANO_OS_CAST(volatile uint32_t*, 0xe0000e40))

/** \brief Address of the Trace Control Register */
#define ITM_TCR_REG             (* NANO_OS_CAST(volatile uint32_t*, 0xe0000e80))

/** \brief Address of the Control Register */
#define DWT_CTRL_REG            (* NANO_OS_CAST(volatile uint32_t*, 0xe0001000))




/** \brief Initialize the user module which will send/receive the trace packets */
nano_os_error_t NANO_OS_USER_TraceInit(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* Enable trace in code debug */
    DEBUG_DEMCR_REG = (1u << 24u);

    /* Configure SWO signal : baudrate, UART protocol (NRZ) */
    TPIU_ACPR_REG = (NANO_OS_CORTEXM_SWO_LINK_CPU_FREQUENCY / NANO_OS_CORTEXM_SWO_LINK_FREQUENCY) - 1u;
    TPIU_SPPR_REG = (2u << 0u);

    /* Enable access to protected registers */
    *((volatile uint32_t *)(0xe0000000 + 0x00FB0)) = 0xC5ACCE55; /* ITM Lock Access Register, C5ACCE55 enables more write access to Control Register 0xE00 :: 0xFFC */

    /* Configure and enable trace module : TraceBusID, SYNCENA, ITMENA*/
    ITM_TCR_REG = (0x7Fu << 16u) | (1u << 2u) | (1u << 0u);

    /* Allow sending trace data in priviledged mode only */
    ITM_TPR_REG = 0xFFFFFFFFu;

    /* Enable selected trace channel */
    ITM_TER_REG[NANO_OS_CORTEXM_SWO_LINK_ITM_CHANNEL / 32u] |= (1 << (NANO_OS_CORTEXM_SWO_LINK_ITM_CHANNEL % 32u));

    DWT_CTRL_REG = 0x400003FE; /* DWT_CTRL */
    *((volatile uint32_t *)(0xe0000000 + 0x40304)) = 0x00000100; /* Formatter and Flush Control Register */



    return ret;
}


/** \brief Send a real time trace packet */
void NANO_OS_USER_TraceSendRttPacket(const void* const rtt_packet, const size_t packet_size)
{
    /* Disable interrupts to protect against trace data sent from interrupt handlers */
    nano_os_int_status_reg_t int_status_reg;
    NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

    /* Send trace packet */
    if ( ((NANO_OS_CAST(uint32_t, rtt_packet) & 0x03) == 0u) &&
         ((packet_size & 0x03) == 0u) )
    {
        /* Write 4 bytes at a time */
        size_t i;
        const uint32_t* rtt_packet_data = NANO_OS_CAST(const uint32_t*, rtt_packet);
        for(i = 0; i < packet_size; i += sizeof(uint32_t))
        {
            /* Wait for space in transmit FIFO */
            while ((ITM_STIM_REG[NANO_OS_CORTEXM_SWO_LINK_ITM_CHANNEL] & 0x01u) == 0u)
            {}

            /* Send data */
            ITM_STIM_REG[NANO_OS_CORTEXM_SWO_LINK_ITM_CHANNEL] = (*rtt_packet_data);
            rtt_packet_data++;
        }
    }
    else
    {
        /* Write 8 bytes at a time */
        size_t i;
        const uint8_t* rtt_packet_data = NANO_OS_CAST(const uint8_t*, rtt_packet);
        for(i = 0; i < packet_size; i++)
        {
            /* Wait for space in transmit FIFO */
            while ((ITM_STIM_REG[NANO_OS_CORTEXM_SWO_LINK_ITM_CHANNEL] & 0x01u) == 0u)
            {}

            /* Send data */
            *(NANO_OS_CAST(volatile uint8_t*, &(ITM_STIM_REG[NANO_OS_CORTEXM_SWO_LINK_ITM_CHANNEL]))) = (*rtt_packet_data);
            rtt_packet_data++;
        }
    }

    /* Restore interrupts */
    NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);
}

