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

#include "ipc.h"

#include "nano_os_api.h"
#include "nano_os_tools.h"

#include "chip_lpc43xx.h"


/** \brief Timeout in milliseconds for IPC message sending */
#define IPC_MSG_SEND_TIMEOUT        100u



/** \brief Max size of an IPC message payload in bytes */
#define MAX_IPC_MSG_PAYLOAD_SIZE    32u

/** \brief Max number of IPC message in an IPC message queue */
#define MAX_IPC_MSG_QUEUE_SIZE      100u


/** \brief Base address for the first IPC message queue */
#define IPC_MSG_QUEUE1_BASE_ADDRESS 0x2000C000u

/** \brief Base address for the second IPC message queue */
#define IPC_MSG_QUEUE2_BASE_ADDRESS 0x2000D000u


/** \brief Flag to indicate a fragmented message */
#define IPC_MSG_FLAG_FRAGMENTED 0x01u



/** \brief IPC message */
typedef struct _ipc_msgt_t
{
    /** \brief Command */
    uint16_t cmd;
    /** \brief Flags */
    uint8_t flags;
    /** \brief Size */
    uint8_t size;
    /** \brief Payload */
    uint8_t payload[MAX_IPC_MSG_PAYLOAD_SIZE];
} ipc_msg_t;

/** \brief IPC message queue */
typedef struct _ipc_msg_queue_t
{
    /** \brief Write index */
    uint16_t write_index;
    /** \brief Read index */
    uint16_t read_index;

    /** \brief Message array */
    ipc_msg_t msgs[MAX_IPC_MSG_QUEUE_SIZE];
} ipc_msg_queue_t;



/** \brief Message queue to send messages */
static volatile ipc_msg_queue_t* tx_msg_queue;

/** \brief Message queue to receive messages */
static volatile ipc_msg_queue_t* rx_msg_queue;



/** \brief Rx ready flag mask */
#define IPC_RX_READY_FLAG_MASK     1u

/** \brief Tx ready flag mask */
#define IPC_TX_READY_FLAG_MASK     2u

/** \brief Flag set to synchronize Rx and Tx with IPC IRQ */
static nano_os_flag_set_t ipc_flag_set;


/** \brief Acknowledge IPC IRQ */
static void IPC_AcknowledgeInterrupt(void);

/** \brief Enable IPC IRQ */
static void IPC_EnableInterrupt(void);

/** \brief Disable IPC IRQ */
static void IPC_DisableInterrupt(void);







/** \brief Initialize the IPC driver */
void IPC_Init(void)
{

    /* Initialize message queues pointers */
    #ifdef CORE_M4
    tx_msg_queue = NANO_OS_CAST(volatile ipc_msg_queue_t*, IPC_MSG_QUEUE1_BASE_ADDRESS);
    rx_msg_queue = NANO_OS_CAST(volatile ipc_msg_queue_t*, IPC_MSG_QUEUE2_BASE_ADDRESS);
    #else
    tx_msg_queue = NANO_OS_CAST(volatile ipc_msg_queue_t*, IPC_MSG_QUEUE2_BASE_ADDRESS);
    rx_msg_queue = NANO_OS_CAST(volatile ipc_msg_queue_t*, IPC_MSG_QUEUE1_BASE_ADDRESS);
    #endif /* CORE_M4 */


    /* Initialize message queues (Cortex-M4 only initialize only once) */
    #ifdef CORE_M4
    (void)MEMSET(NANO_OS_CAST(void*, tx_msg_queue), 0u, sizeof(ipc_msg_queue_t));
    (void)MEMSET(NANO_OS_CAST(void*, rx_msg_queue), 0u, sizeof(ipc_msg_queue_t));
    #endif /* CORE_M4 */

    /* Initialize flag set */
    (void)NANO_OS_FLAG_SET_Create(&ipc_flag_set, 0u, QT_PRIORITY);

    /* Enable interprocess IRQ */
    IPC_AcknowledgeInterrupt();
    IPC_EnableInterrupt();
}

/** \brief Send message over IPC */
bool IPC_SendMessage(const uint16_t command, const void* const data, const size_t data_size)
{
    bool ret = false;

    /* Check parameters */
    if ((data != NULL) || (data_size == 0u))
    {
        /* Send loop */
        bool message_available;
        size_t data_left = data_size;
        uint32_t write_index = tx_msg_queue->write_index;
        const uint8_t* byte_data = NANO_OS_CAST(const uint8_t*, data);
        do
        {
            /* Check if a message is available */
            IPC_DisableInterrupt();
            message_available = (tx_msg_queue->read_index != write_index);
            if (!message_available)
            {
                const nano_os_error_t err = NANO_OS_FLAG_SET_Clear(&ipc_flag_set, IPC_TX_READY_FLAG_MASK);
                NANO_OS_ERROR_ASSERT_RET(err);
            }
            IPC_EnableInterrupt();
            if (message_available)
            {
                volatile ipc_msg_t* msg = &tx_msg_queue->msgs[write_index];

                /* Fill command */
                msg->cmd = command;

                /* Compute message size */
                msg->flags = 0u;
                msg->size = data_left;
                if (msg->size > MAX_IPC_MSG_PAYLOAD_SIZE)
                {
                    msg->size = MAX_IPC_MSG_PAYLOAD_SIZE;
                    msg->flags = IPC_MSG_FLAG_FRAGMENTED;
                }

                /* Fill payload */
                (void)MEMCPY(NANO_OS_CAST(void*, &msg->payload[0u]), byte_data, msg->size);

                /* Prepare for next message */
                byte_data += msg->size;
                data_left -= msg->size;
                if (write_index == (MAX_IPC_MSG_QUEUE_SIZE - 1u))
                {
                    write_index++;
                }
                else
                {
                    write_index = 0u;
                }
                ret = true;
            }
            else
            {
                /* Wait for a free message in the message queue */
                uint32_t flags = 0u;
                const nano_os_error_t err = NANO_OS_FLAG_SET_Wait(&ipc_flag_set, IPC_TX_READY_FLAG_MASK, false, false, &flags, NANO_OS_MS_TO_TICKS(IPC_MSG_SEND_TIMEOUT));
                NANO_OS_ERROR_ASSERT((err == NOS_ERR_SUCCESS) || (err == NOS_ERR_TIMEOUT), NOS_ERR_FAILURE);
                ret = (err == NOS_ERR_SUCCESS);
            }
        }
        while (ret && (!message_available || (data_left != 0u)));
        if (ret)
        {
            /* Update write index */
            tx_msg_queue->write_index = write_index;

            /* Trigger inter-cpu interrupt */
            __asm("sev");
        }
    }

    return ret;
}

