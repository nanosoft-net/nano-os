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

#include "nano_os_mailbox.h"

/* Check if module is enabled */
#if (NANO_OS_MAILBOX_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_trace.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"


/** \brief Prototype for the OS object specific action on wait object signal */
static bool NANO_OS_MAILBOX_WaitObjectWakeupCond(nano_os_wait_object_t* const wait_object, nano_os_task_t* task);


/** \brief Create a new mailbox */
nano_os_error_t NANO_OS_MAILBOX_Create(nano_os_mailbox_t* const mailbox, const uint32_t max_mail_count,
									   const size_t mail_size, void* const mailbox_buffer,
                                       const size_t mailbox_buffer_size, const nano_os_queuing_type_t queuing_type)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if ((mailbox != NULL) && (max_mail_count != 0u) &&
        (mail_size != 0u) && (mailbox_buffer != NULL) &&
		(mailbox_buffer_size >= NANO_OS_MAILBOX_SIZE(mail_size, max_mail_count)))
    {
        /* 0 init of mailbox */
        (void)MEMSET(mailbox, 0, sizeof(nano_os_mailbox_t));

        /* Init underlying wait object */
        ret = NANO_OS_WAIT_OBJECT_Initialize(&mailbox->wait_object, WOT_MAILBOX, queuing_type);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Initialize mailbox data */
            mailbox->mail_count = 0;
            mailbox->mail_size = mail_size;
            mailbox->max_mail_count = max_mail_count;
            mailbox->mail_buffer = NANO_OS_CAST(uint8_t*, mailbox_buffer);
            mailbox->read_pointer = mailbox->mail_buffer;
            mailbox->write_pointer = mailbox->mail_buffer;
            mailbox->end_pointer = &mailbox->mail_buffer[mailbox_buffer_size];

            /* Real time trace event */
            NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_CREATE, mailbox);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_MAILBOX_DESTROY_ENABLED == 1u)

/** \brief Destroy a mailbox */
nano_os_error_t NANO_OS_MAILBOX_Destroy(nano_os_mailbox_t* const mailbox)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((mailbox != NULL) && (mailbox->wait_object.type == WOT_MAILBOX))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_DESTROY, mailbox);

        /* Destroy wait object */
        ret = NANO_OS_WAIT_OBJECT_Destroy(&mailbox->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_MAILBOX_DESTROY_ENABLED == 1u) */


