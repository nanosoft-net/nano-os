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


#ifndef AIC_H
#define AIC_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Interrupt priority */
typedef enum _interrupt_priority_t
{
    INTP_LOWEST = 0,
    INTP_LOW,
    INTP_BELOW_NORMAL,
    INTP_NORMAL,
    INTP_ABOVE_NORMAL,
    INTP_HIGH,
    INTP_VERY_HIGH,
    INTP_HIGHEST
} interrupt_priority_t;

/** \brief Interrupt handler prototype */
typedef void (*fp_interrupt_handler_t)(void);


/** \brief Initialize the interrupt controller */
void AIC_Init();

/** \brief Install an interrupt handler */
void AIC_HandlerInstall(const uint32_t source, const fp_interrupt_handler_t handler, const interrupt_priority_t priority);

/** \brief ENable an interrupt */
void AIC_EnableInterrupt(const uint32_t source);

/** \brief Disable an interrupt */
void AIC_DisableInterrupt(const uint32_t source);

/** \brief Clear an interrupt */
void AIC_ClearInterrupt(const uint32_t source);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AIC_H */
