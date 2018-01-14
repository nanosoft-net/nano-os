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

#ifndef PMC_H
#define PMC_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Enable the clock for a peripheral */
void PMC_EnablePeripheral(const uint32_t id);

/** \brief Disable the clock for a peripheral */
void PMC_DisablePeripheral(const uint32_t id);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PMC_H */