/** \brief Wait for a mail in a mailbox */
nano_os_error_t NANO_OS_MAILBOX_Wait(nano_os_mailbox_t* const mailbox, void* const mail_buffer,
                                     const uint32_t tick_count)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Real time trace event */
    NANO_OS_TRACE_OBJ(NOS_TRACE_MAILBOX_WAIT, mailbox);

    /* Check parameters */
    if ((mailbox != NULL) && (mail_buffer != NULL) && (mailbox->wait_object.type == WOT_MAILBOX))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_DESTROY, mailbox);

        /* Lock mailbox against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(mailbox->wait_object);

        /* Check if a mail is available */
        if (mailbox->mail_count != 0u)
        {
            /* Read mail */
        	(void)MEMCPY(mail_buffer, mailbox->read_pointer, mailbox->mail_size);
        	mailbox->mail_count--;
        	mailbox->read_pointer += mailbox->mail_size;
        	if (mailbox->read_pointer == mailbox->end_pointer)
        	{
        		mailbox->read_pointer = mailbox->mail_buffer;
        	}
			ret = NOS_ERR_SUCCESS;
        }
        else
        {
            /* Save the buffer to store the received mail */
            g_nano_os.current_task->mailbox_buffer = mail_buffer;

            /* Wait for a mail */
            ret = NANO_OS_WAIT_OBJECT_Wait(&mailbox->wait_object, tick_count);
        }

        /* Unlock mailbox against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(mailbox->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


/** \brief Post a mail in a mailbox */
nano_os_error_t NANO_OS_MAILBOX_Post(nano_os_mailbox_t* const mailbox, const void* const mail_buffer)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if ((mailbox != NULL) && (mail_buffer != NULL) && (mailbox->wait_object.type == WOT_MAILBOX))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_MAILBOX_POST, mailbox);

        /* Lock mailbox against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(mailbox->wait_object);

    	/* Check if a mail slot is available */
		if (mailbox->mail_count == mailbox->max_mail_count)
		{
			/* Mailbox is full */
			ret = NOS_ERR_MAILBOX_FULL;
		}
		else
		{
			uint32_t woke_up_task_count = 0u;

			/* Signal waiting tasks */
			ret = NANO_OS_WAIT_OBJECT_Signal(&mailbox->wait_object, NOS_ERR_SUCCESS, false, NANO_OS_MAILBOX_WaitObjectWakeupCond, &woke_up_task_count);
			if (ret == NOS_ERR_SUCCESS)
			{
				/* Check if the mail has been consumed by a task */
				if (woke_up_task_count == 0u)
				{
					/* No task was waiting on the mailbox, write mail into mailbox */
				    mailbox->mail_count++;
                    (void)MEMCPY(mailbox->write_pointer, mail_buffer, mailbox->mail_size);
                    mailbox->write_pointer += mailbox->mail_size;
                    if (mailbox->write_pointer == mailbox->end_pointer)
                    {
                        mailbox->write_pointer = mailbox->mail_buffer;
                    }
				}
				else
				{
				    /* Write the mail directly into the waiting task buffer */
				    (void)MEMCPY(g_nano_os.current_task->mailbox_buffer, mail_buffer, mailbox->mail_size);
				}
			}
		}

		/* Unlock mailbox against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(mailbox->wait_object);
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}


#if (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u)

/** \brief Post a mail in a mailbox from an interrupt handler */
nano_os_error_t NANO_OS_MAILBOX_PostFromIsr(nano_os_mailbox_t* const mailbox, const void* const mail_buffer)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((mailbox != NULL) && (mail_buffer != NULL) && (mailbox->wait_object.type == WOT_MAILBOX))
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_MAILBOX_POST_ISR, mailbox);

        /* Check if operation can be done */
        if (WAIT_OBJECT_ISR_ISLOCKED(mailbox->wait_object))
        {
            /* Fill interrupt service request */
            nano_os_isr_service_request_t request;
            request.service_func = NANO_OS_CAST(fp_nano_os_isr_func_t, NANO_OS_MAILBOX_Post);
            request.object = mailbox;
            request.param1 = NANO_OS_CAST(uint32_t, mail_buffer);

            /* Queue request */
            ret = NANO_OS_INTERRUPT_QueueRequest(&request);
        }
        else
        {
            /* No syscall in progress */
            ret = NANO_OS_MAILBOX_Post(mailbox, mail_buffer);
        }
    }

    return ret;
}

#endif /* (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u) */


#if (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u)

/** \brief Get the current number of mails into a mailbox */
nano_os_error_t NANO_OS_MAILBOX_GetCount(nano_os_mailbox_t* const mailbox, uint32_t* const count)
{
	nano_os_error_t ret = NOS_ERR_INVALID_ARG;

	/* Syscall entry */
	NANO_OS_SYSCALL_Enter(true);

	/* Check parameters */
	if ((mailbox != NULL) && (count != NULL) && (mailbox->wait_object.type == WOT_MAILBOX))
	{
	    /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_MAILBOX_GETCOUNT, mailbox);

        /* Lock mailbox against interrupt handler access */
        WAIT_OBJECT_ISR_LOCK(mailbox->wait_object);

		/* Copy current count */
		(*count) = mailbox->mail_count;

		/* Unlock mailbox against interrupt handler access */
        WAIT_OBJECT_ISR_UNLOCK(mailbox->wait_object);

		ret = NOS_ERR_SUCCESS;
	}

	/* Syscall exit */
	NANO_OS_SYSCALL_Exit();

	return ret;
}

#endif /* (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u) */


#if (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a mailbox */
nano_os_error_t NANO_OS_MAILBOX_SetName(nano_os_mailbox_t* const mailbox, const char* const name)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(false);

    /* Check parameters */
    if (mailbox != NULL)
    {
        /* Real time trace event */
        NANO_OS_TRACE_OBJ(NOS_TRACE_OBJ_NAMING, mailbox);

        /* Save the name */
        mailbox->wait_object.name = name;

        ret = NOS_ERR_SUCCESS;
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u) */


/** \brief Prototype for the OS object specific action on wait object signal */
static bool NANO_OS_MAILBOX_WaitObjectWakeupCond(nano_os_wait_object_t* const wait_object, nano_os_task_t* task)
{
    bool ret = false;
    nano_os_mailbox_t* const mailbox = NANO_OS_CAST(nano_os_mailbox_t*, wait_object);

    /* Check parameters */
    if ((mailbox != NULL) && (task != NULL))
    {
        /* Save the buffer of the woke up task to store the current mail */
        g_nano_os.current_task->mailbox_buffer = task->mailbox_buffer;
    }

    return ret;
}


#endif /* (NANO_OS_MAILBOX_ENABLED == 1u) */
