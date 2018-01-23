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

#ifndef MPU_FAULTS_H
#define MPU_FAULTS_H

#include "nano_os_types.h"



/** \brief Triggers an MPU fault by driving an I/O from the main task */
extern volatile bool g_mpu_fault_io;

/** \brief Triggers an MPU fault by modifying an internal variable of Nano OS from the main task */
extern volatile bool g_mpu_fault_os_main;

/** \brief Triggers an MPU fault by modifying an internal variable of Nano OS from the led task */
extern volatile bool g_mpu_fault_os_led;

/** \brief Triggers an MPU fault by modifying an internal variable of the main task from the led task */
extern volatile bool g_mpu_fault_task_main;

/** \brief Triggers an MPU fault by modifying an internal variable of the led task from the main task */
extern volatile bool g_mpu_fault_task_led;




#endif /* MPU_FAULTS_H */
