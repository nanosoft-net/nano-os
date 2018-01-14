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

#include "bsp.h"

#include <stdio.h>

/** \brief Initialize the board */
nano_os_error_t NANO_OS_BSP_Init(void)
{
    nano_os_error_t ret = NOS_ERR_SUCCESS;

    return ret;
}

/** \brief Get the number of LEDs available */
uint8_t NANO_OS_BSP_GetLedCount(void)
{
    return 10u;
}


/** \brief Turn ON the specified LED */
void NANO_OS_BSP_LedOn(const uint8_t led)
{
    fprintf(stderr, "Led %d ON\r\n", ((uint32_t)led));
}

/** \brief Turn OFF the specified LED */
void NANO_OS_BSP_LedOff(const uint8_t led)
{
    fprintf(stderr, "Led %d OFF\r\n", ((uint32_t)led));
}

/** \brief Get the heap memory area description */
void NANO_OS_BSP_GetHeapArea(void** heap_area, size_t* heap_size)
{
    static uint8_t heap_buffer[2048u];
    (*heap_area) = heap_buffer;
    (*heap_size) = sizeof(heap_buffer);
}
