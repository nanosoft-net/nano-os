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
    .arm

    .section .startup


    .global _start

    .extern pll_init
    .extern c_init
    .extern cpp_init
    .extern main
    .extern sram_remap
    .extern _TOP_OF_SVC_STACK_
    .extern _TOP_OF_IRQ_STACK_
    .extern _TOP_OF_USR_STACK_
    
    .extern NANO_OS_PORT_SvcHandler
    .extern NANO_OS_PORT_IrqHandler

    .weak UndefInstruction_Handler
    .weak PrefetchAbort_Handler
    .weak DataAbort_Handler
    .weak FIQ_Handler


	.equ ARM_MODE_USR, 0x10
	.equ ARM_MODE_IRQ, 0x12
	.equ ARM_MODE_SVC, 0x13

	.equ I_BIT, 0x80
	.equ F_BIT, 0x40



_start:
_vectors:
    /* Cortex A5 internal interrupts */
    b       Reset_Handler              /* Reset Handler */
    b       UndefInstruction_Handler   /* Undefined Instruction Handler */
	b       SVCHandler                 /* SVCCall Handler */
    b       PrefetchAbort_Handler      /* Prefetch Abort Handler */
    b       DataAbort_Handler          /* Data Abort Handler */
    .long   0x00000000                 /* Reserved */
    b       IRQHandler                 /* IRQ Handler */
    b       FIQ_Handler                /* FIQ Handler */
  

Reset_Handler:

	/* Enable VFP */
    /* - Enable access to CP10 and CP11 in CP15.CACR */
    mrc     p15, 0, r0, c1, c0, 2
    orr     r0, r0, #0xf00000
    mcr     p15, 0, r0, c1, c0, 2

	/* - Enable access to CP10 and CP11 in CP15.NSACR */
	/* - Set FPEXC.EN (B30) */
    fmrx    r0, fpexc
    orr     r0, r0, #0x40000000
    fmxr    fpexc, r0

	/* Init IRQ mode stack */
    msr     CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT
    ldr     sp, =_TOP_OF_IRQ_STACK_

    /* Init supervisor mode stack */
    msr     CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT
    ldr     sp, =_TOP_OF_SVC_STACK_

    /* Init user mode stack */
    msr     CPSR_c, #ARM_MODE_USR | I_BIT | F_BIT
    ldr     sp, =_TOP_OF_USR_STACK_

	/* PLL init */
    ldr     r0, =pll_init
    blx     r0

    /* Remap SRAM to address 0x00000000 */
    ldr     r0, =sram_remap
    blx     r0

    /* C vars init */
    ldr     r0, =c_init
    blx     r0

    /* C++ init */
    ldr     r0, =cpp_init
    blx     r0

    /* Enable interrupts */
    cpsie	i

    /* Branch to main */
    ldr     r0, =main
    blx     r0

    /* To prevent exiting from main */
    b       .

/* Redirection to Nano OS handler
   => needed because of the remap of SRAM from address 0x00300000 to 0x00000000 */
NANO_OS_PORT_SvcHandler_Address:
	.long NANO_OS_PORT_SvcHandler
SVCHandler:
	ldr		pc, [pc, #-12]

NANO_OS_PORT_IrqHandler_Address:
	.long NANO_OS_PORT_IrqHandler
IRQHandler:
	ldr		pc, [pc, #-12]


/* Unused handlers */



UndefInstruction_Handler:
    b       .

PrefetchAbort_Handler:
    b       .

DataAbort_Handler:
    b       .

FIQ_Handler:
    b       .

    
    
    .end
