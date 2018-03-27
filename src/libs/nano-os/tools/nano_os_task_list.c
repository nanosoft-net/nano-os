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


#include "nano_os_task_list.h"


/** \brief Add a task at the end of a task list */
nano_os_error_t NANO_OS_TASK_LIST_AddToEnd(nano_os_task_t** task_list, nano_os_task_t* const task)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if( (task_list != NULL) && (task != NULL) )
    {
        /* Check empty list */
        if( (*task_list) == NULL )
        {
            /* Add as first element */
            (*task_list) = task;
        }
        else
        {
            /* Go to the end of the list */
            nano_os_task_t* current = (*task_list);
            while( current->next != NULL )
            {
                current = current->next;
            }

            /* Add the task to the list */
            current->next = task;
        }
        task->next = NULL;

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

/** \brief Add a task by ordering the list from high priority task to low priority task */
nano_os_error_t NANO_OS_TASK_LIST_AddHighPriority(nano_os_task_t** task_list, nano_os_task_t* const task)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if( (task_list != NULL) && (task != NULL) )
    {
        /* Check empty list */
        if( (*task_list) == NULL )
        {
            /* Add as first element */
            (*task_list) = task;
        }
        else
        {
            /* Look for a lower priority task */
            nano_os_task_t* current = (*task_list);
            nano_os_task_t* previous = NULL;
            while( (current != NULL) && (current->priority >= task->priority) )
            {
                previous = current;
                current = current->next;
            }

            /* Add the task to the list */
            if( previous != NULL )
            {
                previous->next = task;
                task->next = current;
            }
            else
            {
                task->next = current;
                (*task_list) = task;
            }
        }

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

/** \brief Remove a task from of a task list */
nano_os_error_t NANO_OS_TASK_LIST_Remove(nano_os_task_t** task_list, nano_os_task_t* const task)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((task_list != NULL) && (task != NULL))
    {
        /* Look for the task in the list */
        nano_os_task_t* previous = NULL;
        nano_os_task_t* current = (*task_list);
        while ((current != task) && (current != NULL))
        {
            previous = current;
            current = current->next;
        }

        /* Check if task has been found */
        if (current != NULL)
        {
            /* Remove the task from the list */
            if (previous != NULL)
            {
                previous->next = current->next;
            }
            else
            {
                (*task_list) = current->next;
            }
            current->next = NULL;

            ret = NOS_ERR_SUCCESS;
        }
    }

    return ret;
}

/** \brief Get the first task from of a task list */
nano_os_task_t* NANO_OS_TASK_LIST_PopTask(nano_os_task_t** task_list)
{
    nano_os_task_t* ret = NULL;

    /* Check parameters */
    if( task_list != NULL )
    {
        /* Get first element */
        ret = (*task_list);

        /* Update task list */
        if( ret != NULL )
        {
            (*task_list) = ret->next;
            ret->next = NULL;
        }
    }

    return ret;
}


