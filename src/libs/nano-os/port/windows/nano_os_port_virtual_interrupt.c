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

#include "nano_os_data.h"
#include "nano_os_port_virtual_interrupt.h"

/** \brief Task which executes the virtual interrupt handlers */
static DWORD NANO_OS_PORT_VIRTUAL_INTERRUPT_Task(LPVOID unused);


/** \brief Initialize the virtual interrupt module */
nano_os_error_t NANO_OS_PORT_VIRTUAL_INTERRUPT_Init(void)
{
    nano_os_error_t ret = NOS_ERR_FAILURE;

    /* Initialize synchronization objects */
    InitializeCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
    InitializeConditionVariable(&g_nano_os.port_data.virtual_int_enable_condvar);
    g_nano_os.port_data.enabled = false;

    InitializeCriticalSection(&g_nano_os.port_data.virtual_int_trigger_mutex);
    InitializeConditionVariable(&g_nano_os.port_data.virtual_int_trigger_condvar);
    g_nano_os.port_data.virtual_int_handler = NULL;

    /* Create the worker task */
    g_nano_os.port_data.virtual_int_task = CreateThread(NULL, 0, NANO_OS_CAST(LPTHREAD_START_ROUTINE, NANO_OS_PORT_VIRTUAL_INTERRUPT_Task), NULL, 0, NULL);
    if (g_nano_os.port_data.virtual_int_task != NULL)
    {
        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/** \brief Triggers a virtual interrupt */
nano_os_error_t NANO_OS_PORT_VIRTUAL_INTERRUPT_Trigger(const fp_nano_os_virtual_interrupt_handler_t handler)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (handler != NULL)
    {
        EnterCriticalSection(&g_nano_os.port_data.virtual_int_trigger_mutex);
        while (g_nano_os.port_data.virtual_int_handler != NULL)
        {
            SleepConditionVariableCS(&g_nano_os.port_data.virtual_int_trigger_condvar, &g_nano_os.port_data.virtual_int_trigger_mutex, INFINITE);
        }
        g_nano_os.port_data.virtual_int_handler = handler;
        WakeAllConditionVariable(&g_nano_os.port_data.virtual_int_trigger_condvar);
        LeaveCriticalSection(&g_nano_os.port_data.virtual_int_trigger_mutex);

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/** \brief Enable the virtual interrupts */
void NANO_OS_PORT_VIRTUAL_INTERRUPT_Enable(void)
{
    EnterCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
    g_nano_os.port_data.enabled = true;
    WakeAllConditionVariable(&g_nano_os.port_data.virtual_int_enable_condvar);
    LeaveCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
}

/** \brief Disable the virtual interrupts */
void NANO_OS_PORT_VIRTUAL_INTERRUPT_Disable(void)
{
    EnterCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
    g_nano_os.port_data.enabled = false;
    LeaveCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
}

/** \brief Disable interrupts and return previous interrupt status register */
nano_os_int_status_reg_t NANO_OS_PORT_VIRTUAL_INTERRUPT_SaveInterruptStatus()
{
    nano_os_int_status_reg_t ret;

    EnterCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
    ret = g_nano_os.port_data.enabled;
    LeaveCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);

    return ret;
}

/** \brief Restore the interrupt status register passed in parameter */
void NANO_OS_PORT_VIRTUAL_INTERRUPT_RestoreInterruptStatus(const nano_os_int_status_reg_t int_status_reg)
{
    EnterCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
    g_nano_os.port_data.enabled = int_status_reg;
    if (g_nano_os.port_data.enabled)
    {
        WakeAllConditionVariable(&g_nano_os.port_data.virtual_int_enable_condvar);
    }
    LeaveCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
}

/** \brief Task which executes the virtual interrupt handlers */
static DWORD NANO_OS_PORT_VIRTUAL_INTERRUPT_Task(LPVOID unused)
{
    /* Change priority of the current thread to real-time */
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    /* Task loop */
    while(true)
    {
        nano_os_task_t* current_task = NULL;

        /* Wait for an interrupt handler to execute */
        EnterCriticalSection(&g_nano_os.port_data.virtual_int_trigger_mutex);
        while (g_nano_os.port_data.virtual_int_handler == NULL)
        {
            SleepConditionVariableCS(&g_nano_os.port_data.virtual_int_trigger_condvar, &g_nano_os.port_data.virtual_int_trigger_mutex, INFINITE);
        }

        /* Check interrupt disable state */
        EnterCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);
        while (!g_nano_os.port_data.enabled)
        {
            SleepConditionVariableCS(&g_nano_os.port_data.virtual_int_enable_condvar, &g_nano_os.port_data.virtual_int_enable_mutex, INFINITE);
        }

        /* Suspend current running thread */
        current_task = g_nano_os.current_task;
        SuspendThread(current_task->port_data.win_handle);
        current_task->port_data.suspent = true;

        LeaveCriticalSection(&g_nano_os.port_data.virtual_int_enable_mutex);


        /* Signal OS that we are in an interrupt handler */
        NANO_OS_INTERRUPT_Enter();

        /* Execute interrupt handler */
        g_nano_os.port_data.virtual_int_handler();

        /* Signal OS that we exit the interrupt handler */
        NANO_OS_INTERRUPT_Exit();

        /* Resume current running thread if it has not been scheduled away */
        if (current_task == g_nano_os.current_task)
        {
            current_task->port_data.suspent = false;
            ResumeThread(g_nano_os.current_task->port_data.win_handle);
        }
        else
        {
            /* Perform context switch */
            if (g_nano_os.current_task->port_data.suspent)
            {
                g_nano_os.current_task->port_data.suspent = false;
                ResumeThread(g_nano_os.current_task->port_data.win_handle);
            }
            else
            {
                SetEvent(g_nano_os.current_task->port_data.run_event);
            }
        }

        /* Allow another virtual interrupt */
        g_nano_os.port_data.virtual_int_handler = NULL;
        WakeAllConditionVariable(&g_nano_os.port_data.virtual_int_trigger_condvar);
        LeaveCriticalSection(&g_nano_os.port_data.virtual_int_trigger_mutex);
    }

    return 0;
}
