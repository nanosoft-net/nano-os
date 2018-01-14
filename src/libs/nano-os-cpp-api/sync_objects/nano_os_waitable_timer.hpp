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

#ifndef NANO_OS_WAITABLE_TIMER_HPP
#define NANO_OS_WAITABLE_TIMER_HPP

#include "nano_os_waitable_timer.h"

#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS waitable timer */
class NanoOsWaitableTimer
{
    public:

        /** \brief Default constructor */
        NanoOsWaitableTimer();

        /** \brief Constructor with creation */
        NanoOsWaitableTimer(const nano_os_queuing_type_t queuing_type);

        /** \brief Destructor */
        ~NanoOsWaitableTimer();


        /** \brief Create the waitable timer */
        nano_os_error_t create(const nano_os_queuing_type_t queuing_type);

        /** \brief Start the waitable timer */
        nano_os_error_t start(const uint32_t first_due_time, const uint32_t period);

        /** \brief Stop the waitable timer */
        nano_os_error_t stop();

        /** \brief Wait for the waitable timer */
        nano_os_error_t wait(const uint32_t tick_count);

        #if (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u)

        /** \brief Associate a name to the waitable timer */
        nano_os_error_t setName(const char* const name);

        #endif /* (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u) */


    private:

        /** \brief Copy constructor => forbidden */
        NanoOsWaitableTimer(const NanoOsWaitableTimer& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying waitable timer object */
        nano_os_waitable_timer_t m_waitable_timer;
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)

#endif // NANO_OS_WAITABLE_TIMER_HPP
