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

#ifndef NANO_OS_MUTEX_HPP
#define NANO_OS_MUTEX_HPP

#include "nano_os_mutex.h"

#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_MUTEX_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS mutex */
class NanoOsMutex
{
    public:

        /** \brief Default constructor */
        NanoOsMutex();

        /** \brief Constructor with creation */
        NanoOsMutex(const nano_os_queuing_type_t queuing_type);

        /** \brief Destructor */
        ~NanoOsMutex();


        /** \brief Create the mutex */
        nano_os_error_t create(const nano_os_queuing_type_t queuing_type);

        /** \brief Lock the mutex */
        nano_os_error_t lock(const uint32_t tick_count);

        #if (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u)

        /** \brief Try to lock the mutex */
        nano_os_error_t tryLock();

        #endif /* (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u) */

        /** \brief Unlock the mutex */
        nano_os_error_t unlock();

        #if (NANO_OS_MUTEX_SETNAME_ENABLED == 1u)

        /** \brief Associate a name to the mutex */
        nano_os_error_t setName(const char* const name);

        #endif /* (NANO_OS_MUTEX_SETNAME_ENABLED == 1u) */
    

    private:

        /** \brief Copy constructor => forbidden */
        NanoOsMutex(const NanoOsMutex& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying mutex object */
        nano_os_mutex_t m_mutex;

        // Needed for the condition variable class to be able to access to the
        // underlying mutex object
        friend class NanoOsCondVar;
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_MUTEX_ENABLED == 1u)

#endif // NANO_OS_MUTEX_HPP
