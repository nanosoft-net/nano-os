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

#ifndef NANO_OS_MAILBOX_H
#define NANO_OS_MAILBOX_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_MAILBOX_ENABLED == 1u)


#include "nano_os_wait_object.h"
#include "nano_os_interrupt.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Macro to help to determine the buffer size needed for a mail box */
#define NANO_OS_MAILBOX_SIZE(mail_size, max_mail_count)		((mail_size) * (max_mail_count))



/** \brief Nano OS mailbox set */
typedef struct _nano_os_mailbox_t
{
    /** \brief Underlying wait object */
    nano_os_wait_object_t wait_object;

    /** \brief Mail size */
    size_t mail_size;
    /** \brief Maximum mail count */
    uint32_t max_mail_count;
    /** \brief Mail buffer */
    uint8_t* mail_buffer;
    /** \brief Current mail count */
	uint32_t mail_count;
	/** \brief Read pointer */
	uint8_t* read_pointer;
	/** \brief Write pointer */
	uint8_t* write_pointer;
	/** \brief End pointer */
	uint8_t* end_pointer;
} nano_os_mailbox_t;




/** \brief Create a new mailbox */
nano_os_error_t NANO_OS_MAILBOX_Create(nano_os_mailbox_t* const mailbox, const uint32_t max_mail_count,
									   const size_t mail_size, void* const mailbox_buffer,
                                       const size_t mailbox_buffer_size, const nano_os_queuing_type_t queuing_type);

#if (NANO_OS_MAILBOX_DESTROY_ENABLED == 1u)

/** \brief Destroy a mailbox */
nano_os_error_t NANO_OS_MAILBOX_Destroy(nano_os_mailbox_t* const mailbox);

#endif /* (NANO_OS_MAILBOX_DESTROY_ENABLED == 1u) */

/** \brief Wait for a mail in a mailbox */
nano_os_error_t NANO_OS_MAILBOX_Wait(nano_os_mailbox_t* const mailbox, void* const mail_buffer,
                                     const uint32_t tick_count);

/** \brief Post a mail in a mailbox */
nano_os_error_t NANO_OS_MAILBOX_Post(nano_os_mailbox_t* const mailbox, const void* const mail_buffer);

#if (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u)

/** \brief Post a mail in a mailbox from an interrupt handler */
nano_os_error_t NANO_OS_MAILBOX_PostFromIsr(nano_os_mailbox_t* const mailbox, const void* const mail_buffer);

#endif /* (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u) */

#if (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u)

/** \brief Get the current number of mails into a mailbox */
nano_os_error_t NANO_OS_MAILBOX_GetCount(nano_os_mailbox_t* const mailbox, uint32_t* const count);

#endif /* (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u) */

#if (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u)

/** \brief Associate a name to a mailbox */
nano_os_error_t NANO_OS_MAILBOX_SetName(nano_os_mailbox_t* const mailbox, const char* const name);

#endif /* (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u) */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_MAILBOX_ENABLED == 1u) */

#endif /* NANO_OS_MAILBOX_H */
