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

#include "uart.h"

#include "nano_os_api.h"
#include "nano_os_tools.h"

#include "stm32f767xx.h"




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



/** \brief Initialize a ring buffer */
static void UART_RingBufferInit(ring_buffer_t* const ring_buffer, uint8_t* const buffer, const size_t buffer_size);

/** \brief Read a byte from a ring buffer */
static bool UART_RingBufferReadByte(ring_buffer_t* const ring_buffer, uint8_t* const byte);

/** \brief Write a byte to a ring buffer */
static bool UART_RingBufferWriteByte(ring_buffer_t* const ring_buffer, const uint8_t byte);

/** \brief Indicate if a ring buffer is empty */
static bool UART_RingBufferIsEmpty(const ring_buffer_t* const ring_buffer);

/** \brief Indicate if a ring buffer is full */
static bool UART_RingBufferIsFull(const ring_buffer_t* const ring_buffer);




/** \brief Initialize the UART driver */
void UART_Init(void)
{

    /* Enable IOs for Rx and Tx lines (PD9 and PD8, AF7)*/
    GPIOD->AFR[1u] &= ~((0x0Fu << 0u) | (0x0Fu << 4u));
    GPIOD->AFR[1u] |= ((7u << 0u) | (7u << 4u));
    GPIOD->MODER &= ~((3u << 16u) | (3u << 18u));
    GPIOD->MODER |= ((2u << 16u) | (2u << 18u));

    /* Turn on power and clock for the UART => UARTCLK = HSI = 16 MHz */
    RCC->APB1ENR &= ~(1u << 18u);
    RCC->DCKCFGR2 &= ~(3u << 4u);
    RCC->DCKCFGR2 |= (2u << 4u);
    RCC->APB1ENR |= (1u << 18u);

    /* Disable receive and transmit */
    USART3->CR1 = 0x00u;

    /* Configure UART baudrate = UARTCLK/USARTDIV = 16000000 / 139 => 115107bps */
    USART3->BRR = 139u;

    /* Configure transfer : 8bits, 1stop bit, no parity, no flow control */
    USART3->CR2 = 0x00u;
    USART3->CR3 = 0x00u;


    /* Enable Rx interrupt */
    USART3->CR1 |= (1u << 5u);
    NVIC_EnableIRQ(USART3_IRQn);

    /* Create Rx and Tx flag set */
    UART_RingBufferInit(&rx_ring_buffer, rx_buffer, sizeof(rx_buffer));
    UART_RingBufferInit(&tx_ring_buffer, tx_buffer, sizeof(tx_buffer));
    (void)NANO_OS_FLAG_SET_Create(&uart_flag_set, 0u, QT_PRIORITY);

    /* Enable receive and transmit */
    USART3->CR1 |= (1u << 0u) | (1u << 2u) | (1u << 3u);
}


/** \brief Send data over the UART */
void UART_Send(const uint8_t* data, uint32_t data_len)
{
    bool data_added;

    do
    {
        /* Disable Rx and Tx interrupts */
        USART3->CR1 &= ~( (1u << 5u) | (1u << 7u) );

        /* Add data to the transmit buffer */
        do
        {
            data_added = UART_RingBufferWriteByte(&tx_ring_buffer, *data);
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
        USART3->CR1 |= ( (1u << 5u) | (1u << 7u) );

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
void UART_Receive(uint8_t* data, uint32_t data_len)
{
    bool data_read;

    while (data_len != 0)
    {
        /* Save interrupt mask */
        uint32_t int_mask = USART3->CR1;

        /* Disable Rx and Tx interrupts */
        USART3->CR1 &= ~( (1u << 5u) | (1u << 7u) );

        /* Get Rx data */
        do
        {
            data_read = UART_RingBufferReadByte(&rx_ring_buffer, data);
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
        USART3->CR1 = int_mask;

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
void USART3_IRQHandler()
{
    bool signal;
    uint8_t data;

    NANO_OS_INTERRUPT_Enter();

    /* Read received data */
    signal = UART_RingBufferIsEmpty(&rx_ring_buffer);
    while ((USART3->ISR & ( 1u << 5u)) != 0) /* RXNE bit */
    {
        /* Read the received byte */
        data = USART3->RDR;

        /* Put the data into the received buffer */
        (void)UART_RingBufferWriteByte(&rx_ring_buffer, data);
    }
    /* Signal received data */
    if (signal && !UART_RingBufferIsEmpty(&rx_ring_buffer))
    {
        (void)NANO_OS_FLAG_SET_SetFromIsr(&uart_flag_set, UART_RX_READY_FLAG_MASK);
    }

    /* Transmit data */
    if ((USART3->ISR & (1u << 7u)) != 0) /* TXE bit */
    {
		if (!UART_RingBufferIsEmpty(&tx_ring_buffer))
		{
		    /* Read byte */
		    signal = UART_RingBufferIsFull(&tx_ring_buffer);
		    (void)UART_RingBufferReadByte(&tx_ring_buffer, &data);

            /* Send byte */
			USART3->TDR = data;

			/* Check end of transmit */
			if (UART_RingBufferIsEmpty(&tx_ring_buffer))
			{
				/* Disable TXE interrupt */
			    USART3->CR1 &= ~(1u << 7u);
			}

			/* Signal tx ready */
			if (signal)
			{
			    (void)NANO_OS_FLAG_SET_SetFromIsr(&uart_flag_set, UART_TX_READY_FLAG_MASK);
			}
		}
		else
		{
		    /* Disable TXE interrupt */
		    USART3->CR1 &= ~(1u << 7u);
		}
    }

    /* Clear error flags */
    USART3->ICR = 0x0Fu;

    NANO_OS_INTERRUPT_Exit();
}




/** \brief Initialize a ring buffer */
static void UART_RingBufferInit(ring_buffer_t* const ring_buffer, uint8_t* const buffer, const size_t buffer_size)
{
    ring_buffer->buffer = buffer;
    ring_buffer->end_of_buffer = (buffer + buffer_size);
    ring_buffer->read_ptr = buffer;
    ring_buffer->write_ptr = buffer;
}

/** \brief Read a byte from a ring buffer */
static bool UART_RingBufferReadByte(ring_buffer_t* const ring_buffer, uint8_t* const byte)
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
static bool UART_RingBufferWriteByte(ring_buffer_t* const ring_buffer, const uint8_t byte)
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
static bool UART_RingBufferIsEmpty(const ring_buffer_t* const ring_buffer)
{
    return (ring_buffer->read_ptr == ring_buffer->write_ptr);
}

/** \brief Indicate if a ring buffer is full */
static bool UART_RingBufferIsFull(const ring_buffer_t* const ring_buffer)
{
    uint8_t* const write_ptr = ring_buffer->write_ptr + 1u;
    return ((write_ptr == ring_buffer->read_ptr) ||
            ((write_ptr == ring_buffer->end_of_buffer) && (ring_buffer->read_ptr == ring_buffer->buffer)));
}
