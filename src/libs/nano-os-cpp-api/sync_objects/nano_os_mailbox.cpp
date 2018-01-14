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

#include "../sync_objects/nano_os_mailbox.hpp"

// Check if module is enabled
#if (NANO_OS_MAILBOX_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsMailbox::NanoOsMailbox()
: m_mailbox()
{
}

/** \brief Constructor with creation */
NanoOsMailbox::NanoOsMailbox(const uint32_t max_mail_count,
                             const size_t mail_size, void* const mailbox_buffer,
                             const size_t mailbox_buffer_size, const nano_os_queuing_type_t queuing_type)
: m_mailbox()
{
    const nano_os_error_t ret = create(max_mail_count, mail_size, mailbox_buffer, 
                                       mailbox_buffer_size, queuing_type);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsMailbox::~NanoOsMailbox()
{
    #if (NANO_OS_MAILBOX_DESTROY_ENABLED == 1u)

    // Destroy the mailbox
    const nano_os_error_t ret = NANO_OS_MAILBOX_Destroy(&m_mailbox);
    NANO_OS_ERROR_ASSERT_RET(ret);

    #else

    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);

    #endif /* (NANO_OS_MAILBOX_DESTROY_ENABLED == 1u) */
}

/** \brief Create the mailbox */
nano_os_error_t NanoOsMailbox::create(const uint32_t max_mail_count,
                                      const size_t mail_size, void* const mailbox_buffer,
                                      const size_t mailbox_buffer_size, const nano_os_queuing_type_t queuing_type)
{
    return NANO_OS_MAILBOX_Create(&m_mailbox, max_mail_count, mail_size, 
                                  mailbox_buffer, mailbox_buffer_size, queuing_type);
}

/** \brief Wait for a mail in the mailbox */
nano_os_error_t NanoOsMailbox::wait(void* const mail_buffer,
                                    const uint32_t tick_count)
{
    return NANO_OS_MAILBOX_Wait(&m_mailbox, mail_buffer, tick_count);
}

/** \brief Post a mail in the mailbox */
nano_os_error_t NanoOsMailbox::post(void* const mail_buffer)
{
    return NANO_OS_MAILBOX_Post(&m_mailbox, mail_buffer);
}

#if (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u)

/** \brief Post a mail in the mailbox from an interrupt handler */
nano_os_error_t NanoOsMailbox::postFromIsr(void* const mail_buffer)
{
    return NANO_OS_MAILBOX_PostFromIsr(&m_mailbox, mail_buffer);
}

#endif /* (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u) */

#if (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u)

/** \brief Get the current number of mails into the mailbox */
nano_os_error_t NanoOsMailbox::getCount(uint32_t& count)
{
    return NANO_OS_MAILBOX_GetCount(&m_mailbox, &count);
}

#endif /* (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u) */

#if (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u)

/** \brief Associate a name to the mailbox */
nano_os_error_t NanoOsMailbox::setName(const char* const name)
{
    return NANO_OS_MAILBOX_SetName(&m_mailbox, name);
}

#endif /* (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u) */

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_MAILBOX_ENABLED == 1u)
