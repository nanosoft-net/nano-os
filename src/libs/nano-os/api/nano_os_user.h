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

#ifndef NANO_OS_USER_H
#define NANO_OS_USER_H

#include "nano_os_types.h"
#include "nano_os_interrupt.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/********************************************************************************/
/*                               Callbacks                                      */
/********************************************************************************/


#if (NANO_OS_ERROR_HANDLER_CALLBACK_ENABLED == 1u)
/** \brief Error handler callback */
void NANO_OS_USER_ErrorHandlerCallback(const nano_os_error_t error);
#endif /* (NANO_OS_ERROR_HANDLER_CALLBACK_ENABLED == 1u) */

#if (NANO_OS_IDLE_TASK_CALLBACK_ENABLED == 1u)
/** \brief Idle task callback */
void NANO_OS_USER_IdleTaskCallback(void);
#endif /* (NANO_OS_IDLE_TASK_CALLBACK_ENABLED == 1u) */

#if (NANO_OS_TICK_CALLBACK_ENABLED == 1u)
/** \brief Tick interrupt callback */
void NANO_OS_USER_TickCallback(void);
#endif /* (NANO_OS_TICK_CALLBACK_ENABLED == 1u) */

#if (NANO_OS_ISR_REQUEST_ERROR_CALLBACK_ENABLED == 1u)
/** \brief Interrupt service error callback */
void NANO_OS_USER_IsrRequestErrorCallback(const nano_os_isr_service_request_t* const isr_request, const nano_os_error_t error);
#endif /* (NANO_OS_ISR_REQUEST_ERROR_CALLBACK_ENABLED == 1u) */





/********************************************************************************/
/*                            System timer                                      */
/********************************************************************************/


/* Check if the port provides the system timer */
#if (NANO_OS_PORT_PROVIDES_SYSTEM_TIMER == 0u)
/** \brief Nano OS system timer callback function */
typedef void (*fp_nano_os_system_timer_callback_func_t)(void);

/** \brief Initialize and start the system timer */
nano_os_error_t NANO_OS_USER_SystemTimerInit(const fp_nano_os_system_timer_callback_func_t system_timer_irq_callback);

#if ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u))
/** \brief Get the current timestamp in µs */
uint32_t NANO_OS_USER_SystemTimerGetTimestampInUs(void);
#endif /* ((NANO_OS_TRACE_ENABLED == 1u) || (NANO_OS_CPU_USAGE_MEASUREMENT_ENABLED == 1u)) */

#endif /* NANO_OS_PORT_PROVIDES_SYSTEM_TIMER */






/********************************************************************************/
/*                                Modules                                       */
/********************************************************************************/


/* Check if module is enabled */
#if (NANO_OS_TRACE_ENABLED == 1u)

/** \brief Initialize the user module which will send/receive the trace packets */
nano_os_error_t NANO_OS_USER_TraceInit(void);

/** \brief Send a real time trace packet */
void NANO_OS_USER_TraceSendRttPacket(const void* const rtt_packet, const size_t packet_size);

#endif /* (NANO_OS_TRACE_ENABLED == 1u) */



/* Check if module is enabled */
#if (NANO_OS_CONSOLE_ENABLED == 1u)

/** \brief Initialize the hardware drivers of the user module which will send/receive the console packets */
nano_os_error_t NANO_OS_USER_ConsoleHwInit(void);

/** \brief Initialize the user module which will send/receive the console packets */
nano_os_error_t NANO_OS_USER_ConsoleInit(void);

/** \brief Wait forever for an incoming char on the console link (this function must be blocking) */
nano_os_error_t NANO_OS_USER_ConsoleReadChar(char* const c);

/** \brief Write a string on the console link */
nano_os_error_t NANO_OS_USER_ConsoleWriteString(const char* const string);

#endif /* (NANO_OS_CONSOLE_ENABLED == 1u) */



/* Check if module is enabled */
#if (NANO_OS_DEBUG_ENABLED == 1u)

/** \brief Initialize the hardware drivers of the user module which will send/receive the debug packets */
nano_os_error_t NANO_OS_USER_DebugHwInit(void);

/** \brief Initialize the user module which will send/receive the debug packets */
nano_os_error_t NANO_OS_USER_DebugInit(void);

/** \brief Wait forever for an incoming byte on the debug link (this function must be blocking) */
nano_os_error_t NANO_OS_USER_DebugReadByte(uint8_t* const c);

/** \brief Initialize a new packet to send on the debug link */
nano_os_error_t NANO_OS_USER_DebugInitPacket(const size_t packet_size);

/** \brief Fill current packet with data to send on the debug link */
nano_os_error_t NANO_OS_USER_DebugWriteToPacket(const uint8_t* const data, const size_t size);

/** \brief Finalize the current packet to send on the debug link */
nano_os_error_t NANO_OS_USER_DebugFinalizePacket(void);

#endif /* (NANO_OS_DEBUG_ENABLED == 1u) */




#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_USER_H */
