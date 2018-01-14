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

	.section .startup

	.global _start
	.global Reset_Handler

	.weak PrivInstr_Handler
    .weak Access_Handler
    .weak UndefInstr_Handler
    .weak FPU_Handler
    .weak NMI_Handler

	.extern _TOP_OF_USTACK_
	.extern _TOP_OF_ISTACK_
	.extern fixed_vector_table
	.extern _pll_init
    .extern _c_init
    .extern _main

_start:
Reset_Handler:

	/* Configure stack pointers */
	mvtc #_TOP_OF_USTACK_, usp
	mvtc #_TOP_OF_ISTACK_, isp

    /* PLL init */
    bsr _pll_init

    /* C vars init */
    bsr _c_init

    /* Branch to main */
    bsr _main

    /* To prevent exiting from main */
    bra .

    /* Will never reach here but needed so that the linker
       doesn't optimize the fixed vector table away */
    bra fixed_vector_table


PrivInstr_Handler:
	bra .

Access_Handler:
	bra .

UndefInstr_Handler:
	bra .

FPU_Handler:
	bra .

NMI_Handler:
	bra .


  

    .end
