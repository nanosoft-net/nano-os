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

#ifndef NANO_OS_PORT_CFG_H
#define NANO_OS_PORT_CFG_H


/*********************************************************/
/*                 Nano OS settings                      */
/*              Do not edit this settings                */
/*********************************************************/

/** \brief Port name */
#define NANO_OS_PORT_NAME       "cortex-m7 with FPU and MPU"


/** \brief Indicate if the port uses a descending stack */
#define NANO_OS_PORT_DESCENDING_STACK                   1u

/** \brief Indicate if the port contains specific global data */
#define NANO_OS_PORT_CONTAINS_GLOBAL_DATA               1u

/** \brief Indicate if the port contains specific task data */
#define NANO_OS_PORT_CONTAINS_TASK_DATA                 1u

/** \brief Indicate if the port provides the stacks for Nano OS internal tasks */
#define NANO_OS_PORT_PROVIDES_STACKS                    1u

/** \brief Indicate if the port provides the system timer for Nano OS */
#define NANO_OS_PORT_PROVIDES_SYSTEM_TIMER              1u

/** \brief Minimum stack size in number of elements for this port */
#define NANO_OS_PORT_MIN_STACK_SIZE                     128u

/** \brief Maximum number of memory regions managed by the MPU */
#define NANO_OS_PORT_MPU_REGION_COUNT                   8u

/** \brief Maximum number of MPU memory regions specific to a task */
#define NANO_OS_PORT_TASK_MPU_REGION_COUNT              4u

/** \brief Number of MPU memory regions global to all tasks */
#define NANO_OS_PORT_COMMON_MPU_REGION_COUNT           (NANO_OS_PORT_MPU_REGION_COUNT - NANO_OS_PORT_TASK_MPU_REGION_COUNT)


#endif /* NANO_OS_PORT_CFG_H */
