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

#include "nano_os_task.hpp"


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


/** \brief Default constructor */
NanoOsTask::NanoOsTask()
: m_task()
, m_param(NULL)
, m_task_method()
{
}

/** \brief Constructor with creation */
NanoOsTask::NanoOsTask(const nano_os_task_init_data_t& task_init_data)
: m_task()
, m_param(NULL)
, m_task_method()
{
    const nano_os_error_t ret = create(task_init_data);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Constructor with creation */
NanoOsTask::NanoOsTask(nano_os_task_init_data_t& task_init_data, const TaskMethod& task_method)
: m_task()
, m_param(NULL)
, m_task_method()
{
    const nano_os_error_t ret = create(task_init_data, task_method);
    NANO_OS_ERROR_ASSERT_RET(ret);
}

/** \brief Destructor */
NanoOsTask::~NanoOsTask()
{
    // Check the task state
    NANO_OS_ERROR_ASSERT((m_task.state == NOS_TS_DEAD), NOS_ERR_FAILURE);
}

/** \brief Create the task */
nano_os_error_t NanoOsTask::create(const nano_os_task_init_data_t& task_init_data)
{
    return NANO_OS_TASK_Create(&m_task, &task_init_data);
}

/** \brief Create the task */
nano_os_error_t NanoOsTask::create(nano_os_task_init_data_t& task_init_data, const TaskMethod& task_method)
{
    // Save the task parameter and replace it by the task object instance
    m_param = task_init_data.param;
    task_init_data.param = this;

    // Save the task runner and replace the startup function
    m_task_method = task_method;
    task_init_data.task_func = &NanoOsTask::taskStart;

    // Create the task
    return NANO_OS_TASK_Create(&m_task, &task_init_data);
}

#if (NANO_OS_TASK_SLEEP_ENABLED == 1u)

/** \brief Suspend the current task for fixed number of ticks */
nano_os_error_t NanoOsTask::sleep(const uint32_t tick_count)
{
    return NANO_OS_TASK_Sleep(tick_count);
}

#endif /* (NANO_OS_TASK_SLEEP_ENABLED == 1u) */

#if (NANO_OS_TASK_JOIN_ENABLED == 1u)

/** \brief Wait for the end of the task for fixed number of ticks */
nano_os_error_t NanoOsTask::join(void** return_value, const uint32_t tick_count)
{
    return NANO_OS_TASK_Join(&m_task, return_value, tick_count);
}

#endif /* (NANO_OS_TASK_JOIN_ENABLED == 1u) */


/** \brief Startup function for the tasks */
void* NanoOsTask::taskStart(void* const param)
{
    // Check parameter
    NANO_OS_ERROR_ASSERT((param != NULL), NOS_ERR_FAILURE);

    // Get the task to run
    NanoOsTask* const task = reinterpret_cast<NanoOsTask*>(param);
    NANO_OS_ERROR_ASSERT(!(task->m_task_method.isNull()), NOS_ERR_FAILURE);

    // Run the task
    return task->m_task_method.invoke(task->m_param);
}


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
