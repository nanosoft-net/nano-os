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

#ifndef NANO_OS_SCHEDULER_H
#define NANO_OS_SCHEDULER_H

#include "nano_os_types.h"
#include "nano_os_task.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief First scheduling which will start Nano OS */
nano_os_error_t NANO_OS_SCHEDULER_FirstScheduling(void);

/** \brief Schedule the next running task and do a context switch if needed */
void NANO_OS_SCHEDULER_Schedule(const bool from_isr);

/** \brief Register a task to the scheduler */
nano_os_error_t NANO_OS_SCHEDULER_RegisterTask(nano_os_task_t* const task);

/** \brief Unregister a task from the scheduler */
nano_os_error_t NANO_OS_SCHEDULER_UnregisterTask(nano_os_task_t* const task);

/** \brief Set a task into ready state */
nano_os_error_t NANO_OS_SCHEDULER_SetTaskReady(nano_os_task_t* const task);

/** \brief Set a task into pending state */
nano_os_error_t NANO_OS_SCHEDULER_SetTaskPending(nano_os_task_t* const task, const uint32_t tick_count);

/** \brief Go through all the suspent tasks to check their wake-up timeout */
nano_os_error_t NANO_OS_SCHEDULER_HandleSuspentTasks(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_SCHEDULER_H */
