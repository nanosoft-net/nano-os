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

#ifndef MEMORY_REGIONS_H
#define MEMORY_REGIONS_H


/* These are all linker exported symbols */




/* Nano OS internal variables */
extern char _OS_VAR_START_[];
extern char _OS_VAR_END_[];


/* Idle task stack */
extern char _IDLE_TASK_VAR_START_[];
extern char _IDLE_TASK_VAR_END_[];

/* Console module data */
extern char _CONSOLE_MODULE_VAR_START_[];
extern char _CONSOLE_MODULE_VAR_END_[];

/* Debug module data */
extern char _DEBUG_MODULE_VAR_START_[];
extern char _DEBUG_MODULE_VAR_END_[];

/* RAM memory area which can be read by the Nano OS modules to
 * enable the retrieval of task and sync objects infos */
extern char _NANO_OS_MODULES_RO_DATA_START_[];
extern char _NANO_OS_MODULES_RO_DATA_END_[];


/* Variables which are used by all tasks */
extern char _COMMON_DATA_START_[];
extern char _COMMON_DATA_END_[];

/* Code area which is used by all tasks */
extern char _COMMON_CODE_START_[];
extern char _COMMON_CODE_END_[];


/* Main task private data */
extern char _MAIN_TASK_VAR_START_[];
extern char _MAIN_TASK_VAR_END_[];

/* LED task private data */
extern char _LED_TASK_VAR_START_[];
extern char _LED_TASK_VAR_END_[];



#endif /* MEMORY_REGIONS_H */
