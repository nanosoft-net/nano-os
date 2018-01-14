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


    .section .text

	/** \brief Service call number for the first context switch */
	.equ SVC_FIRST_CONTEXT_SWITCH, 0x24
	/** \brief Service call number to switch to priviledged mode */
	.equ SVC_SWITCH_TO_PRIVILEDGED_MODE, 0xAA


	.global _NANO_OS_PORT_SaveInterruptStatus
	.global _NANO_OS_PORT_RestoreInterruptStatus

	.global _NANO_OS_PORT_AtomicInc32
	.global _NANO_OS_PORT_AtomicDec32

	.global _NANO_OS_PORT_FirstContextSwitch
	.global _NANO_OS_PORT_ContextSwitch
	.global _NANO_OS_PORT_ContextSwitchFromIsr

	.global _NANO_OS_PORT_SwitchToPriviledgedMode
	.global _NANO_OS_PORT_SwitchToUnpriviledgedMode



    .extern _g_nano_os




.type _NANO_OS_PORT_SaveInterruptStatus, %function
/* nano_os_int_status_reg_t NANO_OS_PORT_SaveInterruptStatus(void)
   Disable interrupts and return previous interrupt status register -> Register R1 */
_NANO_OS_PORT_SaveInterruptStatus:

	mvfc psw, r1
	clrpsw i
    rts


.type _NANO_OS_PORT_RestoreInterruptStatus, %function
/* void NANO_OS_PORT_RestoreInterruptStatus(const nano_os_int_status_reg_t int_status_reg)
   Restore the interrupt status register passed in parameter -> Register R1 */
_NANO_OS_PORT_RestoreInterruptStatus:

	mvtc r1, psw
    rts





.type _NANO_OS_PORT_AtomicInc32, %function
/* void NANO_OS_PORT_AtomicInc32(uint32_t* const var)
   Atomic increment of a 32bits variable */
_NANO_OS_PORT_AtomicInc32:

	/* Disable and save interrupt state */
	mvfc psw, r2
	clrpsw i

	/* Increment variable */
	mov [r1], r3
	add #1, r3
	mov r3, [r1]

	/* Restore previous interrupt state */
	mvtc r1, psw

	rts


.type _NANO_OS_PORT_AtomicDec32, %function
/* void NANO_OS_PORT_AtomicDec32(uint32_t* const var)
   Atomic decrement of a 32bits variable */
_NANO_OS_PORT_AtomicDec32:

	/* Disable and save interrupt state */
	mvfc psw, r2
	clrpsw i

	/* Decrement variable */
	mov [r1], r3
	sub #1, r3
	mov r3, [r1]

	/* Restore previous interrupt state */
	mvtc r1, psw

	rts


.type _NANO_OS_PORT_SwitchToPriviledgedMode, %function
/* void NANO_OS_PORT_SwitchToPriviledgedMode(void)
   Switch the CPU to priviledged mode */
_NANO_OS_PORT_SwitchToPriviledgedMode:

	rts


.type NANO_OS_PORT_SwitchToUnpriviledgedMode, %function
/* void NANO_OS_PORT_SwitchToUnpriviledgedMode(void)
   Switch the CPU to unpriviledged mode */
_NANO_OS_PORT_SwitchToUnpriviledgedMode:

	rts



.type _NANO_OS_PORT_FirstContextSwitch, %function
/* nano_os_error_t NANO_OS_PORT_FirstContextSwitch(void)
   Port specific first task context switch */
_NANO_OS_PORT_FirstContextSwitch:

    rts



.type _NANO_OS_PORT_ContextSwitchFromIsr, %function
/* void NANO_OS_PORT_ContextSwitchFromIsr(void)
   Port specific interrupt level context switch */
_NANO_OS_PORT_ContextSwitchFromIsr:

	rts



.type _NANO_OS_PORT_ContextSwitch, %function
/* void NANO_OS_PORT_ContextSwitch(void)
   Port specific task level context switch : register saving + context switch */
_NANO_OS_PORT_ContextSwitch:

    rts


    .end
