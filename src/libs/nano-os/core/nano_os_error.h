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

#ifndef NANO_OS_ERROR_H
#define NANO_OS_ERROR_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Assert macro to call error handler */
#define NANO_OS_ERROR_ASSERT(assertion, error) if (!(assertion)) { NANO_OS_ERROR_Handler((error)); }

/** \brief Assert macro on a return value to call error handler */
#define NANO_OS_ERROR_ASSERT_RET(error) if ((error) != NOS_ERR_SUCCESS) { NANO_OS_ERROR_Handler((error)); }



/** \brief Nano OS error codes */
typedef enum _nano_os_error_t
{
    /** \brief Success */
    NOS_ERR_SUCCESS = 0,
    /** \brief Generic error */
    NOS_ERR_FAILURE = 1,
    /** \brief Invalid argument */
    NOS_ERR_INVALID_ARG = 2,
    /** \brief Timeout */
    NOS_ERR_TIMEOUT = 3,
    /** \brief Error during port specific initialization */
    NOS_ERR_PORT_INIT = 4,
    /** \brief Invalid OS object */
    NOS_ERR_INVALID_OBJECT = 5,
    /** \brief OS object has been destroyed */
    NOS_ERR_OBJECT_DESTROYED = 6,
    /** \brief No more task are in ready state */
    NOS_ERR_NO_TASK_READY = 7,
    /** \brief Invalid task priority level */
    NOS_ERR_INVALID_PRIORITY_LEVEL = 8,
    /** \brief Interrupt service request list is full */
    NOS_ERR_ISR_REQUEST_LIST_FULL = 9,
    /** \brief Semaphore is full */
    NOS_ERR_SEMAPHORE_FULL = 10,
    /** \brief Mutex is locked by another task */
    NOS_ERR_MUTEX_LOCKED = 11,
    /** \brief Mutex is owned by another task */
    NOS_ERR_MUTEX_NOT_OWNED = 12,
    /** \brief Invalid mutex lock count */
    NOS_ERR_MUTEX_LOCK_COUNT = 13,
	/** \brief Mailbox is full */
	NOS_ERR_MAILBOX_FULL = 14,
	/** \brief Timer is already started */
	NOS_ERR_TIMER_STARTED = 15,
	/** \brief Timer is already stopped */
    NOS_ERR_TIMER_STOPPED = 16,
    /** \brief Timer task potentially overrun */
    NOS_ERR_TIMER_TASK_OVERRUN = 17,
    /** \brief Heap memory is corrupted */
    NOS_ERR_CORRUPTED_HEAP = 18,
    /** \brief Unsupported operation */
    NOS_ERR_NOT_SUPPORTED = 19,
    /** \brief Invalid stack pointer, often due to stack overflow */
    NOS_ERR_INVALID_SP = 20,
    /** \brief Stack pointer has moved outside of stack range during execution or stack has been corrupted */
    NOS_ERR_CORRUPTED_STACK = 21,
    /** \brief Syscall is not allowed until Nano OS is started */
    NOS_ERR_OS_NOT_STARTED = 22,
    /** \brief MPU is not available on this device */
    NOS_ERR_MPU_NOT_AVAILABLE = 23,
    /** \brief MPU region start address alignment is invalid */
    NOS_ERR_MPU_REGION_ALIGNMENT = 24,
    /** \brief MPU region attribute is invalid */
    NOS_ERR_MPU_REGION_ATTRIBUTE = 25
} nano_os_error_t;


/** \brief Nano OS error handler */
void NANO_OS_ERROR_Handler(const nano_os_error_t error);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_ERROR_H */
