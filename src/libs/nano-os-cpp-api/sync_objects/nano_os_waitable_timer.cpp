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

#include "../sync_objects/nano_os_waitable_timer.hpp"

// Check if module is enabled
#if (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsWaitableTimer::NanoOsWaitableTimer()
: m_waitable_timer()
{
}

/** \brief Constructor with creation */
NanoOsWaitableTimer::NanoOsWaitableTimer(const nano_os_queuing_type_t queuing_type)
: m_waitable_timer()
{
    const nano_os_error_t ret = create(queuing_type);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsWaitableTimer::~NanoOsWaitableTimer()
{
    #if (NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u)

    // Destroy the waitable timer
    (void)stop();
    const nano_os_error_t ret = NANO_OS_WAITABLE_TIMER_Destroy(&m_waitable_timer);
    NANO_OS_ERROR_ASSERT_RET(ret);

    #else

    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);

    #endif /* (NANO_OS_WAITABLE_TIMER_DESTROY_ENABLED == 1u) */
}

/** \brief Create the waitable timer */
nano_os_error_t NanoOsWaitableTimer::create(const nano_os_queuing_type_t queuing_type)
{
    return NANO_OS_WAITABLE_TIMER_Create(&m_waitable_timer, queuing_type);
}

/** \brief Start the waitable timer */
nano_os_error_t NanoOsWaitableTimer::start(const uint32_t first_due_time, const uint32_t period)
{
    return NANO_OS_WAITABLE_TIMER_Start(&m_waitable_timer, first_due_time, period);
}

/** \brief Stop the waitable timer */
nano_os_error_t NanoOsWaitableTimer::stop()
{
    return NANO_OS_WAITABLE_TIMER_Stop(&m_waitable_timer);
}

/** \brief Wait for the waitable timer */
nano_os_error_t NanoOsWaitableTimer::wait(const uint32_t tick_count)
{
    return NANO_OS_WAITABLE_TIMER_Wait(&m_waitable_timer, tick_count);
}

#if (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u)

/** \brief Associate a name to the waitable timer */
nano_os_error_t NanoOsWaitableTimer::setName(const char* const name)
{
    return NANO_OS_WAITABLE_TIMER_SetName(&m_waitable_timer, name);
}

#endif /* (NANO_OS_WAITABLE_TIMER_SETNAME_ENABLED == 1u) */

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_WAITABLE_TIMER_ENABLED == 1u)
