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
#include "nano_os_tools.h"


/** \brief UART Rx buffer size in bytes */
#define UART_RX_BUFFER_SIZE    64u

/** \brief UART Tx buffer size in bytes */
#define UART_TX_BUFFER_SIZE    32u

/** \brief Ring buffer */
typedef struct _ring_buffer_t
{
    /** \brief Buffer */
    uint8_t* buffer;
    /** \brief End of buffer */
    uint8_t* end_of_buffer;
    /** \brief Read pointer */
    uint8_t* read_ptr;
    /** \brief Write pointer */
    uint8_t* write_ptr;
} ring_buffer_t;

/** \brief Receive buffer */
static uint8_t rx_buffer[UART_RX_BUFFER_SIZE];
static ring_buffer_t rx_ring_buffer;

/** \brief Transmit buffer */
static uint8_t tx_buffer[UART_TX_BUFFER_SIZE];
static ring_buffer_t tx_ring_buffer;


/** \brief Rx ready flag mask */
#define UART_RX_READY_FLAG_MASK     1u

/** \brief Tx ready flag mask */
#define UART_TX_READY_FLAG_MASK     2u

/** \brief Flag set to synchronize Rx and Tx with IRQ */
static nano_os_flag_set_t uart_flag_set;



/** \brief UART interrupt handler */
static void DBGU_UART_Handler(void);

/** \brief Initialize a ring buffer */
static void DBGU_UART_RingBufferInit(ring_buffer_t* const ring_buffer, uint8_t* const buffer, const size_t buffer_size);

/** \brief Read a byte from a ring buffer */
static bool DBGU_UART_RingBufferReadByte(ring_buffer_t* const ring_buffer, uint8_t* const byte);

/** \brief Write a byte to a ring buffer */
static bool DBGU_UART_RingBufferWriteByte(ring_buffer_t* const ring_buffer, const uint8_t byte);

/** \brief Indicate if a ring buffer is empty */
static bool DBGU_UART_RingBufferIsEmpty(const ring_buffer_t* const ring_buffer);

/** \brief Indicate if a ring buffer is full */
static bool DBGU_UART_RingBufferIsFull(const ring_buffer_t* const ring_buffer);




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

    /* Create Rx and Tx flag set */
    DBGU_UART_RingBufferInit(&rx_ring_buffer, rx_buffer, sizeof(rx_buffer));
    DBGU_UART_RingBufferInit(&tx_ring_buffer, tx_buffer, sizeof(tx_buffer));
    (void)NANO_OS_FLAG_SET_Create(&uart_flag_set, 0u, QT_PRIORITY);


    /* Enable Rx and Tx */
    DBGU->DBGU_CR = (1 << 4) | (1 << 6);
}


/** \brief Send data over the UART */
void DBG_UART_Send(const uint8_t* data, uint32_t data_len)
{
    bool data_added;

    do
    {
        /* Disable Rx and Tx interrupts */
        DBGU->DBGU_IDR = 0xFFFFFFFF;

        /* Add data to the transmit buffer */
        do
        {
            data_added = DBGU_UART_RingBufferWriteByte(&tx_ring_buffer, *data);
            if (data_added)
            {
                data++;
                data_len--;
            }
        }
        while (data_added && (data_len != 0));
        if (!data_added)
        {
            NANO_OS_FLAG_SET_Clear(&uart_flag_set, UART_TX_READY_FLAG_MASK);
        }

        /* Enable Rx and Tx interrupts */
        DBGU->DBGU_IER = (1 << 0) | (1 << 1);

        /* Wait for Tx ready if more data has to be sent */
        if (data_len != 0)
        {
            uint32_t wake_up_flags = 0u;
            do
            {
                (void)NANO_OS_FLAG_SET_Wait(&uart_flag_set, UART_TX_READY_FLAG_MASK, true, false, &wake_up_flags, 0xFFFFFFFFu);
            }
            while (wake_up_flags == 0u);
        }
    }
    while (data_len != 0);

    return;
}


/** \brief Receive data on the UART */
void DBG_UART_Receive(uint8_t* data, uint32_t data_len)
{
    bool data_read;

    while (data_len != 0)
    {
        /* Save interrupt mask */
        uint32_t int_mask = DBGU->DBGU_IMR;

        /* Disable Rx and Tx interrupts */
        DBGU->DBGU_IDR = 0xFFFFFFFF;

        /* Get Rx data */
        do
        {
            data_read = DBGU_UART_RingBufferReadByte(&rx_ring_buffer, data);
            if (data_read)
            {
                data++;
                data_len--;
            }
        }
        while (data_read && (data_len != 0));
        if (!data_read)
        {
            NANO_OS_FLAG_SET_Clear(&uart_flag_set, UART_RX_READY_FLAG_MASK);
        }

        /* Restore interrupt mask */
        DBGU->DBGU_IER = int_mask;

        /* Wait for Rx ready if more data has to be received */
        if (data_len != 0)
        {
            uint32_t wake_up_flags = 0u;
            do
            {
                (void)NANO_OS_FLAG_SET_Wait(&uart_flag_set, UART_RX_READY_FLAG_MASK, true, false, &wake_up_flags, 0xFFFFFFFFu);
            }
            while (wake_up_flags == 0u);
        }
    }
}

