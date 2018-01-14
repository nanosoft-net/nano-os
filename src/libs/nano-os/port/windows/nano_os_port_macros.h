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

#ifndef NANO_OS_PORT_MACROS_H
#define NANO_OS_PORT_MACROS_H


/** \brief Disable interrupts */
#define NANO_OS_PORT_DISABLE_INTERRUPTS()   NANO_OS_PORT_VIRTUAL_INTERRUPT_Disable()

/** \brief Enable interrupts */
#define NANO_OS_PORT_ENABLE_INTERRUPTS()    NANO_OS_PORT_VIRTUAL_INTERRUPT_Enable()

/** \brief Get the current value of the task stack pointer */
#define NANO_OS_PORT_GET_TASK_SP()          NANO_OS_CAST(uint32_t, (&current_task->stack_origin[current_task->stack_size - 1u]))

/** \brief Enter into a critical section */
#define NANO_OS_PORT_ENTER_CRITICAL(int_status_reg)   (int_status_reg = NANO_OS_PORT_VIRTUAL_INTERRUPT_SaveInterruptStatus())

/** \brief Leave a critical section */
#define NANO_OS_PORT_LEAVE_CRITICAL(int_status_reg)   NANO_OS_PORT_VIRTUAL_INTERRUPT_RestoreInterruptStatus(int_status_reg)

/** \brief Atomic increment of a 32bits variable */
#define NANO_OS_PORT_ATOMIC_INC32(var)      NANO_OS_PORT_AtomicInc32(&var)

/** \brief Atomic decrement of a 32bits variable */
#define NANO_OS_PORT_ATOMIC_DEC32(var)      NANO_OS_PORT_AtomicDec32(&var)

/** \brief Atomic read of a 32bits variable */
#define NANO_OS_PORT_ATOMIC_READ32(var)     (var)

/** \brief Switch to priviledged mode  */
#define NANO_OS_PORT_RAISE_PRIVILEDGES()    

/** \brief Switch to unpriviledged mode  */
#define NANO_OS_PORT_LOWER_PRIVILEDGES()    

/** \brief Manage the entry of a system call  */
#define NANO_OS_PORT_SYSCALL_ENTER()

/** \brief Manage the exit of a system call  */
#define NANO_OS_PORT_SYSCALL_EXIT()

/** \brief Task context stack frame size in number of elements for this port */
#define NANO_OS_PORT_TASK_CONTEXT_STACK_FRAME_SIZE(task)      0u

/** \brief Idle task port specific operation */
#define NANO_OS_PORT_IDLE_TASK_HOOK()       Sleep(1)


/* Functions implemented in assembly language and needed by the port specific macros */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Enable the virtual interrupts */
void NANO_OS_PORT_VIRTUAL_INTERRUPT_Enable(void);

/** \brief Disable the virtual interrupts */
void NANO_OS_PORT_VIRTUAL_INTERRUPT_Disable(void);

/** \brief Disable interrupts and return previous interrupt status register */
nano_os_int_status_reg_t NANO_OS_PORT_VIRTUAL_INTERRUPT_SaveInterruptStatus(void);

/** \brief Restore the interrupt status register passed in parameter */
void NANO_OS_PORT_VIRTUAL_INTERRUPT_RestoreInterruptStatus(const nano_os_int_status_reg_t int_status_reg);


/** \brief Atomic increment of a 32bits variable */
void NANO_OS_PORT_AtomicInc32(uint32_t* const var);

/** \brief Atomic decrement of a 32bits variable */
void NANO_OS_PORT_AtomicDec32(uint32_t* const var);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* NANO_OS_PORT_MACROS_H */
