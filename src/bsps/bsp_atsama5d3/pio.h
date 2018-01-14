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

#ifndef PIO_H
#define PIO_H


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief PIO port */
typedef enum _pio_port_t
{
    PIO_A = 0,
    PIO_B,
    PIO_C,
    PIO_D,
    PIO_E,
} pio_port_t;

/** \brief PIO mode */
typedef enum _pio_mode_t
{
    PIOM_INPUT = 0,
    PIOM_OUTPUT,
    PIOM_PERIPH_A,
    PIOM_PERIPH_B,
    PIOM_PERIPH_C,
    PIOM_PERIPH_D
} pio_mode_t;

/** \brief PIO pull mode */
typedef enum _pio_pull_mode_t
{
    PIOPM_NONE = 0,
    PIOPM_UP,
    PIOPM_DOWN
} pio_pull_mode_t;

/** \brief PIO level */
typedef enum _pio_level_t
{
    PIOL_HIGH = 0,
    PIOL_LOW = 1
} pio_level_t;


/** \brief Initialize PIO module */
void PIO_Init();

/** \brief Configure an IO */
void PIO_Configure(const pio_port_t port, const uint8_t line, const pio_mode_t mode, const pio_pull_mode_t pull_mode);

/** \brief Set an IO in high state */
void PIO_SetHigh(const pio_port_t port, const uint8_t line);

/** \brief Set an IO in low state */
void PIO_SetLow(const pio_port_t port, const uint8_t line);

/** \brief Set the level of an IO */
void PIO_SetLevel(const pio_port_t port, const uint8_t line, const pio_level_t level);

/** \brief Get the level of an IO */
pio_level_t PIO_Get(const pio_port_t port, const uint8_t line);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PIO_H */