/** \brief Receive message over IPC */
bool IPC_ReceiveMessage(uint16_t* const command, void* const data, size_t* data_size, const uint32_t ms_timeout)
{
    bool ret = false;

    /* Check parameters */
    if ((command != NULL) && (data != NULL) && (data_size != NULL))
    {
        /* Receive loop */
        bool message_available;
        uint8_t flags = 0u;
        uint32_t read_index = rx_msg_queue->read_index;
        uint8_t* byte_data = NANO_OS_CAST(uint8_t*, data);
        do
        {
            /* Check if a message is available */
            IPC_DisableInterrupt();
            message_available = (rx_msg_queue->write_index != read_index);
            if (!message_available)
            {
                (void)NANO_OS_FLAG_SET_Clear(&ipc_flag_set, IPC_RX_READY_FLAG_MASK);
            }
            IPC_EnableInterrupt();
            if (message_available)
            {
                volatile ipc_msg_t* msg = &rx_msg_queue->msgs[read_index];

                /* Read command */
                (*command) = msg->cmd;

                /* Read flags */
                flags = msg->flags;

                /* Read data */
                (void)MEMCPY(byte_data, NANO_OS_CAST(void*, &msg->payload[0u]), msg->size);

                /* Prepare for next message */
                byte_data += msg->size;
                (*data_size) += msg->size;
                if (read_index == (MAX_IPC_MSG_QUEUE_SIZE - 1u))
                {
                    read_index++;
                }
                else
                {
                    read_index = 0u;
                }
                ret = true;
            }
            else
            {
                /* Wait for a message in the message queue */
                uint32_t flags = 0u;
                const nano_os_error_t err = NANO_OS_FLAG_SET_Wait(&ipc_flag_set, IPC_RX_READY_FLAG_MASK, false, false, &flags, NANO_OS_MS_TO_TICKS(ms_timeout));
                ret = (err == NOS_ERR_SUCCESS);
            }
        }
        while (ret && (!message_available || ((flags & IPC_MSG_FLAG_FRAGMENTED) != 0u)));
        if (ret)
        {
            /* Update read index */
            rx_msg_queue->read_index = read_index;

            /* Trigger inter-cpu interrupt */
            __asm("sev");
        }
    }

    return ret;
}


/** \brief IPC IRQ handler */
#ifdef CORE_M4
void M0APP_Handler(void)
#else
void M4_Handler(void)
#endif /* CORE_M4 */
{
    uint32_t flags = 0u;

    NANO_OS_INTERRUPT_Enter();

    /* Check space left in receive message queue */
    if (rx_msg_queue->read_index != rx_msg_queue->write_index)
    {
        flags = IPC_RX_READY_FLAG_MASK;
    }

    /* Check space left in send message queue */
    if (tx_msg_queue->read_index != tx_msg_queue->write_index)
    {
        flags |= IPC_TX_READY_FLAG_MASK;
    }

    /* Signal flag set if needed */
    if (flags != 0u)
    {
        (void)NANO_OS_FLAG_SET_SetFromIsr(&ipc_flag_set, flags);
    }

    /* Acknowledge interrupt */
    IPC_AcknowledgeInterrupt();

    NANO_OS_INTERRUPT_Exit();
}


/** \brief Acknowledge IPC IRQ */
static void IPC_AcknowledgeInterrupt(void)
{
    #ifdef CORE_M4
    LPC_CREG->M0APPTXEVENT = 0u;
    #else
    LPC_CREG->M4TXEVENT = 0u;
    #endif /* CORE_M4 */
}

/** \brief Enable IPC IRQ */
static void IPC_EnableInterrupt(void)
{
    #ifdef CORE_M4
    NVIC_EnableIRQ(M0APP_IRQn);
    #else
    NVIC_EnableIRQ(M4_IRQn);
    #endif /* CORE_M4 */
}

/** \brief Disable IPC IRQ */
static void IPC_DisableInterrupt(void)
{
    #ifdef CORE_M4
    NVIC_DisableIRQ(M0APP_IRQn);
    #else
    NVIC_DisableIRQ(M4_IRQn);
    #endif /* CORE_M4 */
}

