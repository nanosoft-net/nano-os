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

#include "nano_os_data.h"
#include "nano_os_port.h"
#include "nano_os_tools.h"
#include "nano_os_interrupt.h"
#include "nano_os_scheduler.h"


/** \brief Interrupt service request task */
static void* NANO_OS_INTERRUPT_ServiceTask(void* const param);



/** \brief Initialize the interrupt service module */
nano_os_error_t NANO_OS_INTERRUPT_Init(const nano_os_port_init_data_t* const port_init_data)
{
    nano_os_error_t ret;
    nano_os_task_init_data_t task_init_data;

    /* Initialize request list */
    g_nano_os.isr_request_count = 0u;
    g_nano_os.isr_request_read = g_nano_os.isr_requests;
    g_nano_os.isr_request_write = g_nano_os.isr_requests;

    /* Create service request task */
    (void)MEMSET(&task_init_data, 0, sizeof(nano_os_task_init_data_t));
    #if (NANO_OS_TASK_NAME_ENABLED == 1u)
    task_init_data.name = "ISR request task";
    #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
    task_init_data.base_priority = NANO_OS_NUMBER_OF_PRIORITY_LEVELS - 1u;
    task_init_data.stack_origin = g_nano_os.isr_request_task_stack;
    task_init_data.stack_size = NANO_OS_ISR_REQUEST_TASK_STACK_SIZE;
    task_init_data.task_func = NANO_OS_INTERRUPT_ServiceTask;
    task_init_data.param = NULL;
    #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
    (void)MEMCPY(&task_init_data.port_init_data, &port_init_data->isr_request_task_init_data, sizeof(nano_os_port_task_init_data_t));
    #endif /* (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u) */
    ret = NANO_OS_TASK_Create(&g_nano_os.isr_request_task, &task_init_data);

    /* Init underlying wait object */
    if (ret == NOS_ERR_SUCCESS)
    {
        ret = NANO_OS_WAIT_OBJECT_Initialize(&g_nano_os.isr_request_task_wait_object, WOT_TASK, QT_PRIORITY);
    }

    return ret;
}


/** \brief Queue an interrupt service request */
nano_os_error_t NANO_OS_INTERRUPT_QueueRequest(nano_os_isr_service_request_t* const isr_request)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (isr_request != NULL)
    {
        /* Disable interrupts */
        nano_os_int_status_reg_t int_status_reg;
        NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

        /* Check if there is space left in the request list */
        if (g_nano_os.isr_request_count == NANO_OS_MAX_ISR_REQUEST_COUNT)
        {
            /* List if already full */
            ret = NOS_ERR_ISR_REQUEST_LIST_FULL;
        }
        else
        {
            uint32_t woke_up_task_count = 0;

            /* Add the request to the list */
            (void)MEMCPY(g_nano_os.isr_request_write, isr_request, sizeof(nano_os_isr_service_request_t));
            g_nano_os.isr_request_count++;
            g_nano_os.isr_request_write++;
            if (g_nano_os.isr_request_write == &g_nano_os.isr_requests[NANO_OS_MAX_ISR_REQUEST_COUNT])
            {
                g_nano_os.isr_request_write = g_nano_os.isr_requests;
            }

            /* Signal interrupt service task */
            ret = NANO_OS_WAIT_OBJECT_Signal(&g_nano_os.isr_request_task_wait_object, NOS_ERR_SUCCESS, false, NULL, &woke_up_task_count);
        }

        /* Enable interrupts */
        NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);
    }

    return ret;
}



/** \brief Manage the entry into an interrupt handler */
void NANO_OS_INTERRUPT_Enter(void)
{
    /* Increment the nesting count */
    NANO_OS_PORT_ATOMIC_INC32(g_nano_os.int_nesting_count);
}

/** \brief Manage the exit of an interrupt handler */
void NANO_OS_INTERRUPT_Exit(void)
{
    nano_os_int_status_reg_t int_status_reg;

    /* Disable interrupts */
    NANO_OS_PORT_ENTER_CRITICAL(int_status_reg);

    /* Decrement the nesting count */
    g_nano_os.int_nesting_count--;

    /* If no more locks, check for context switch */
    if ((g_nano_os.int_nesting_count == 0u) && (g_nano_os.lock_count == 0u))
    {
        /* Call the scheduler */
        NANO_OS_SCHEDULER_Schedule(true);
    }

    /* Restore interrupts */
    NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg);
}


/** \brief Interrupt service request task */
static void* NANO_OS_INTERRUPT_ServiceTask(void* const param)
{
    NANO_OS_UNUSED(param);

    /* Task loop */
    while(true)
    {
        nano_os_error_t ret;

        /* Increment lock count as if it were a syscall */
        NANO_OS_PORT_ATOMIC_INC32(g_nano_os.lock_count);

        /* Wait for a request */
        NANO_OS_PORT_DISABLE_INTERRUPTS();
        ret = NANO_OS_WAIT_OBJECT_Wait(&g_nano_os.isr_request_task_wait_object, 0xFFFFFFFFu);
        NANO_OS_PORT_ENABLE_INTERRUPTS();

        /* Decrement lock count as if it were a syscall */
        NANO_OS_PORT_ATOMIC_DEC32(g_nano_os.lock_count);

        /* Check wait status */
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Handle requests */

            /* Disable interrupts */
            NANO_OS_PORT_DISABLE_INTERRUPTS();

            while (g_nano_os.isr_request_count != 0u)
            {
                /* Enable interrupts during system call */
                NANO_OS_PORT_ENABLE_INTERRUPTS();

                /* Execute system call */
                ret = g_nano_os.isr_request_read->service_func(g_nano_os.isr_request_read->object,
                                                               g_nano_os.isr_request_read->param1,
                                                               g_nano_os.isr_request_read->param2);
                if (ret != NOS_ERR_SUCCESS)
                {
                    /* Call error handler */
                    #if (NANO_OS_ISR_REQUEST_ERROR_CALLBACK_ENABLED == 1u)
                    NANO_OS_USER_IsrRequestErrorCallback(g_nano_os.isr_request_read, ret);
                    #else
                    NANO_OS_ERROR_ASSERT_RET(ret);
                    #endif /* (NANO_OS_ISR_REQUEST_ERROR_CALLBACK_ENABLED == 1u) */
                }

                /* Disable interrupts to re-check the request count and possibly re-enter the loop */
                NANO_OS_PORT_DISABLE_INTERRUPTS();

                /* Next request */
                g_nano_os.isr_request_count--;
                g_nano_os.isr_request_read++;
                if (g_nano_os.isr_request_read == &g_nano_os.isr_requests[NANO_OS_MAX_ISR_REQUEST_COUNT])
                {
                    g_nano_os.isr_request_read = g_nano_os.isr_requests;
                }
            }

            /* Enable interrupts */
            NANO_OS_PORT_DISABLE_INTERRUPTS();
        }
        else if (ret == NOS_ERR_TIMEOUT)
        {
            /* No request to handle, return in wait state */
        }
        else
        {
            /* Critical error */
            NANO_OS_ERROR_Handler(ret);
        }
    }

    return NULL;
}
