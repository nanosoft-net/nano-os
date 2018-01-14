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

#ifndef NANO_OS_FLAG_SET_HPP
#define NANO_OS_FLAG_SET_HPP

#include "nano_os_flag_set.h"

#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_FLAG_SET_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS flag set */
class NanoOsFlagSet
{
    public:

        /** \brief Default constructor */
        NanoOsFlagSet();

        /** \brief Constructor with creation */
        NanoOsFlagSet(const uint32_t initial_mask,
                      const nano_os_queuing_type_t queuing_type);

        /** \brief Destructor */
        ~NanoOsFlagSet();


        /** \brief Create the flag set */
        nano_os_error_t create(const uint32_t initial_mask,
                               const nano_os_queuing_type_t queuing_type);

        /** \brief Wait for a specific flag mask in the flag set */
        nano_os_error_t wait(const uint32_t wait_mask,
                             const bool wait_all, const bool reset_flags,
                             uint32_t* const wakeup_flags, const uint32_t tick_count);

        /** \brief Set a flag mask in the flag set */
        nano_os_error_t set(const uint32_t mask);

        #if (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u)

        /** \brief Set a flag mask in the flag set from an interrupt handler */
        nano_os_error_t setFromIsr(const uint32_t mask);

        #endif /* (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u) */

        #if (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u)

        /** \brief Clear a flag mask in the flag set */
        nano_os_error_t clear(const uint32_t mask);

        #if (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u)

        /** \brief Clear a flag mask in the flag set from an interrupt handler */
        nano_os_error_t clearFromIsr(const uint32_t mask);

        #endif /* (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u) */

        #endif /* (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u) */

        #if (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u)

        /** \brief Associate a name to the flag set */
        nano_os_error_t setName(const char* const name);

        #endif /* (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u) */


    private:

        /** \brief Copy constructor => forbidden */
        NanoOsFlagSet(const NanoOsFlagSet& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying flag set object */
        nano_os_flag_set_t m_flag_set;
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_FLAG_SET_ENABLED == 1u)

#endif // NANO_OS_FLAG_SET_HPP
