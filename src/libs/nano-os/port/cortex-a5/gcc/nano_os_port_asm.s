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

    .syntax unified
    .cpu cortex-a5
    .fpu vfpv4
    .arm

    /** \brief Service call number for the first context switch */
	.equ SVC_FIRST_CONTEXT_SWITCH, 0x24
	/** \brief Service call number to switch to priviledged mode */
	.equ SVC_SWITCH_TO_PRIVILEDGED_MODE, 0xAA

	/** \brief USR execution mode */
	.equ ARM_MODE_USR, 0x10
	/** \brief IRQ execution mode */
	.equ ARM_MODE_IRQ, 0x12
	/** \brief SVC execution mode */
	.equ ARM_MODE_SVC, 0x13
	/** \brief SYS execution mode */
	.equ ARM_MODE_SYS, 0x1F

	/* Include CPU specific macros */
    .include "nano_os_port_cpu_asm.inc"



	.global NANO_OS_PORT_SaveInterruptStatus
	.global NANO_OS_PORT_RestoreInterruptStatus

	.global NANO_OS_PORT_AtomicInc32
	.global NANO_OS_PORT_AtomicDec32

	.global NANO_OS_PORT_FirstContextSwitch
	.global NANO_OS_PORT_ContextSwitch

	.global NANO_OS_PORT_SwitchToPriviledgedMode
	.global NANO_OS_PORT_SwitchToUnpriviledgedMode

	.global NANO_OS_PORT_SvcHandler
	.global NANO_OS_PORT_IrqHandler

    .extern g_nano_os
	.extern NANO_OS_INTERRUPT_Enter
	.extern NANO_OS_INTERRUPT_Exit



.type NANO_OS_PORT_SaveInterruptStatus, %function
/* nano_os_int_status_reg_t NANO_OS_PORT_SaveInterruptStatus(void)
   Disable interrupts and return previous interrupt status register -> Register R0 */
NANO_OS_PORT_SaveInterruptStatus:

    mrs     r0, cpsr
    cpsid   i
    bx      lr


.type NANO_OS_PORT_RestoreInterruptStatus, %function
/* void NANO_OS_PORT_RestoreInterruptStatus(const nano_os_int_status_reg_t int_status_reg)
   Restore the interrupt status register passed in parameter -> Register R0 */
NANO_OS_PORT_RestoreInterruptStatus:

    msr     cpsr, r0
    bx      lr




.type NANO_OS_PORT_SwitchToPriviledgedMode, %function
/* void NANO_OS_PORT_SwitchToPriviledgedMode(void)
   Switch the CPU to priviledged mode */
NANO_OS_PORT_SwitchToPriviledgedMode:

	svc SVC_SWITCH_TO_PRIVILEDGED_MODE
	bx lr


.type NANO_OS_PORT_SwitchToUnpriviledgedMode, %function
/* void NANO_OS_PORT_SwitchToUnpriviledgedMode(void)
   Switch the CPU to unpriviledged mode */
NANO_OS_PORT_SwitchToUnpriviledgedMode:

	cps 	#ARM_MODE_USR
	bx lr



.type NANO_OS_PORT_FirstContextSwitch, %function
/* nano_os_error_t NANO_OS_PORT_FirstContextSwitch(void)
   Port specific first task context switch */
NANO_OS_PORT_FirstContextSwitch:

	/* Start first context switch */
	svc		SVC_FIRST_CONTEXT_SWITCH

	/* Return to caller (should never happen)*/
    bx      lr



.type NANO_OS_PORT_ContextSwitch, %function
/* void NANO_OS_PORT_ContextSwitch(void)
   Port specific task level context switch : register saving + context switch */
