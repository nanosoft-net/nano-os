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

#include "nano_os.h"
#include "nano_os_data.h"
#include "nano_os_port.h"
#include "nano_os_interrupt.h"
#include "nano_os_port_virtual_interrupt.h"
#include "nano_os_scheduler.h"


/** \brief Windows task to execute Nano OS tasks */
static DWORD NANO_OS_PORT_TaskStart(LPVOID param);

/** \brief Windows task to execute tick timer */
static DWORD NANO_OS_PORT_TickTimerTask(LPVOID param);




/** \brief Port specific initialization */
nano_os_error_t NANO_OS_PORT_Init(nano_os_port_init_data_t* const port_init_data)
{
    nano_os_error_t ret = NOS_ERR_FAILURE;
    NANO_OS_UNUSED(port_init_data);

    /* Change priority class of the process to real-time */
    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

    /* Change priority of the urrent thread to real-time */
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    /* Initialize the virtual interrupt module */
    ret = NANO_OS_PORT_VIRTUAL_INTERRUPT_Init();
    if (ret == NOS_ERR_SUCCESS)
    {
        /* Create a task to emulate tick interrupt */
        g_nano_os.port_data.tick_timer_task = CreateThread(NULL, 0, NANO_OS_CAST(LPTHREAD_START_ROUTINE, NANO_OS_PORT_TickTimerTask), NULL, 0, NULL);
        if (g_nano_os.port_data.tick_timer_task != NULL)
        {
            InitializeCriticalSection(&g_nano_os.port_data.scheduler_mutex);
            ret = NOS_ERR_SUCCESS;
        }
    }

    #if ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u))
    /* Get the frequency of the high performance counter */
    QueryPerformanceFrequency(&g_nano_os.port_data.hpc_frequency);
    #endif /* ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)) */

    return ret;
}


/** \brief Get the port version */
nano_os_error_t NANO_OS_PORT_GetVersion(nano_os_version_t* const port_version)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (port_version != NULL)
    {
        /* Fill Port version */
        port_version->major = 1;
        port_version->minor = 0;

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}


/** \brief Port specific initialization of the task context */
nano_os_error_t NANO_OS_PORT_InitTask(nano_os_task_t* const task, const nano_os_task_init_data_t* const task_init_data)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((task != NULL) &&
        (task_init_data != NULL) &&
        (task_init_data->stack_size >= NANO_OS_PORT_MIN_STACK_SIZE) &&
        (task_init_data->task_func != NULL))
    {
        ret = NOS_ERR_FAILURE;

        /* Create the event which allows the tasks to run */
        task->port_data.run_event = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (task->port_data.run_event != NULL)
        {
            /* Save the parameters */
            task->port_data.task_func = task_init_data->task_func;
            task->port_data.param = task_init_data->param;

            /* Create the windows task */
            task->port_data.win_handle = CreateThread(NULL, 0, NANO_OS_CAST(LPTHREAD_START_ROUTINE, NANO_OS_PORT_TaskStart), task, 0, NULL);
            if (task->port_data.win_handle != NULL)
            {
                ret = NOS_ERR_SUCCESS;
            }
        }
    }

    return ret;
}


#if ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u))

/** \brief Get the current timestamp in µs */
uint32_t NANO_OS_PORT_GetTimestampInUs(void)
{
    uint32_t us_timestamp = GetTickCount() * 1000u;
    LARGE_INTEGER current_time;

    /* Get high performance counter value */
    QueryPerformanceCounter(&current_time);

    /* Convert to microseconds */
    current_time.QuadPart *= 1000000;
    current_time.QuadPart /= g_nano_os.port_data.hpc_frequency.QuadPart;
    us_timestamp = NANO_OS_CAST(uint32_t, current_time.QuadPart);

    return us_timestamp;
}

#endif /* ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)) */


/** \brief Port specific first task context switch */
nano_os_error_t NANO_OS_PORT_FirstContextSwitch(void)
{
    /* Execute a context switch to start the first task */
    NANO_OS_PORT_ContextSwitch();

    /* This call shall not return on success */
    SuspendThread(GetCurrentThread());

    return NOS_ERR_FAILURE;
}

/** \brief Port specific task level context switch */
void NANO_OS_PORT_ContextSwitch(void)
{
    /* Protect context switchs */
    EnterCriticalSection(&g_nano_os.port_data.scheduler_mutex);

    /* Allow next running task to execute */
    nano_os_task_t* current_task = g_nano_os.current_task;
    g_nano_os.current_task = g_nano_os.next_running_task;
    if (g_nano_os.current_task->port_data.suspent)
    {
        g_nano_os.current_task->port_data.suspent = false;
        NANO_OS_PORT_ENABLE_INTERRUPTS();
        ResumeThread(g_nano_os.current_task->port_data.win_handle);
    }
    else
    {
        SetEvent(g_nano_os.current_task->port_data.run_event);
    }

    /* End of context switch protection */
    LeaveCriticalSection(&g_nano_os.port_data.scheduler_mutex);

    /* Wait for the next scheduling of the task on the run event */
    if (current_task != NULL)
    {
        WaitForSingleObject(current_task->port_data.run_event, INFINITE);
    }
}

/** \brief Port specific interrupt level context switch */
void NANO_OS_PORT_ContextSwitchFromIsr(void)
{
    /* Allow next running task to execute */
    g_nano_os.current_task = g_nano_os.next_running_task;
}

/** \brief Atomic increment of a 32bits variable */
void NANO_OS_PORT_AtomicInc32(uint32_t* const var)
{
    (void)InterlockedIncrement(NANO_OS_CAST(LONG*, var));
}

/** \brief Atomic decrement of a 32bits variable */
void NANO_OS_PORT_AtomicDec32(uint32_t* const var)
{
    (void)InterlockedDecrement(NANO_OS_CAST(LONG*, var));
}


/** \brief Windows task to execute Nano OS tasks */
static DWORD NANO_OS_PORT_TaskStart(LPVOID param)
{
    /* Get the task */
    nano_os_task_t* const task = NANO_OS_CAST(nano_os_task_t*, param);

    /* Change priority of the current thread to real-time */
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    /* Wait for the first scheduling of the task on the run event */
    WaitForSingleObject(task->port_data.run_event, INFINITE);

    /* Enable interrupts */
    NANO_OS_PORT_ENABLE_INTERRUPTS();

    /* Start the task */
    NANO_OS_TASK_Start(task, task->port_data.task_func, task->port_data.param);

    return 0;
}

/** \brief Windows task to execute tick timer */
static DWORD NANO_OS_PORT_TickTimerTask(LPVOID param)
{
    NANO_OS_UNUSED(param);
    const uint64_t period = 1000u / NANO_OS_TICK_RATE_HZ;

    /* Change priority of the current thread to real-time */
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    /* Task loop */
    while(true)
    {
        /* Wait for the next tick */
        const uint64_t next_time = GetTickCount64() + period;
        while(GetTickCount64() <= next_time)
        {
            Sleep(1);
        }

        /* OS tick interrupt handler */
        if (g_nano_os.current_task != NULL)
        {
            NANO_OS_PORT_VIRTUAL_INTERRUPT_Trigger(NANO_OS_TickInterrupt);
        }
    }

    return 0;
}
