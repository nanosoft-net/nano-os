
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
#include "dbg_uart.h"
#include "aic.h"
#include "pmc.h"
#include "pio.h"
#include "nano_os_api.h"


/** \brief UART Rx buffer size in bytes */
#define UART_RX_BUFFER_SIZE    64u

/** \brief UART Tx buffer size in bytes */
#define UART_TX_BUFFER_SIZE    32u



/** \brief Receive buffer */
static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static uint8_t* rx_buffer_read_ptr;
static uint8_t* rx_buffer_write_ptr;
static volatile uint32_t rx_buffer_len;
static nano_os_semaphore_t rx_sem;

/** \brief Transmit buffer */
static uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint32_t tx_buffer_count;
static volatile uint32_t tx_buffer_len;
static nano_os_semaphore_t tx_sem;


/** \brief UART interrupt handler */
static void DBGU_UART_Handler(void);




/** \brief Initialize the UART driver */
void DBG_UART_Init(void)
{
    /* Enable DBGU clock */
    PMC_EnablePeripheral(ID_DBGU);

    /* Configure PB30 (DRXD) and PB31(RTXD) to be used by the DBGU */
    PIO_Configure(PIO_B, 30, PIOM_PERIPH_A, PIOPM_NONE);
    PIO_Configure(PIO_B, 31, PIOM_PERIPH_A, PIOPM_NONE);

    /* Disable DBGU */
    DBGU->DBGU_CR = (1 << 2) | (1 << 3) | (1 << 8);

    /* Configure baudrate => 115200-8-N-1 */
    DBGU->DBGU_MR = (0b100 << 9) | (0b00 << 14);
    DBGU->DBGU_BRGR = 72;

    /* Enable Rx interrupt only */
    DBGU->DBGU_IDR = 0xFFFFFFFF;
    DBGU->DBGU_IER = (1 << 1);
    AIC_HandlerInstall(DBGU_IRQn, DBGU_UART_Handler, INTP_LOWEST);
    AIC_EnableInterrupt(DBGU_IRQn);

    /* Create Rx and Tx semaphores */
    rx_buffer_read_ptr = &rx_buffer[0u];
    rx_buffer_write_ptr = &rx_buffer[0u];
    (void)NANO_OS_SEMAPHORE_Create(&rx_sem, 0u, 1u, QT_PRIORITY);
    (void)NANO_OS_SEMAPHORE_Create(&tx_sem, 1u, 1u, QT_PRIORITY);

    /* Enable Rx and Tx */
    DBGU->DBGU_CR = (1 << 4) | (1 << 6);
}


/** \brief Send data over the UART */
void DBG_UART_Send(const uint8_t* data, uint32_t data_len)
{

    while (data_len != 0)
    {
        uint32_t tx_len = data_len;

        /* Wait end of transmit */
        (void)NANO_OS_SEMAPHORE_Wait(&tx_sem, 0xFFFFFFFFu);

        /* Disable Rx and Tx interrupts */
        DBGU->DBGU_IDR = 0xFFFFFFFF;

        /* Add transmit data to the stream */
        if (tx_len > UART_TX_BUFFER_SIZE)
        {
            tx_len = UART_TX_BUFFER_SIZE;
        }
        tx_buffer_len = tx_len;
        tx_buffer_count = 0u;
        MEMCPY(tx_buffer, data, tx_buffer_len);

        /* Enable Rx and Tx interrupts */
        DBGU->DBGU_IER = (1 << 0) | (1 << 1);

        /* Next data */
        data += tx_len;
        data_len -= tx_len;
    }

    return;
}

/** \brief Receive data on the UART */
void DBG_UART_Receive(uint8_t* data, uint32_t data_len)
{
    while (data_len != 0)
    {
        if (rx_buffer_len == 0u)
        {
            /* Wait for received data */
            (void)NANO_OS_SEMAPHORE_Wait(&rx_sem, 0xFFFFFFFFu);
        }

        /* Save interrupt mask */
        uint32_t int_mask = DBGU->DBGU_IMR;

        /* Disable Rx and Tx interrupts */
        DBGU->DBGU_IDR = 0xFFFFFFFF;

        /* Get rx data */
        if (rx_buffer_len != 0u)
        {
            (*data) = (*rx_buffer_read_ptr);

            data++;
            rx_buffer_read_ptr++;
            if (rx_buffer_read_ptr == &rx_buffer[sizeof(rx_buffer)])
            {
                rx_buffer_read_ptr = &rx_buffer[0u];
            }

            data_len--;
            rx_buffer_len--;
        }

        /* Restore interrupt mask */
        DBGU->DBGU_IER = int_mask;
    }
}


/** \brief UART interrupt handler */
static void DBGU_UART_Handler(void)
{
    uint8_t data;

    /* Read received data */
    while( (DBGU->DBGU_SR & ( 1 << 0))!=0 ) /* RXRDY bit */
    {
        /* Read the received byte */
        data = DBGU->DBGU_RHR;

        /* Put the data into the received buffer */
        if (rx_buffer_len != sizeof(rx_buffer))
        {
            (*rx_buffer_write_ptr) = data;

            rx_buffer_len++;
            rx_buffer_write_ptr++;
            if (rx_buffer_write_ptr == &rx_buffer[sizeof(rx_buffer)])
            {
                rx_buffer_write_ptr = &rx_buffer[0u];
            }
        }

        /* Signal received data */
        NANO_OS_SEMAPHORE_PostFromIsr(&rx_sem);
    }

    /* Transmit data */
    if( (DBGU->DBGU_SR & (1 << 1))!=0 ) /* TXRDY bit */
    {
        uint32_t buffer_len = tx_buffer_len;
        if( buffer_len!=0 )
        {
            /* Get next byte to transmit from the stream */
            data = tx_buffer[tx_buffer_count];

            /* Send byte */
            DBGU->DBGU_THR = data;

            /* Check end of transmit */
            tx_buffer_count++;
            if( tx_buffer_count==buffer_len )
            {
                /* Disable TXRDY interrupt */
                DBGU->DBGU_IDR = (1 << 1);

                /* Signal end of transmit */
                (void)NANO_OS_SEMAPHORE_PostFromIsr(&tx_sem);
                tx_buffer_len = 0;
            }
        }
        else
        {
            /* Disable TXRDY interrupt */
            DBGU->DBGU_IDR = (1 << 1);
        }
    }
}