NANO_OS_PORT_ContextSwitch:

	/* Save minimal context */
	stmdb	sp!, {r0-r3, r12, lr}

	/* Save cpsr */
	mrs		r0, cpsr
	stmdb	sp!, {r0}

	/* Save additionnal registers */
	stmdb	sp!, {r4-r11}

	/* Get the stack pointer of the next task */
    ldr     r0, =g_nano_os
    ldr     r1, [r0, #4]
    ldr     r2, [r1]

    /* Save the stack pointer for the current task */
    ldr     r3, [r0]
    str     sp, [r3]

    /* Set the stack pointer to the next task */
    mov     sp, r2

    /* g_nano_os.current_task = g_nano_os.next_running_task */
    str     r1, [r0]

	/* Restore additionnal registers */
	ldmia	sp!, {r4-r11}

	/* Restore cpsr */
	ldmia	sp!, {r0}
	msr		cpsr, r0

	/* Restore minimal context */
	ldmia	sp!, {r0-r3, r12, lr}

    /* Return to task */
    bx      lr


.type NANO_OS_PORT_SvcHandler, %function
/* void NANO_OS_PORT_SvcHandler(void)
   Handler for the SVC exception. Performs switchs between priviledged modes */
NANO_OS_PORT_SvcHandler:

	/* Save minimal context */
	stmdb	sp!, {r0-r3, r12, lr}

	/* Extract SVC number */
	ldr		r0, [lr, #-4]
	bic		r0, r0, #0xFF000000

	/* Branch to corresponding handler */
	movs	r1, SVC_FIRST_CONTEXT_SWITCH
	cmp		r0, r1
	beq		NANO_OS_PORT_SvcHandler_FirstContextSwitch
	movs	r1, SVC_SWITCH_TO_PRIVILEDGED_MODE
	cmp		r0, r1
	beq		NANO_OS_PORT_SvcHandler_SwitchToPriviledgedMode

NANO_OS_PORT_SvcHandler_FirstContextSwitch:
	/* First context switch */

	/* Get the stack pointer of the next task */
    ldr     r0, =g_nano_os
    ldr     r1, [r0, #4]
    ldr     r2, [r1]

    /* g_nano_os.current_task = g_nano_os.next_running_task */
    str     r1, [r0]

    /* Restore stack pointer for further SVC calls */
	add		sp, sp, #0x24

	/* Restore the additionnals registers from the next task stack */
    ldmia	r2!, {r4-r11}

    /* Restore the spsr from the next task stack */
	ldmia	r2!, {r3}
	msr		spsr, r3

	/* Prepare SP value for USR and SYS mode */
	cps		#ARM_MODE_SYS
	isb
	mov		sp, r2
	add		sp, sp, #24
	cps		#ARM_MODE_SVC
	isb

	/* Restore context and exit exception */
	mov		lr,	r2
    ldmia	lr!, {r0-r3, r12, pc}^


NANO_OS_PORT_SvcHandler_SwitchToPriviledgedMode:
	/* Switch the CPU to priviledged mode */
	mrs 	r0, spsr
	orr		r0, r0, #ARM_MODE_SYS
	msr		spsr, r0

NANO_OS_PORT_SvcHandler_Exit:

	/* Restore context and exit exception */
    ldmia	sp!, {r0-r3, r12, pc}^


.type NANO_OS_PORT_IrqHandler2, %function
/* void NANO_OS_PORT_IrqHandler2()
   Handler for the IRQ exception. Call user IRQ handler and performs context switchs */
NANO_OS_PORT_IrqHandler2:

	/* Adjust return address */
	sub		lr, lr, #4

	/* Save minimal context */
	stmdb	sp!, {r0-r1}
	mov		r0, sp
	mov		r1, lr

	/* Restore stack pointer for further IRQs */
	add		sp, sp, #8

	/* Switch to SVC mode */
	cps		#ARM_MODE_SVC
	isb

	/* Restore saved registers */
	mov		lr, r1
	ldr		r1, [r0, #4]
	ldr		r0, [r0]

	/* Save minimal context */
	stmdb	sp!, {r0-r3, r12, lr}

	/* Check for 8-byte alignment and save a word */
    /* to indicate the stack adjustment used (0 or 4)
    /* plus a dummy word preserve 8-byte alignment */
    and		r0, sp, #4
    sub		sp, sp, r0
    stmdb	sp!, {r0-r1}

	/* Interrupt entry */
	blx		NANO_OS_INTERRUPT_Enter

	/* Get the IRQ number and handler
	   Output:
           R0 => IRQ number
           R1 => IRQ handler address */
	NANO_OS_PORT_GetIRQHandler

	/* Call user defined handler */
	mov	r12, sp
	blx		r1

	/* Acknowledge interrupt
	   Input
           R0 => IRQ number */
    NANO_OS_PORT_AcknowledgeIRQ

    /* Interrupt exit */
	blx		NANO_OS_INTERRUPT_Exit

	ldmia	sp!, {r0-r1}
    add		sp, sp, r0

	/* Restore context and exit exception */
    ldmia	sp!, {r0-r3, r12, pc}^



.type NANO_OS_PORT_IrqHandler, %function
/* void NANO_OS_PORT_IrqHandler()
   Handler for the IRQ exception. Call user IRQ handler and performs context switchs */
NANO_OS_PORT_IrqHandler:

	/* Adjust return address */
	sub		lr, lr, #4

	/* Save minimal context */
	stmdb	sp!, {r0-r3, r12, lr}

	/* Check for 8-byte alignment and save a word */
    /* to indicate the stack adjustment used (0 or 4) */
    and		r0, sp, #4
    sub		sp, sp, r0
    stmdb	sp!, {r0}

	/* Interrupt entry */
	blx		NANO_OS_INTERRUPT_Enter

	/* Get the IRQ number and handler
	   Output:
           R0 => IRQ number
           R1 => IRQ handler address */
	NANO_OS_PORT_GetIRQHandler

	/* Call user defined handler */
	mov	r12, sp
	blx		r1

	/* Acknowledge interrupt
	   Input
           R0 => IRQ number */
    NANO_OS_PORT_AcknowledgeIRQ

    /* Interrupt exit */
	blx		NANO_OS_INTERRUPT_Exit

	/* Adjust stack pointer with stored alignment */
	ldmia	sp!, {r0}
    add		sp, sp, r0

    /* Check if a context switch is needed */
    ldr		r0, =g_nano_os
    ldrb	r1, [r0, #8]
	cmp		r1, #0

NANO_OS_PORT_IrqHandler_ExitWithoutContextSwitch:

	/* Restore context and exit exception */
    ldmiaeq	sp!, {r0-r3, r12, pc}^

NANO_OS_PORT_IrqHandler_ExitWithContextSwitch:

	/* Reset context switch flag */
	mov		r1, #0
	strb	r1, [r0, #8]

	/* Switch to SYS mode to execute first part of context switch */
	mrs		r0, spsr
	mov		r12, sp
	cps		#ARM_MODE_SYS
	isb

	/* Copy R0-R3 / R12 / LR from exception stack to task stack */
	ldr		r3, [r12, #0x14]
	ldr		r2, [r12, #0x10]
	ldr		r1, [r12, #0x0C]
	stmdb	sp!, {r1-r3}
	ldr		r3, [r12, #0x08]
	ldr		r2, [r12, #0x04]
	ldr		r1, [r12, #0x00]
	stmdb	sp!, {r1-r3}

	/* Save task cpsr */
	stmdb	sp!, {r0}

	/* Save additionnal registers */
	stmdb	sp!, {r4-r11}

	/* Get the stack pointer of the next task */
	ldr		r0, =g_nano_os
    ldr     r1, [r0, #4]
    ldr     r2, [r1]

    /* Save the stack pointer for the current task */
    ldr     r3, [r0]
    str     sp, [r3]

    /* Set the stack pointer to the next task */
    mov     sp, r2

    /* g_nano_os.current_task = g_nano_os.next_running_task */
    str     r1, [r0]

	/* Restore additionnal registers */
	ldmia	sp!, {r4-r11}

	/* Restore task cpsr */
	ldmia	sp!, {r0}

	/* Copy R0-R3 / R12 / LR from task stack to exception stack */
	ldmia	sp!, {r1-r3}
	str		r1, [r12, #0x00]
	str		r2, [r12, #0x04]
	str		r3, [r12, #0x08]
	ldmia	sp!, {r1-r3}
	str		r1, [r12, #0x0C]
	str		r2, [r12, #0x10]
	str		r3, [r12, #0x14]

	/* Switch back to IRQ mode to end context switch */
	cps		#ARM_MODE_IRQ
	isb

	/* Restore task cpsr */
	msr		spsr, r0

	/* Restore context and exit exception */
    ldmia	sp!, {r0-r3, r12, pc}^


    .end
