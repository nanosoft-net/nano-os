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


#ifndef DEFECT_H
#define DEFECT_H

#include "nano_os_api.h"



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** \brief Defect sources */
typedef enum _defect_src_t
{
    /** \brief Heartbeat */
    DEF_HEARTBEAT = 0u,
    /** \brief IPC reception */
    DEF_IPC_RX = 1u,
    /** \brief IPC transmission */
    DEF_IPC_TX = 2u,

    /** \brief Number of defect sources */
    DEF_MAX
} defect_src_t;






/** \brief Initialize the defect management  */
nano_os_error_t DEFECT_Init(void);

/** \brief Raise a defect */
bool DEFECT_Raise(const defect_src_t defect_src);

/** \brief Clear a defect */
bool DEFECT_Clear(const defect_src_t defect_src);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DEFECT_H */
