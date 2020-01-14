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
#include "pmc.h"

/** \brief Enable the clock for a peripheral */
void PMC_EnablePeripheral(const uint32_t id)
{
    if( id < 32 )
    {
        PMC->PMC_PCER0 = (1 << id);
    }
    else
    {
        PMC->PMC_PCER1 = (1 << (id-32));
    }
}

/** \brief Disable the clock for a peripheral */
void PMC_DisablePeripheral(const uint32_t id)
{
    if( id < 32 )
    {
        PMC->PMC_PCDR0 = (1 << id);
    }
    else
    {
        PMC->PMC_PCDR1 = (1 << (id-32));
    }
}

