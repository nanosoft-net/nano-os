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

#include "../sync_objects/nano_os_mutex.hpp"

// Check if module is enabled
#if (NANO_OS_MUTEX_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsMutex::NanoOsMutex()
: m_mutex()
{
}

/** \brief Constructor with creation */
NanoOsMutex::NanoOsMutex(const nano_os_queuing_type_t queuing_type)
: m_mutex()
{
    const nano_os_error_t ret = create(queuing_type);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsMutex::~NanoOsMutex()
{
    #if (NANO_OS_MUTEX_DESTROY_ENABLED == 1u)

    // Destroy the mutex
    const nano_os_error_t ret = NANO_OS_MUTEX_Destroy(&m_mutex);
    NANO_OS_ERROR_ASSERT_RET(ret);

    #else

    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);

    #endif /* (NANO_OS_MUTEX_DESTROY_ENABLED == 1u) */
}

/** \brief Create the mutex */
nano_os_error_t NanoOsMutex::create(const nano_os_queuing_type_t queuing_type)
{
    return NANO_OS_MUTEX_Create(&m_mutex, queuing_type);
}

/** \brief Lock the mutex */
nano_os_error_t NanoOsMutex::lock(const uint32_t tick_count)
{
    return NANO_OS_MUTEX_Lock(&m_mutex, tick_count);
}

#if (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u)

/** \brief Try to lock the mutex */
nano_os_error_t NanoOsMutex::tryLock()
{
    return NANO_OS_MUTEX_TryLock(&m_mutex);
}

#endif /* (NANO_OS_MUTEX_TRYLOCK_ENABLED == 1u) */

/** \brief Unlock the mutex */
nano_os_error_t NanoOsMutex::unlock()
{
    return NANO_OS_MUTEX_Unlock(&m_mutex);
}

#if (NANO_OS_MUTEX_SETNAME_ENABLED == 1u)

/** \brief Associate a name to the mutex */
nano_os_error_t NanoOsMutex::setName(const char* const name)
{
    return NANO_OS_MUTEX_SetName(&m_mutex, name);
}

#endif /* (NANO_OS_MUTEX_SETNAME_ENABLED == 1u) */


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_MUTEX_ENABLED == 1u)
