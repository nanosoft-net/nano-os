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

#ifndef NANO_OS_COND_VAR_HPP
#define NANO_OS_COND_VAR_HPP

#include "nano_os_cond_var.h"

#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_COND_VAR_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

// Pre-declarations
class NanoOsMutex;

/** \brief Nano OS condition variable */
class NanoOsCondVar
{
    public:

        /** \brief Default constructor */
        NanoOsCondVar();

        /** \brief Constructor with creation */
        NanoOsCondVar(const nano_os_queuing_type_t queuing_type);

        /** \brief Destructor */
        ~NanoOsCondVar();


        /** \brief Create the condition variable */
        nano_os_error_t create(const nano_os_queuing_type_t queuing_type);

        /** \brief Wait for the condition variable */
        nano_os_error_t wait(NanoOsMutex& mutex, const uint32_t tick_count);

        /** \brief Signal the condition variable */
        nano_os_error_t signal(const bool broadcast);

        #if (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u)

        /** \brief Signal the condition variable from an interrupt handler */
        nano_os_error_t signalFromIsr(const bool broadcast);

        #endif /* (NANO_OS_COND_VAR_SIGNALFROMISR_ENABLED == 1u) */

        #if (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u)

        /** \brief Associate a name to the condition variable */
        nano_os_error_t setName(const char* const name);

        #endif /* (NANO_OS_COND_VAR_SETNAME_ENABLED == 1u) */

    private:

        /** \brief Copy constructor => forbidden */
        NanoOsCondVar(const NanoOsCondVar& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying condition variable object */
        nano_os_cond_var_t m_cond_var;
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_COND_VAR_ENABLED == 1u)

#endif // NANO_OS_COND_VAR_HPP
