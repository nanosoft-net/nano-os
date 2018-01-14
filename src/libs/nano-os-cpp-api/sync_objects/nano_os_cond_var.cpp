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

#include "../sync_objects/nano_os_cond_var.hpp"

// Check if module is enabled
#if (NANO_OS_COND_VAR_ENABLED == 1u)

#include "../sync_objects/nano_os_mutex.hpp"

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsCondVar::NanoOsCondVar()
: m_cond_var()
{
}

/** \brief Constructor with creation */
NanoOsCondVar::NanoOsCondVar(const nano_os_queuing_type_t queuing_type)
: m_cond_var()
{
    const nano_os_error_t ret = create(queuing_type);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsCondVar::~NanoOsCondVar()
{
    #if (NANO_OS_COND_VAR_DESTROY_ENABLED == 1u)

    // Destroy the condition variable
    const nano_os_error_t ret = NANO_OS_COND_VAR_Destroy(&m_cond_var);
    NANO_OS_ERROR_ASSERT_RET(ret);

    #else

    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);

    #endif /* (NANO_OS_COND_VAR_DESTROY_ENABLED == 1u) */
}


/** \brief Create the condition variable */
nano_os_error_t NanoOsCondVar::create(const nano_os_queuing_type_t queuing_type)
{
    return NANO_OS_COND_VAR_Create(&m_cond_var, queuing_type);
}

/** \brief Wait for the condition variable */
nano_os_error_t NanoOsCondVar::wait(NanoOsMutex& mutex, const uint32_t tick_count)
{
    return NANO_OS_COND_VAR_Wait(&m_cond_var, &mutex.m_mutex, tick_count);
}

/** \brief Signal the condition variable */
nano_os_error_t NanoOsCondVar::signal(const bool broadcast)
{
    return NANO_OS_COND_VAR_Signal(&m_cond_var, broadcast);
}

#if (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u)

/** \brief Signal the condition variable from an interrupt handler */
nano_os_error_t NanoOsCondVar::signalFromIsr(const bool broadcast)
{
    return NANO_OS_COND_VAR_SignalFromIsr(&m_cond_var, broadcast);
}

#endif /* (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u) */

#if (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u)

/** \brief Associate a name to the condition variable */
nano_os_error_t NanoOsCondVar::setName(const char* const name)
{
    return NANO_OS_COND_VAR_SetName(&m_cond_var, name);
}

#endif /* (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u) */



#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_COND_VAR_ENABLED == 1u)
