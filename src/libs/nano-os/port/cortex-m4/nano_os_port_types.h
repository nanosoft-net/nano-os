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

#ifndef NANO_OS_PORT_TYPES_H
#define NANO_OS_PORT_TYPES_H


/* Standard integer types*/
#if (NANO_OS_USE_STD_INT == 0)
typedef signed char             int8_t;
typedef unsigned char           uint8_t;
typedef short int               int16_t;
typedef unsigned short int      uint16_t;
typedef long int                int32_t;
typedef unsigned long int       uint32_t;
typedef long long int           int64_t;
typedef unsigned long long int  uint64_t;
#endif /* (NANO_OS_USE_STD_INT == 0) */

/* Standard bool type definition */
#if (NANO_OS_USE_STD_BOOL == 0u)
#define bool    uint8_t
#endif /* (NANO_OS_USE_STD_BOOL == 0u) */


/** \brief Stack data type */
typedef uint32_t nano_os_stack_t;

/** \brief Interrupt status register */
typedef uint32_t nano_os_int_status_reg_t;


/** \brief Port specific task data */
typedef struct _nano_os_port_task_data_t
{
    /** \brief Indicate if the task uses FPU instructions */
    bool use_fpu;
    /** \brief Indicate if the task runs in priviledged mode */
    bool is_priviledged;
} nano_os_port_task_data_t;


/** \brief Port specific task init data */
typedef struct _nano_os_port_task_init_data_t
{
    /** \brief Indicate if the task uses FPU instructions */
    bool use_fpu;
    /** \brief Indicate if the task runs in priviledged mode */
    bool is_priviledged;
} nano_os_port_task_init_data_t;


#endif /* NANO_OS_PORT_TYPES_H */
