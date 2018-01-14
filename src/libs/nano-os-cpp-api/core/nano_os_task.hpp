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

#ifndef NANO_OS_TASK_HPP
#define NANO_OS_TASK_HPP

#include "nano_os_task.h"
#include "nano_os_cpp_api_cfg.h"
#include "nano_os_delegate.hpp"

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS task */
class NanoOsTask
{
    public:

        /** \brief Delegate type for task method */
        typedef NanoOsDelegate1<void*, void* const> TaskMethod;


        /** \brief Default constructor */
        NanoOsTask();

        /** \brief Constructor with creation */
        NanoOsTask(const nano_os_task_init_data_t& task_init_data);

        /** \brief Constructor with creation */
        NanoOsTask(nano_os_task_init_data_t& task_init_data, const TaskMethod& task_method);

        /** \brief Destructor */
        virtual ~NanoOsTask();


        /** \brief Create the task */
        nano_os_error_t create(const nano_os_task_init_data_t& task_init_data);

        /** \brief Create the task */
        nano_os_error_t create(nano_os_task_init_data_t& task_init_data, const TaskMethod& task_method);

        #if (NANO_OS_TASK_SLEEP_ENABLED == 1u)

        /** \brief Suspend the current task for fixed number of ticks */
        static nano_os_error_t sleep(const uint32_t tick_count);

        #endif /* (NANO_OS_TASK_SLEEP_ENABLED == 1u) */

        #if (NANO_OS_TASK_JOIN_ENABLED == 1u)

        /** \brief Wait for the end of the task for fixed number of ticks */
        nano_os_error_t join(void** return_value, const uint32_t tick_count);

        #endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */


    private:

        /** \brief Copy constructor => forbidden */
        NanoOsTask(const NanoOsTask& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Underlying task object */
        nano_os_task_t m_task;

        /** \brief Task parameter */
        void* m_param;

        /** \brief Object/method which will run the task */
        TaskMethod m_task_method;



        /** \brief Startup function for the tasks */
        static void* taskStart(void* const param);
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // NANO_OS_TASK_HPP
