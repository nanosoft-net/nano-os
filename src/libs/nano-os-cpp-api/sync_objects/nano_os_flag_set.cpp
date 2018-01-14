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

#include "../sync_objects/nano_os_flag_set.hpp"

// Check if module is enabled
#if (NANO_OS_FLAG_SET_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsFlagSet::NanoOsFlagSet()
: m_flag_set()
{
}

/** \brief Constructor with creation */
NanoOsFlagSet::NanoOsFlagSet(const uint32_t initial_mask,
                             const nano_os_queuing_type_t queuing_type)
: m_flag_set()
{
    const nano_os_error_t ret = create(initial_mask, queuing_type);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsFlagSet::~NanoOsFlagSet()
{
    #if (NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u)

    // Destroy the flag set
    const nano_os_error_t ret = NANO_OS_FLAG_SET_Destroy(&m_flag_set);
    NANO_OS_ERROR_ASSERT_RET(ret);

    #else

    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);

    #endif /* (NANO_OS_FLAG_SET_DESTROY_ENABLED == 1u) */
}

/** \brief Create the flag set */
nano_os_error_t NanoOsFlagSet::create(const uint32_t initial_mask,
                                      const nano_os_queuing_type_t queuing_type)
{
    return NANO_OS_FLAG_SET_Create(&m_flag_set, initial_mask, queuing_type);
}

/** \brief Wait for a specific flag mask in the flag set */
nano_os_error_t NanoOsFlagSet::wait(const uint32_t wait_mask,
                                    const bool wait_all, const bool reset_flags,
                                    uint32_t* const wakeup_flags, const uint32_t tick_count)
{
    return NANO_OS_FLAG_SET_Wait(&m_flag_set, wait_mask, wait_all, reset_flags, wakeup_flags, tick_count);
}

/** \brief Set a flag mask in the flag set */
nano_os_error_t NanoOsFlagSet::set(const uint32_t mask)
{
    return NANO_OS_FLAG_SET_Set(&m_flag_set, mask);
}

#if (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u)

/** \brief Set a flag mask in the flag set from an interrupt handler */
nano_os_error_t NanoOsFlagSet::setFromIsr(const uint32_t mask)
{
    return NANO_OS_FLAG_SET_SetFromIsr(&m_flag_set, mask);
}

#endif /* (NANO_OS_FLAG_SET_SETFROMISR_ENABLED == 1u) */

#if (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u)

/** \brief Clear a flag mask in the flag set */
nano_os_error_t NanoOsFlagSet::clear(const uint32_t mask)
{
    return NANO_OS_FLAG_SET_Clear(&m_flag_set, mask);
}

#if (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u)

/** \brief Clear a flag mask in the flag set from an interrupt handler */
nano_os_error_t NanoOsFlagSet::clearFromIsr(const uint32_t mask)
{
    return NANO_OS_FLAG_SET_ClearFromIsr(&m_flag_set, mask);
}

#endif /* (NANO_OS_FLAG_SET_CLEARFROMISR_ENABLED == 1u) */

#endif /* (NANO_OS_FLAG_SET_CLEAR_ENABLED == 1u) */

#if (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u)

/** \brief Associate a name to the flag set */
nano_os_error_t NanoOsFlagSet::setName(const char* const name)
{
    return NANO_OS_FLAG_SET_SetName(&m_flag_set, name);
}

#endif /* (NANO_OS_FLAG_SET_SETNAME_ENABLED == 1u) */

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_FLAG_SET_ENABLED == 1u)
