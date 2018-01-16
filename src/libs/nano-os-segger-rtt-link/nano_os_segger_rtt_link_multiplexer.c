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

#include "nano_os_segger_rtt_link_multiplexer.h"

/* Check if module is enabled */
#if (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u)

#include "SEGGER_RTT.h"

#include "nano_os_api.h"
#include "nano_os_user.h"



/** \brief Decoding state of the multiplexer task */
typedef enum _nano_os_segger_rtt_link_multiplexer_state_t
{
    /** \brief Waiting new packet */
    NOSRLMS_WAITING_PACKET = 0u,
    /** \brief Waiting channel */
    NOSRLMS_WAITING_CHANNEL = 1u,
    /** \brief Waiting data size low */
    NOSRLMS_WAITING_DATA_SIZE_LOW = 2u,
    /** \brief Waiting data size high */
    NOSRLMS_WAITING_DATA_SIZE_HIGH = 3u,
    /** \brief Waiting data */
    NOSRLMS_WAITING_DATA = 4u

} nano_os_segger_rtt_link_multiplexer_state_t;

/** \brief Multiplexer decoder data */
typedef struct _nano_os_segger_rtt_link_multiplexer_decoder_t
{
    /** \brief Decoder state */
    nano_os_segger_rtt_link_multiplexer_state_t state;
    /** \brief Channel id of the current packet */
    uint8_t channel;
    /** \brief Size of the current packet */
    uint16_t packet_size;
} nano_os_segger_rtt_link_multiplexer_decoder_t;




/** \brief Number of buffer to retrieve RTT data */
#define NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_COUNT    (NANO_OS_SEGGER_RTT_LINK_CONSOLE_ENABLED + \
                                                                NANO_OS_SEGGER_RTT_LINK_DEBUG_ENABLED)


/** \brief Size of the buffer to retrieve RTT data in bytes */
#define NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_SIZE     32u

/** \brief Ring buffer to retrieve RTT data */
typedef struct _nano_os_segger_rtt_link_multiplexer_ring_buffer_t
{
    /** \brief Data */
    uint8_t data[NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_SIZE];
    /** \brief Read pointer */
    const uint8_t* read_ptr;
    /** \brief Write pointer */
    uint8_t* write_ptr;
} nano_os_segger_rtt_link_multiplexer_ring_buffer_t;



/** \brief Buffers to retrieve data on each channel */
static nano_os_segger_rtt_link_multiplexer_ring_buffer_t nano_os_segger_rtt_link_receive_buffers[NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_COUNT];

/** \brief Mutex to access the RTT link */
static nano_os_mutex_t nano_os_segger_rtt_link_mutex;

/** \brief Buffer to send RTT data */
static uint8_t nano_os_segger_rtt_link_send_buffer[512u];

/** \brief Buffer to receive RTT data */
static uint8_t nano_os_segger_rtt_link_receive_buffer[32u];

/** \brief Decoder state */
static nano_os_segger_rtt_link_multiplexer_decoder_t nano_os_segger_rtt_link_multiplexer_decoder;


/** \brief Multiplexer decoder to receive and dispatch data from RTT link */
static void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_DecoderReceiveAndDispatch(void);

/** \brief Initialize a ring buffer */
static void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_InitRingBuffer(nano_os_segger_rtt_link_multiplexer_ring_buffer_t* const ring_buffer);

/** \brief Write a byte into the ring buffer */
static void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RingBufferWrite(nano_os_segger_rtt_link_multiplexer_ring_buffer_t* const ring_buffer,
                                                                const uint8_t data);

/** \brief Read a byte from the ring buffer */
static bool NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RingBufferRead(nano_os_segger_rtt_link_multiplexer_ring_buffer_t* const ring_buffer,
                                                               uint8_t* const data);




