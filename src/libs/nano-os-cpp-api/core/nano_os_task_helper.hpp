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

#ifndef NANO_OS_TASK_HELPER_HPP
#define NANO_OS_TASK_HELPER_HPP

#include "nano_os_task.hpp"
#include "nano_os_tools.h"


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Nano OS helper for task creation */
template <uint32_t STACK_SIZE>
class NanoOsTaskHelper : public NanoOsTask
{
    public:

        /** \brief Default constructor */
        NanoOsTaskHelper()
        : NanoOsTask()
        , m_stack()
        {}

        /** \brief Constructor with creation */
        NanoOsTaskHelper(const char* name, const uint8_t priority, void* const param, const TaskMethod& task_method)
        : NanoOsTask()
        , m_stack()
        {
            const nano_os_error_t ret = create(name, priority, param, task_method);
            NANO_OS_ERROR_ASSERT_RET(ret);
        }

        #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)

        /** \brief Constructor with creation */
        NanoOsTaskHelper(const char* name, const uint8_t priority, void* const param, 
                         const nano_os_port_task_init_data_t& port_data, const TaskMethod& task_method)
        : NanoOsTask()
        , m_stack()
        {
            const nano_os_error_t ret = create(name, priority, param, port_data, task_method);
            NANO_OS_ERROR_ASSERT_RET(ret);
        }

        #endif // (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)

        /** \brief Destructor */
        virtual ~NanoOsTaskHelper()
        {}


        /** \brief Create the task */
        nano_os_error_t create(const char* name, const uint8_t priority, void* const param, const TaskMethod& task_method)
        {
            nano_os_task_init_data_t task_init_data;
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            task_init_data.name = name;
            #else
            NANO_OS_UNUSED(name);
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
            task_init_data.param = param;
            task_init_data.base_priority = priority;
            task_init_data.stack_origin = m_stack;
            task_init_data.stack_size = STACK_SIZE;
            #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
            (void)MEMSET(&task_init_data.port_init_data, 0, sizeof(nano_os_port_task_init_data_t));
            #endif // (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)
            return NanoOsTask::create(task_init_data, task_method);
        }

        #if (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)

        /** \brief Create the task */
        nano_os_error_t create(const char* name, const uint8_t priority, void* const param, 
                               const nano_os_port_task_init_data_t& port_data, const TaskMethod& task_method)
        {
            nano_os_task_init_data_t task_init_data;
            #if (NANO_OS_TASK_NAME_ENABLED == 1u)
            task_init_data.name = param;
            #else
            NANO_OS_UNUSED(name);
            #endif /* (NANO_OS_TASK_NAME_ENABLED == 1u) */
            task_init_data.param = param;
            task_init_data.base_priority = priority;
            task_init_data.stack_origin = m_stack;
            task_init_data.stack_size = STACK_SIZE;
            (void)MEMCPY(&task_init_data.port_init_data, &port_data, sizeof(nano_os_port_task_init_data_t));
            return NanoOsTask::create(task_init_data, task_method);
        }

        #endif // (NANO_OS_PORT_CONTAINS_TASK_DATA == 1u)

    private:

        /** \brief Copy constructor => forbidden */
        NanoOsTaskHelper(const NanoOsTaskHelper& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Task stack */
        nano_os_stack_t m_stack[STACK_SIZE];
};

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // NANO_OS_TASK_HELPER_HPP
