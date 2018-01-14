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



/* \brief Macro specific to atsama5 CPU to retrieve IRQ number and handler
   Output:
   R0 => IRQ number
   R1 => IRQ handler address
 */
.macro NANO_OS_PORT_GetIRQHandler

    /* Load interrupt handler address (AIC->AIC_IVR) */
    ldr     r1, =0xFFFFF010
    ldr     r1, [r1]

    /* Because of protected mode, a write must be done in AIC->AIC_IVR */
    str     r1, [r1]

    /* Load interrupt number (AIC->AIC_ICR) */
    ldr     r0, =0xFFFFF018
    ldr     r0, [r0]

.endm



/* \brief Macro specific to atsama5 CPU to acknowledge IRQ
   Input
   R0 => IRQ number
 */
.macro NANO_OS_PORT_AcknowledgeIRQ

    /* Acknowledge interrupt (AIC->AIC_EOICR), any value can be written */
    ldr     r0, =0xFFFFF038
    str     r0, [r0]

.endm