/** \brief UART interrupt handler */
static void DBGU_UART_Handler(void)
{
    bool signal;
    uint8_t data;

    /* Read received data */
    signal = DBGU_UART_RingBufferIsEmpty(&rx_ring_buffer);
    while( (DBGU->DBGU_SR & ( 1 << 0))!=0 ) /* RXRDY bit */
    {
        /* Read the received byte */
        data = DBGU->DBGU_RHR;

        /* Put the data into the received buffer */
        (void)DBGU_UART_RingBufferWriteByte(&rx_ring_buffer, data);
    }
    /* Signal received data */
    if (signal && !DBGU_UART_RingBufferIsEmpty(&rx_ring_buffer))
    {
        (void)NANO_OS_FLAG_SET_SetFromIsr(&uart_flag_set, UART_RX_READY_FLAG_MASK);
    }

    /* Transmit data */
    if( (DBGU->DBGU_SR & (1 << 1))!=0 ) /* TXRDY bit */
    {
		if (!DBGU_UART_RingBufferIsEmpty(&tx_ring_buffer))
		{
		    /* Read byte */
		    signal = DBGU_UART_RingBufferIsFull(&tx_ring_buffer);
		    (void)DBGU_UART_RingBufferReadByte(&tx_ring_buffer, &data);

            /* Send byte */
            DBGU->DBGU_THR = data;

			/* Check end of transmit */
			if (DBGU_UART_RingBufferIsEmpty(&tx_ring_buffer))
			{
				/* Disable TXRDY interrupt */
                DBGU->DBGU_IDR = (1 << 1);
			}

			/* Signal tx ready */
			if (signal)
			{
			    (void)NANO_OS_FLAG_SET_SetFromIsr(&uart_flag_set, UART_TX_READY_FLAG_MASK);
			}
		}
		else
		{
		    /* Disable TXRDY interrupt */
            DBGU->DBGU_IDR = (1 << 1);
		}
    }
}




/** \brief Initialize a ring buffer */
static void DBGU_UART_RingBufferInit(ring_buffer_t* const ring_buffer, uint8_t* const buffer, const size_t buffer_size)
{
    ring_buffer->buffer = buffer;
    ring_buffer->end_of_buffer = (buffer + buffer_size);
    ring_buffer->read_ptr = buffer;
    ring_buffer->write_ptr = buffer;
}

/** \brief Read a byte from a ring buffer */
static bool DBGU_UART_RingBufferReadByte(ring_buffer_t* const ring_buffer, uint8_t* const byte)
{
    bool ret = false;

    if (ring_buffer->read_ptr != ring_buffer->write_ptr)
    {
        (*byte) = (*ring_buffer->read_ptr);
        ring_buffer->read_ptr++;
        if (ring_buffer->read_ptr == ring_buffer->end_of_buffer)
        {
            ring_buffer->read_ptr = ring_buffer->buffer;
        }
        ret = true;
    }

    return ret;
}

/** \brief Write a byte to a ring buffer */
static bool DBGU_UART_RingBufferWriteByte(ring_buffer_t* const ring_buffer, const uint8_t byte)
{
    bool ret = false;

    uint8_t* write_ptr = ring_buffer->write_ptr;
    write_ptr++;
    if (write_ptr == ring_buffer->end_of_buffer)
    {
        write_ptr = ring_buffer->buffer;
    }
    if (write_ptr != ring_buffer->read_ptr)
    {
        (*ring_buffer->write_ptr) = byte;
        ring_buffer->write_ptr = write_ptr;
        ret = true;
    }

    return ret;
}

/** \brief Indicate if a ring buffer is empty */
static bool DBGU_UART_RingBufferIsEmpty(const ring_buffer_t* const ring_buffer)
{
    return (ring_buffer->read_ptr == ring_buffer->write_ptr);
}

/** \brief Indicate if a ring buffer is full */
static bool DBGU_UART_RingBufferIsFull(const ring_buffer_t* const ring_buffer)
{
    uint8_t* const write_ptr = ring_buffer->write_ptr + 1u;
    return ((write_ptr == ring_buffer->read_ptr) ||
            ((write_ptr == ring_buffer->end_of_buffer) && (ring_buffer->read_ptr == ring_buffer->buffer)));
}