/** \brief Initialize the multiplexer for RTT packets */
nano_os_error_t NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Init(void)
{
    int32_t err;
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    /* 0 init */
    (void)MEMSET(&nano_os_segger_rtt_link_multiplexer_decoder, 0, sizeof(nano_os_segger_rtt_link_multiplexer_decoder_t));
    for (err = 0; err < NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_COUNT; err++)
    {
        NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_InitRingBuffer(&nano_os_segger_rtt_link_receive_buffers[err]);
    }

    /* Configure buffers */
    err = SEGGER_RTT_ConfigUpBuffer(NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_BUFFER,
                                    "Nano OS Multiplexer Up",
                                    nano_os_segger_rtt_link_send_buffer,
                                    sizeof(nano_os_segger_rtt_link_send_buffer),
                                    SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    if (err != 0)
    {
        ret = NOS_ERR_FAILURE;
    }
    err = SEGGER_RTT_ConfigDownBuffer(NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_BUFFER,
                                      "Nano OS Multiplexer Down",
                                      nano_os_segger_rtt_link_receive_buffer,
                                      sizeof(nano_os_segger_rtt_link_receive_buffer),
                                      SEGGER_RTT_MODE_NO_BLOCK_SKIP);
    if (err != 0)
    {
        ret = NOS_ERR_FAILURE;
    }

    /* Create mutex */
    if (ret == NOS_ERR_SUCCESS)
    {
        ret = NANO_OS_MUTEX_Create(&nano_os_segger_rtt_link_mutex, QT_PRIORITY);
    }

    return ret;
}


/** \brief Send data using the multiplexer on the RTT link */
void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Send(const uint8_t channel, const void* const data, const size_t data_size)
{
    uint32_t ret;
    nano_os_int_status_reg_t int_status_reg;

    /* Prepare multiplex information */
    uint8_t multiplex_header[4u];
    multiplex_header[0u] = 0xF0;
    multiplex_header[1u] = channel;
    multiplex_header[2u] = NANO_OS_CAST(uint8_t, (data_size & 0xFFu));
    multiplex_header[3u] = NANO_OS_CAST(uint8_t, ((data_size >> 8u) & 0xFFu));

    /* Disable interrupts to protect against trace data sent from interrupt handlers */
    NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

    /* Send multiplex information */
    ret = NANO_OS_CAST(uint32_t, SEGGER_RTT_WriteSkipNoLock(NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_BUFFER, multiplex_header, sizeof(multiplex_header)));
    if (ret != 0u)
    {
        /* Send data */
        do
        {
            ret = NANO_OS_CAST(uint32_t, SEGGER_RTT_WriteSkipNoLock(NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_BUFFER, data, data_size));
        }
        while (ret == 0u);
    }

    /* Restore interrupts */
    NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);
}


