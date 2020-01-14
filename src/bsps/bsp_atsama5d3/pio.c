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


#include "sama5d3x.h"
#include "pio.h"
#include "pmc.h"

/** \brief Accesses to all PIO port configuration registers */
static volatile Pio* pio_ports[] = {PIOA, PIOB, PIOC, PIOD, PIOE};

/** \brief Initialize PIO module */
void PIO_Init()
{
    /* Enable PIOA, PIOB, PIOC, PIOD, PIOE clocks */
    PMC_EnablePeripheral(ID_PIOA);
    PMC_EnablePeripheral(ID_PIOB);
    PMC_EnablePeripheral(ID_PIOC);
    PMC_EnablePeripheral(ID_PIOD);
    PMC_EnablePeripheral(ID_PIOE);
}


/** \brief Configure an IO */
void PIO_Configure(const pio_port_t port, const uint8_t line, const pio_mode_t mode, const pio_pull_mode_t pull_mode)
{
    /* Get pointer to the port registers */
    volatile Pio* port_reg = pio_ports[port];
    uint32_t line_mask = (1 << line);
    
    /* Configure mode */
    if( (mode == PIOM_INPUT) || (mode == PIOM_OUTPUT) )
    {
        /* Global purpose IO */
        port_reg->PIO_PER |= line_mask;
        if( mode == PIOM_OUTPUT )
        {
            port_reg->PIO_OER = line_mask;
        }
        else
        {
            port_reg->PIO_ODR = line_mask;
        }
    }
    else
    {
        /* Peripheral function */
        port_reg->PIO_PDR = line_mask;
        if( mode == PIOM_PERIPH_A )
        {
            port_reg->PIO_ABCDSR[0] &= ~(line_mask);
            port_reg->PIO_ABCDSR[1] &= ~(line_mask);
        }
        else if( mode == PIOM_PERIPH_B )
        {
            port_reg->PIO_ABCDSR[0] |= line_mask;
            port_reg->PIO_ABCDSR[1] &= ~(line_mask);
        }
        else if( mode == PIOM_PERIPH_C )
        {
            port_reg->PIO_ABCDSR[0] &= ~(line_mask);
            port_reg->PIO_ABCDSR[1] |= line_mask;
        }
        else
        {
            port_reg->PIO_ABCDSR[0] |= line_mask;
            port_reg->PIO_ABCDSR[1] |= line_mask;
        }
    }
    
    /* Configure pull up/down */
    if( pull_mode == PIOPM_NONE )
    {
        port_reg->PIO_PUDR = line_mask;
        port_reg->PIO_PPDDR = line_mask;
    }
    else if( pull_mode == PIOPM_UP )
    {
        port_reg->PIO_PUER = line_mask;
        port_reg->PIO_PPDDR = line_mask;
    }
    else
    {
        port_reg->PIO_PUDR = line_mask;
        port_reg->PIO_PPDER = line_mask;
    }
}

/** \brief Set an IO in high state */
void PIO_SetHigh(const pio_port_t port, const uint8_t line)
{
    pio_ports[port]->PIO_SODR = (1 << line);
}

/** \brief Set an IO in low state */
void PIO_SetLow(const pio_port_t port, const uint8_t line)
{
    pio_ports[port]->PIO_CODR = (1 << line);
}

/** \brief Set the level of an IO */
void PIO_SetLevel(const pio_port_t port, const uint8_t line, const pio_level_t level)
{
    if( level == PIOL_HIGH )
    {
        pio_ports[port]->PIO_SODR = (1 << line);
    }
    else
    {
        pio_ports[port]->PIO_CODR = (1 << line);
    }
}

/** \brief Get the level of an IO */
pio_level_t PIO_Get(const pio_port_t port, const uint8_t line)
{
    return (pio_level_t)((pio_ports[port]->PIO_PDSR >> line) & 0x01);
}




