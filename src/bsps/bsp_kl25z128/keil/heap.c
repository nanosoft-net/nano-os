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


/** \brief Get the heap memory area description */
void NANO_OS_BSP_GetHeapArea(void** heap_area, size_t* heap_size)
{
    extern char __heap_base[];
    extern char __heap_limit[];

    (*heap_area) = __heap_base;
    (*heap_size) = NANO_OS_CAST(size_t, __heap_limit - __heap_base);
}
