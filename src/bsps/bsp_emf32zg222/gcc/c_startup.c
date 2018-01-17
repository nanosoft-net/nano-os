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


/** \brief RAM variables initialization :
 *         - bss section is set to 0
 *         - data section is initialized from ROM */
void c_init()
{
    extern char _BSS_START_[];
    extern char _BSS_END_[];
    extern char _DATA_ROM_START_[];
    extern char _DATA_RAM_START_[];
    extern char _DATA_RAM_END_[];

    unsigned long *src, *dest;

    /* 0 init of the BSS section */
    src = (unsigned long*)_BSS_START_;
    while(src!=(unsigned long*)_BSS_END_)
    {
        (*src) = 0;
        src++;
    }

    /* init of the DATA section */
    src = (unsigned long*)_DATA_ROM_START_;
    dest = (unsigned long*)_DATA_RAM_START_;
    while(dest!=(unsigned long*)_DATA_RAM_END_)
    {
        (*dest) = (*src);
        src++;
        dest++;
    }
}
