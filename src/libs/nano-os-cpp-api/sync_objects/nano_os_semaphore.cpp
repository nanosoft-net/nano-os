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

#include "../sync_objects/nano_os_semaphore.hpp"

// Check if module is enabled
#if (NANO_OS_SEMAPHORE_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsSemaphore::NanoOsSemaphore()
: m_semaphore()
{
}

/** \brief Constructor with creation */
NanoOsSemaphore::NanoOsSemaphore(const uint32_t initial_count,
                                 const uint32_t max_count, const nano_os_queuing_type_t queuing_type)
: m_semaphore()
{
    const nano_os_error_t ret = create(initial_count, max_count, queuing_type);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsSemaphore::~NanoOsSemaphore()
{
    #if (NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u)

    // Destroy the semaphore
    const nano_os_error_t ret = NANO_OS_SEMAPHORE_Destroy(&m_semaphore);
    NANO_OS_ERROR_ASSERT_RET(ret);

    #else

    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);

    #endif /* (NANO_OS_SEMAPHORE_DESTROY_ENABLED == 1u) */
}


/** \brief Create the semaphore */
nano_os_error_t NanoOsSemaphore::create(const uint32_t initial_count,
                                        const uint32_t max_count, const nano_os_queuing_type_t queuing_type)
{
    return NANO_OS_SEMAPHORE_Create(&m_semaphore, initial_count, max_count, queuing_type);
}

/** \brief Wait for a token in the semaphore */
nano_os_error_t NanoOsSemaphore::wait(const uint32_t tick_count)
{
    return NANO_OS_SEMAPHORE_Wait(&m_semaphore, tick_count);
}

/** \brief Post a token to the semaphore */
nano_os_error_t NanoOsSemaphore::post()
{
    return NANO_OS_SEMAPHORE_Post(&m_semaphore);
}

#if (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u)

/** \brief Post a token to the semaphore from an interrupt handler */
nano_os_error_t NanoOsSemaphore::postFromIsr()
{
    return NANO_OS_SEMAPHORE_PostFromIsr(&m_semaphore);
}

#endif /* (NANO_OS_SEMAPHORE_POSTFROMISR_ENABLED == 1u) */

#if (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u)

/** \brief Get the current count of the semaphore */
nano_os_error_t NanoOsSemaphore::getCount(uint32_t& count)
{
    return NANO_OS_SEMAPHORE_GetCount(&m_semaphore, &count);
}

#endif /* (NANO_OS_SEMAPHORE_GETCOUNT_ENABLED == 1u) */

#if (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u)

/** \brief Associate a name to the semaphore */
nano_os_error_t NanoOsSemaphore::setName(const char* const name)
{
    return NANO_OS_SEMAPHORE_SetName(&m_semaphore, name);
}

#endif /* (NANO_OS_SEMAPHORE_SETNAME_ENABLED == 1u) */


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_SEMAPHORE_ENABLED == 1u)
