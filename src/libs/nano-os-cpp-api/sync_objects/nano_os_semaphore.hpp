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

#ifndef NANO_OS_SEMAPHORE_HPP
#define NANO_OS_SEMAPHORE_HPP

#include "nano_os_semaphore.h"

#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_SEMAPHORE_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS semaphore */
class NanoOsSemaphore
{
    public:

        /** \brief Default constructor */
        NanoOsSemaphore();

        /** \brief Constructor with creation */
        NanoOsSemaphore(const uint32_t initial_count,
                        const uint32_t max_count, const nano_os_queuing_type_t queuing_type);

        /** \brief Destructor */
        ~NanoOsSemaphore();


        /** \brief Create the semaphore */
        nano_os_error_t create(const uint32_t initial_count,
                               const uint32_t max_count, const nano_os_queuing_type_t queuing_type);

        /** \brief Wait for a token in the semaphore */
        nano_os_error_t wait(const uint32_t tick_count);

        /** \brief Post a token to the semaphore */
        nano_os_error_t post();

        #if (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u)

        /** \brief Post a token to the semaphore from an interrupt handler */
        nano_os_error_t postFromIsr();

        #endif /* (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u) */

        #if (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u)

        /** \brief Get the current count of the semaphore */
        nano_os_error_t getCount(uint32_t& count);

        #endif /* (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u) */

        #if (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u)

        /** \brief Associate a name to the semaphore */
        nano_os_error_t setName(const char* const name);

        #endif /* (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u) */

    private:

        /** \brief Copy constructor => forbidden */
        NanoOsSemaphore(const NanoOsSemaphore& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying semaphore object */
        nano_os_semaphore_t m_semaphore;
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_SEMAPHORE_ENABLED == 1u)

#endif // NANO_OS_SEMAPHORE_HPP
