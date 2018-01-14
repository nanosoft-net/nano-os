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

#ifndef NANO_OS_TIMER_HPP
#define NANO_OS_TIMER_HPP

#include "nano_os_timer.h"
#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_TIMER_ENABLED == 1u)

#include "nano_os_delegate.hpp"

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS timer */
class NanoOsTimer
{
    public:

        /** \brief Delegate type for timer callback method */
        typedef NanoOsDelegate2<void, NanoOsTimer&, void* const> TimerMethod;
    

        /** \brief Default constructor */
        NanoOsTimer();

        /** \brief Constructor with creation */
        NanoOsTimer(const TimerMethod& callback, void* const user_data);

        /** \brief Destructor */
        ~NanoOsTimer();



        /** \brief Create the timer */
        nano_os_error_t create(const TimerMethod& callback, void* const user_data);

        /** \brief Start the timer */
        nano_os_error_t start(const uint32_t first_due_time, const uint32_t period);

        /** \brief Stop the timer */
        nano_os_error_t stop();

        /** \brief Check if the timer has elapsed */
        nano_os_error_t hasElapsed(bool& has_elapsed);


    private:

        /** \brief Copy constructor => forbidden */
        NanoOsTimer(const NanoOsTimer& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying waitable timer object */
        nano_os_timer_t m_timer;

        /** \brief Timer callback method */
        TimerMethod m_callback;

        /** \brief User data */
        void* m_user_data;


        /** \brief Generic timer callback */
        static void timerCallback(nano_os_timer_t* const timer, void* const user_data);
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_TIMER_ENABLED == 1u)

#endif // NANO_OS_TIMER_HPP
