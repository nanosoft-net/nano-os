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

#ifndef NANO_OS_HPP
#define NANO_OS_HPP

#include "nano_os.h"
#include "nano_os_cpp_api_cfg.h"

#if (NANO_OS_HEAP_ENABLED == 1u)
#include "nano_os_heap.hpp"
#endif /* (NANO_OS_HEAP_ENABLED == 1u) */

#if (NANO_OS_CONSOLE_ENABLED == 1u)
#include "nano_os_console.hpp"
#endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Represent Nano OS */
class NanoOs
{
	public:

		/** \brief Default constructor */
        NanoOs();

        /** \brief Constructor with Nano OS init */
        NanoOs(const bool initialize);

        /** \brief Destructor */
        ~NanoOs();

		/** \brief Get the unique instance */
        static NanoOs& getInstance();

        /** \brief Initialize Nano OS */
        nano_os_error_t init();

        /** \brief Start Nano OS (this function returns only in case of error at during first scheduling) */
        nano_os_error_t start();

        /** \brief Get the current tick count since Nano OS is started */
        nano_os_error_t getTickCount(uint32_t& tick_count);

        /** \brief Get Nano OS version and the current used port version */
        nano_os_error_t getVersion(nano_os_version_t& version, nano_os_version_t& port_version);


        #if (NANO_OS_HEAP_ENABLED == 1u)
        NanoOsHeap& getHeapModule() { return m_heap_module; }
        #endif /* (NANO_OS_HEAP_ENABLED == 1u) */

        #if (NANO_OS_CONSOLE_ENABLED == 1u)
        NanoOsConsole& getConsoleModule() { return m_console_module; }
        #endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */

	private:

		/** \brief Copy constructor => forbidden */
        NanoOs(const NanoOs& copy) { NANO_OS_UNUSED(copy); }


        #if (NANO_OS_HEAP_ENABLED == 1u)
        NanoOsHeap m_heap_module;
        #endif /* (NANO_OS_HEAP_ENABLED == 1u) */

        #if (NANO_OS_CONSOLE_ENABLED == 1u)
        NanoOsConsole m_console_module;
        #endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */

		/** \brief Unique instance */
		static NanoOs* m_singleton;
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // NANO_OS_HPP