/** \brief Wait forever for an incoming byte the RTT link */
nano_os_error_t NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_Read(const uint8_t channel, uint8_t* const data)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((channel < NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_COUNT) && (data != NULL))
    {
        bool success;
        do
        {
            /* Lock decoder */
            (void)NANO_OS_MUTEX_Lock(&nano_os_segger_rtt_link_mutex, 0xFFFFFFFFu);

            /* Decode incoming data if any available */
            NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_DecoderReceiveAndDispatch();

            /* Try to read a byte from the corresponding ring buffer */
            success = NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RingBufferRead(&nano_os_segger_rtt_link_receive_buffers[channel], data);

            /* Unlock decoder */
            (void)NANO_OS_MUTEX_Unlock(&nano_os_segger_rtt_link_mutex);

            /* Sleep until next try */
            if (!success)
            {
                NANO_OS_TASK_Sleep(NANO_OS_MS_TO_TICKS(NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_POOLING_PERIOD));
            }
        }
        while (!success);

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/** \brief Multiplexer decoder to receive and dispatch data from RTT link */
static void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_DecoderReceiveAndDispatch(void)
{
    uint8_t read_byte = 0u;
    int32_t nb_byte_read = 0;

    do
    {
        /* Try to read a byte from the RTT link */
        nb_byte_read = SEGGER_RTT_ReadNoLock(NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_BUFFER, &read_byte, 1u);
        if (nb_byte_read != 0u)
        {
            /* Decode char */
            switch(nano_os_segger_rtt_link_multiplexer_decoder.state)
            {
                case NOSRLMS_WAITING_PACKET:
                {
                    /* Wait for packet start */
                    if (read_byte == 0xF0u)
                    {
                        nano_os_segger_rtt_link_multiplexer_decoder.state = NOSRLMS_WAITING_CHANNEL;
                    }
                }
                break;

                case NOSRLMS_WAITING_CHANNEL:
                {
                    /* Wait for the channel id */
                    if (read_byte < NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_COUNT)
                    {
                        nano_os_segger_rtt_link_multiplexer_decoder.channel = read_byte;
                        nano_os_segger_rtt_link_multiplexer_decoder.state = NOSRLMS_WAITING_DATA_SIZE_LOW;
                    }
                    else
                    {
                        nano_os_segger_rtt_link_multiplexer_decoder.state = NOSRLMS_WAITING_PACKET;
                    }
                }
                break;

                case NOSRLMS_WAITING_DATA_SIZE_LOW:
                {
                    /* Wait for data size */
                    nano_os_segger_rtt_link_multiplexer_decoder.packet_size = read_byte;
                    nano_os_segger_rtt_link_multiplexer_decoder.state = NOSRLMS_WAITING_DATA_SIZE_HIGH;
                }
                break;

                case NOSRLMS_WAITING_DATA_SIZE_HIGH:
                {
                    /* Wait for data size */
                    nano_os_segger_rtt_link_multiplexer_decoder.packet_size += NANO_OS_CAST(uint16_t, (read_byte << 8u));
                    if (nano_os_segger_rtt_link_multiplexer_decoder.packet_size > 0)
                    {
                        nano_os_segger_rtt_link_multiplexer_decoder.state = NOSRLMS_WAITING_DATA;
                    }
                    else
                    {
                        nano_os_segger_rtt_link_multiplexer_decoder.state = NOSRLMS_WAITING_PACKET;
                    }
                }
                break;

                case NOSRLMS_WAITING_DATA:
                {
                    /* Store received data in corresponding ring buffer */
                    NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RingBufferWrite(&nano_os_segger_rtt_link_receive_buffers[nano_os_segger_rtt_link_multiplexer_decoder.channel],
                                                                        read_byte);

                    /* Check end of packet */
                    nano_os_segger_rtt_link_multiplexer_decoder.packet_size--;
                    if (nano_os_segger_rtt_link_multiplexer_decoder.packet_size == 0u)
                    {
                        nano_os_segger_rtt_link_multiplexer_decoder.state = NOSRLMS_WAITING_PACKET;
                    }
                }
                break;

                default:
                {
                    NANO_OS_ERROR_ASSERT(false, NOS_ERR_FAILURE);
                }
                break;
            }
        }
    }
    while (nb_byte_read != 0u);

    return;
}


/** \brief Initialize a ring buffer */
static void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_InitRingBuffer(nano_os_segger_rtt_link_multiplexer_ring_buffer_t* const ring_buffer)
{
    /* Initialize read and write pointers */
    ring_buffer->read_ptr = &ring_buffer->data[0u];
    ring_buffer->write_ptr = &ring_buffer->data[0u];
}

/** \brief Write a byte into the ring buffer */
static void NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RingBufferWrite(nano_os_segger_rtt_link_multiplexer_ring_buffer_t* const ring_buffer,
                                                                const uint8_t data)
{
    /* Store data */
    (*ring_buffer->write_ptr) = data;

    /* Update write pointer */
    ring_buffer->write_ptr++;
    if (ring_buffer->write_ptr == &ring_buffer->data[NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_SIZE])
    {
        ring_buffer->write_ptr = &ring_buffer->data[0u];
    }

    /* Update read pointer if needed */
    if (ring_buffer->write_ptr == ring_buffer->read_ptr)
    {
        ring_buffer->read_ptr++;
        if (ring_buffer->read_ptr == &ring_buffer->data[NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_SIZE])
        {
            ring_buffer->read_ptr = &ring_buffer->data[0u];
        }
    }
}

/** \brief Read a byte from the ring buffer */
static bool NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RingBufferRead(nano_os_segger_rtt_link_multiplexer_ring_buffer_t* const ring_buffer,
                                                               uint8_t* const data)
{
    bool ret = false;

    /* Check if data is available */
    if (ring_buffer->read_ptr != ring_buffer->write_ptr)
    {
        /* Read data */
        (*data) = (*ring_buffer->read_ptr);

        /* Update read pointer */
        ring_buffer->read_ptr++;
        if (ring_buffer->read_ptr == &ring_buffer->data[NANO_OS_SEGGER_RTT_LINK_MULTIPLEXER_RX_BUFFER_SIZE])
        {
            ring_buffer->read_ptr = &ring_buffer->data[0u];
        }

        ret = true;
    }

    return ret;
}

#endif /* (NANO_OS_SEGGER_RTT_LINK_MULTIPLEX_ENABLED == 1u) */
