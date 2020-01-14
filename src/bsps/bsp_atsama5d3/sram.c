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

#include "sama5d3x.h"


/** \brief Remap SRAM to address 0x00000000 */
void sram_remap(void)
{
    /* Remap SRAM to address 0x00000000 */
    MATRIX->MATRIX_MRCR = (1 << 0);
    AXIMX->AXIMX_REMAP = (1 << 0);
}

