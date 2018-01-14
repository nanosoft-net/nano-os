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

#include "../sync_objects/nano_os_timer.hpp"

// Check if module is enabled
#if (NANO_OS_TIMER_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsTimer::NanoOsTimer()
: m_timer()
, m_callback()
, m_user_data(NULL)
{
}

/** \brief Constructor with creation */
NanoOsTimer::NanoOsTimer(const TimerMethod& callback, void* const user_data)
: m_timer()
, m_callback()
, m_user_data(NULL)
{
    const nano_os_error_t ret = create(callback, user_data);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsTimer::~NanoOsTimer()
{
    #if (NANO_OS_TIMER_DESTROY_ENABLED == 1u)

    // Destroy the timer
    const nano_os_error_t ret = NANO_OS_TIMER_Destroy(&m_timer);
    NANO_OS_ERROR_ASSERT_RET(ret);

    #else

    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);

    #endif /* (NANO_OS_TIMER_DESTROY_ENABLED == 1u) */
}

/** \brief Create the timer */
nano_os_error_t NanoOsTimer::create(const TimerMethod& callback, void* const user_data)
{
    // Save callback and user data
    m_callback = callback;
    m_user_data = user_data;
    return NANO_OS_TIMER_Create(&m_timer, &NanoOsTimer::timerCallback, this);
}

/** \brief Start the timer */
nano_os_error_t NanoOsTimer::start(const uint32_t first_due_time, const uint32_t period)
{
    return NANO_OS_TIMER_Start(&m_timer, first_due_time, period);
}

/** \brief Stop the timer */
nano_os_error_t NanoOsTimer::stop()
{
    return NANO_OS_TIMER_Stop(&m_timer);
}

/** \brief Check if the timer has elapsed */
nano_os_error_t NanoOsTimer::hasElapsed(bool& has_elapsed)
{
    return NANO_OS_TIMER_HasElapsed(&m_timer, &has_elapsed);
}



/** \brief Generic timer callback */
void NanoOsTimer::timerCallback(nano_os_timer_t* const timer, void* const user_data)
{
    NANO_OS_UNUSED(timer);

    // Check parameter
    NANO_OS_ERROR_ASSERT((user_data != NULL), NOS_ERR_FAILURE);

    // Get the corresponding timer object
    NanoOsTimer* const timer_obj = reinterpret_cast<NanoOsTimer*>(user_data);
    NANO_OS_ERROR_ASSERT(!(timer_obj->m_callback.isNull()), NOS_ERR_FAILURE);

    // Call the callback
    return timer_obj->m_callback.invoke((*timer_obj), timer_obj->m_user_data);
}


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_TIMER_ENABLED == 1u)
