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

#ifndef NANO_OS_MAILBOX_HPP
#define NANO_OS_MAILBOX_HPP

#include "nano_os_mailbox.h"

#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_MAILBOX_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS mailbox */
class NanoOsMailbox
{
    public:

        /** \brief Default constructor */
        NanoOsMailbox();

        /** \brief Constructor with creation */
        NanoOsMailbox(const uint32_t max_mail_count,
                      const size_t mail_size, void* const mailbox_buffer,
                      const size_t mailbox_buffer_size, const nano_os_queuing_type_t queuing_type);

        /** \brief Destructor */
        ~NanoOsMailbox();


        /** \brief Create the mailbox */
        nano_os_error_t create(const uint32_t max_mail_count,
                               const size_t mail_size, void* const mailbox_buffer,
                               const size_t mailbox_buffer_size, const nano_os_queuing_type_t queuing_type);

        /** \brief Wait for a mail in the mailbox */
        nano_os_error_t wait(void* const mail_buffer,
                             const uint32_t tick_count);

        /** \brief Post a mail in the mailbox */
        nano_os_error_t post(void* const mail_buffer);

        #if (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u)

        /** \brief Post a mail in the mailbox from an interrupt handler */
        nano_os_error_t postFromIsr(void* const mail_buffer);

        #endif /* (NANO_OS_MAILBOX_POSTFROMISR_ENABLED == 1u) */

        #if (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u)

        /** \brief Get the current number of mails into the mailbox */
        nano_os_error_t getCount(uint32_t& count);

        #endif /* (NANO_OS_MAILBOX_GETCOUNT_ENABLED == 1u) */

        #if (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u)

        /** \brief Associate a name to the mailbox */
        nano_os_error_t setName(const char* const name);

        #endif /* (NANO_OS_MAILBOX_SETNAME_ENABLED == 1u) */

    private:

        /** \brief Copy constructor => forbidden */
        NanoOsMailbox(const NanoOsMailbox& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying mailbox object */
        nano_os_mailbox_t m_mailbox;
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_MAILBOX_ENABLED == 1u)

#endif // NANO_OS_MAILBOX_HPP
