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

#ifndef NANO_OS_TASK_LIST_H
#define NANO_OS_TASK_LIST_H

#include "nano_os_types.h"
#include "nano_os_task.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Add a task at the end of a task list */
nano_os_error_t NANO_OS_TASK_LIST_AddToEnd(nano_os_task_t** task_list, nano_os_task_t* const task);

/** \brief Add a task by ordering the list from high priority task to low priority task */
nano_os_error_t NANO_OS_TASK_LIST_AddHighPriority(nano_os_task_t** task_list, nano_os_task_t* const task);

/** \brief Remove a task from of a task list */
nano_os_error_t NANO_OS_TASK_LIST_Remove(nano_os_task_t** task_list, nano_os_task_t* const task);

/** \brief Get the first task from of a task list */
nano_os_task_t* NANO_OS_TASK_LIST_PopTask(nano_os_task_t** task_list);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_TASK_LIST_H */
