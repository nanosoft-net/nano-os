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

#include "nano_os.hpp"

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

/** \brief Unique instance */
NanoOs* NanoOs::m_singleton = NULL;

/** \brief Default constructor */
NanoOs::NanoOs()
{
    // Check singleton
    if (m_singleton == NULL)
    {
        m_singleton = this;
    }
    else
    {
        // Only one instance allowed
        NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);
    }
}

/** \brief Constructor with Nano OS init */
NanoOs::NanoOs(const bool initialize)
{
    // Check singleton
    if (m_singleton == NULL)
    {
        m_singleton = this;
        if (initialize)
        {
            const nano_os_error_t ret = init();
            NANO_OS_ERROR_ASSERT_RET(ret);
        }
    }
    else
    {
        // Only one instance allowed
        NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);
    }
}

/** \brief Destructor */
NanoOs::~NanoOs()
{
    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);
}

/** \brief Get the unique instance */
NanoOs& NanoOs::getInstance()
{
    if (m_singleton == NULL)
    {
        NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);
    }

    return (*m_singleton);
}

/** \brief Initialize Nano OS */
nano_os_error_t NanoOs::init()
{
    return NANO_OS_Init();
}

/** \brief Start Nano OS (this function returns only in case of error at during first scheduling) */
nano_os_error_t NanoOs::start()
{
    return NANO_OS_Start();
}

/** \brief Get the current tick count since Nano OS is started */
nano_os_error_t NanoOs::getTickCount(uint32_t& tick_count)
{
    return NANO_OS_GetTickCount(&tick_count);
}

/** \brief Get Nano OS version and the current used port version */
nano_os_error_t NanoOs::getVersion(nano_os_version_t& version, nano_os_version_t& port_version)
{
    return NANO_OS_GetVersion(&version, &port_version);
}


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
