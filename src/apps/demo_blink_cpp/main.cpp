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

#include "nano_os_cpp_api.h"
#include "bsp.h"

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

using namespace NANO_OS_CPP_API_NAMESPACE;

#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1)

/** \brief Blink demo */
class BlinkDemo
{
    public:

        /** \brief Initialize the demo */
        nano_os_error_t init()
        {
            nano_os_error_t ret;

            #if (NANO_OS_HEAP_ENABLED == 1u)
            // Initialize heap
            NanoOs& nano_os = NanoOs::getInstance();
            NanoOsHeap& nano_os_heap = nano_os.getHeapModule();
            void* heap_area;
            size_t heap_size;
            NANO_OS_BSP_GetHeapArea(&heap_area, &heap_size);
            nano_os_heap.init(heap_area, heap_size);
            #endif // (NANO_OS_HEAP_ENABLED == 1u)

            // Create the waitable timer
            ret = m_waitable_timer.create(QT_PRIORITY);
            if (ret == NOS_ERR_SUCCESS)
            {
                // Name the waitable timer
                (void)m_waitable_timer.setName("WAIT_TIMER0");

                // Create the timer
                NanoOsTimer::TimerMethod timer_callback = NanoOsTimer::TimerMethod::create<BlinkDemo, &BlinkDemo::timerCallback>(*this);
                #if (NANO_OS_HEAP_ENABLED == 1u)
                m_timer = new NanoOsTimer(timer_callback, NANO_OS_CAST(void*, 1u));
                if (m_timer != NULL)
                {
                #else
                m_timer.create(timer_callback, NANO_OS_CAST(void*, 1u));
                #endif // (NANO_OS_HEAP_ENABLED == 1u)

                    // Create the task
                    NanoOsTask::TaskMethod task_method = NanoOsTask::TaskMethod::create<BlinkDemo, &BlinkDemo::taskLoop>(*this);
                    ret = m_task.create("Blink demo task", 5u, NANO_OS_CAST(void*, 0u), task_method);
                
                #if (NANO_OS_HEAP_ENABLED == 1u)
                }
                else
                {
                    ret = NOS_ERR_FAILURE;
                }
                #endif // (NANO_OS_HEAP_ENABLED == 1u)
            }

            #if (NANO_OS_CONSOLE_ENABLED == 1u)
            // Register the console command
            if (ret == NOS_ERR_SUCCESS)
            {
                NanoOsConsole::CommandHandlerMethod demo_cmd_delegate = NanoOsConsole::CommandHandlerMethod::create<BlinkDemo, &BlinkDemo::consoleCommandHandler>(*this);
                m_command_group.addCommand("demo", "Print some information about the demo application", demo_cmd_delegate);
                
                NanoOsConsole& nano_os_console = NanoOs::getInstance().getConsoleModule();
                ret = nano_os_console.registerCommands(m_command_group);
            }
            #endif // (NANO_OS_CONSOLE_ENABLED == 1u)

            return ret;
        }

    private:

        /** \brief Main task */
        NanoOsTaskHelper<128u> m_task;

        /** \brief Waitable timer */
        NanoOsWaitableTimer m_waitable_timer;

        /** \brief Timer */
        #if (NANO_OS_HEAP_ENABLED == 1u)
        NanoOsTimer* m_timer;
        #else
        NanoOsTimer m_timer;
        #endif // (NANO_OS_HEAP_ENABLED == 1u)

        #if (NANO_OS_CONSOLE_ENABLED == 1u)

        /** \brief Console command group */
        NanoOsConsoleCmdGroup<1u> m_command_group;

        #endif // (NANO_OS_CONSOLE_ENABLED == 1u)



        /** \brief Task loop */
        void* taskLoop(void* const param)
        {
            bool led_on = true;

            // Get parameter
            const uint32_t led_id = NANO_OS_CAST(uint32_t, param);

            // Start the timer
            #if (NANO_OS_HEAP_ENABLED == 1u)
            m_timer->start(NANO_OS_MS_TO_TICKS(500u), NANO_OS_MS_TO_TICKS(500u));
            #else
            m_timer.start(NANO_OS_MS_TO_TICKS(500u), NANO_OS_MS_TO_TICKS(500u));
            #endif // (NANO_OS_HEAP_ENABLED == 1u)

            // Start the waitable timer
            m_waitable_timer.start(NANO_OS_MS_TO_TICKS(250u), NANO_OS_MS_TO_TICKS(500u));

            // Task loop
            while (true)
            {
                // Wait for the waitable timer
                m_waitable_timer.wait(0xFFFFFFFFu);

                // Update the LED state
                if (led_on)
                {
                    NANO_OS_BSP_LedOn(led_id);
                }
                else
                {
                    NANO_OS_BSP_LedOff(led_id);
                }
                led_on = !led_on;
            }

            return NULL;
        }

        /** \brief Timer callback */
        void timerCallback(NanoOsTimer& timer, void* const user_data)
        {
            static bool led_on = true;

            /* Get parameter */
            const uint32_t led_id = NANO_OS_CAST(uint32_t, user_data);

            /* Update the LED state */
            if (led_on)
            {
                NANO_OS_BSP_LedOn(led_id);
            }
            else
            {
                NANO_OS_BSP_LedOff(led_id);
            }
            led_on = !led_on;
        }


        #if (NANO_OS_CONSOLE_ENABLED == 1u)
        void consoleCommandHandler(const char* params)
        {
            NANO_OS_UNUSED(params);
            NanoOsConsole& nano_os_console = NanoOs::getInstance().getConsoleModule();
            nano_os_console.writeString("-----------------------------\r\n");
            nano_os_console.writeString("Demo blink - C++ API\r\n");
            nano_os_console.writeString("-----------------------------\r\n");
            nano_os_console.writeString("Purpose of this application is to present the C++ API features.\r\n\r\n");
        }
        #endif // (NANO_OS_CONSOLE_ENABLED == 1u)
};


/** \brief Nano OS instance */
static NanoOs nano_os;

/** \brief Blink demo */
static BlinkDemo blink_demo;



/** \brief Application entry point */
int main()
{
    nano_os_error_t ret;

    // Initialize board
    ret = NANO_OS_BSP_Init();
    if (ret == NOS_ERR_SUCCESS)
    {
        // Initialize operating system
        ret = nano_os.init();
        if (ret == NOS_ERR_SUCCESS)
        {
            // Initialize the demo
            ret = blink_demo.init();
            if (ret == NOS_ERR_SUCCESS)
            {
                // Start operating system
                ret = nano_os.start();
            }
        }
    }

    /* If we are here, an error happened during initialization */
    while(true)
    {}

    return 0;
}
