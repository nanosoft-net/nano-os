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

#ifndef NANO_OS_PORT_STASKS_H
#define NANO_OS_PORT_STASKS_H


#include "nano_os_types.h"


/** \brief Idle task stack */
extern nano_os_stack_t g_idle_task_stack[];

/** \brief ISR request task stack */
extern nano_os_stack_t g_isr_request_task_stack[];

/** \brief Timer task stack */
extern nano_os_stack_t g_timer_task_stack[];



/* Linker exported symbols */
extern char _OS_VAR_START_[];
extern char _OS_VAR_END_[];
extern char _IDLE_TASK_VAR_START_[];
extern char _IDLE_TASK_VAR_END_[];
extern char _ISR_REQUEST_TASK_VAR_START_[];
extern char _ISR_REQUEST_TASK_VAR_END_[];
extern char _TIMER_TASK_VAR_START_[];
extern char _TIMER_TASK_VAR_END_[];

extern char _COMMON_CODE_START_[];
extern char _COMMON_CODE_END_[];
extern char _COMMON_DATA_START_[];
extern char _COMMON_DATA_END_[];

#endif /* NANO_OS_PORT_STASKS_H */
