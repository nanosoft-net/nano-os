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

#include "nano_os.h"
#include "nano_os_port.h"
#include "nano_os_port_mpu.h"


/** \brief Maximum value for the MPU attribute AP field */
#define NANO_OS_PORT_MPU_ATTRIBUTE_MAX_AP_FIELD_VALUE       0x07u

/** \brief Maximum value for the MPU attribute memory access field */
#define NANO_OS_PORT_MPU_ATTRIBUTE_MAX_MEM_FIELD_VALUE      0x3Fu





/** \brief Address of the MPU type register */
#define MPU_TYPE_REG            (* NANO_OS_CAST(volatile uint32_t*, 0xe000ed90))

/** \brief Address of the MPU control register */
#define MPU_CTRL_REG            (* NANO_OS_CAST(volatile uint32_t*, 0xe000ed94))

/** \brief Address of the MPU region number register */
#define MPU_RNR_REG             (* NANO_OS_CAST(volatile uint32_t*, 0xe000ed98))

/** \brief Address of the MPU region base address register */
#define MPU_RBAR_REG            (* NANO_OS_CAST(volatile uint32_t*, 0xe000ed9c))

/** \brief Address of the MPU region attribute and size register */
#define MPU_RASR_REG            (* NANO_OS_CAST(volatile uint32_t*, 0xe000eda0))



/** \brief Initialize the MPU */
nano_os_error_t NANO_OS_PORT_MPU_Init(void)
{
    nano_os_error_t ret = NOS_ERR_MPU_NOT_AVAILABLE;

    /* Check if an MPU is available on this device */
    const uint32_t mpu_type_reg = MPU_TYPE_REG;
    const uint8_t mpu_region_count = (mpu_type_reg >> 8u);
    if ((mpu_type_reg != 0u) && (mpu_region_count >= NANO_OS_PORT_MPU_REGION_COUNT))
    {
        uint8_t i;

        /* Disable MPU */
        NANO_OS_MPU_Disable();

        /* Enable default memory map and disable MPU during hard fault, NMI, and FAULTMASK handlers*/
        MPU_CTRL_REG = (1u << 2u);

        /* Disable all MPU regions */
        for (i = 0u; i < mpu_region_count; i++)
        {
            /* Select region */
            MPU_RNR_REG = i;

            /* Disable region */
            MPU_RASR_REG = 0x00u;
            MPU_RBAR_REG = 0x00u;
        }

        ret = NOS_ERR_SUCCESS;
    }

    return ret;
}

/** \brief Enable MPU */
void NANO_OS_MPU_Enable(void)
{
    MPU_CTRL_REG |= 0x01u;
}

/** \brief Disable MPU */
void NANO_OS_MPU_Disable(void)
{
    MPU_CTRL_REG &= (~0x01u);
}

/** \brief Configure an MPU region */
void NANO_OS_MPU_ConfigureRegion(const uint32_t base_address,
                                 uint32_t attributes)
{
    /* Configure base address and region number */
    MPU_RBAR_REG = base_address;

    /* Configure attributes */
    MPU_RASR_REG = attributes;

    return;
}


/** \brief Compute the attributes fields of an MPU region */
nano_os_error_t NANO_OS_MPU_ComputeRegionAttributes(nano_os_port_mpu_region_t* const region, const uint32_t base_address, const bool execute_never,
                                                    const uint8_t access_permission, const uint8_t memory_access_attributes,
                                                    const bool shareable, const uint8_t subregion_enabled_bits,
                                                    const uint32_t size)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if (region != NULL)
    {
        /* Check attributes */
        if ((access_permission <= NANO_OS_PORT_MPU_ATTRIBUTE_MAX_AP_FIELD_VALUE) &&
            (memory_access_attributes <= NANO_OS_PORT_MPU_ATTRIBUTE_MAX_MEM_FIELD_VALUE) &&
            (size > NANO_OS_PORT_MPU_MIN_REGION_SIZE))
        {
            uint8_t i = 0;
            uint32_t region_size = 0;
            uint32_t new_attributes = 0u;

            /* Compute new values */
            if (execute_never)
            {
                new_attributes |= (1u << 28u);
            }
            new_attributes |= (access_permission << 24u);
            new_attributes |= (memory_access_attributes << 16u);
            if (shareable)
            {
                new_attributes |= (1u << 18u);
            }
            new_attributes |= (subregion_enabled_bits << 8u);
            if (size == 0xFFFFFFFFu)
            {
                i = 32u;
            }
            else
            {
                i = 0u;
                region_size = 1;
                while ( size > (1u << i))
                {
                    i++;
                    region_size |= region_size << 1u;
                }
            }
            region_size = region_size >> 1u;
            new_attributes |= ((i - 1) << 1u);

            /* Check base address alignment */
            if ((base_address & region_size) == 0)
            {
                /* Apply attributes */
                region->base_address = base_address;
                region->attributes = new_attributes;

                ret = NOS_ERR_SUCCESS;
            }
            else
            {
                ret = NOS_ERR_MPU_REGION_ALIGNMENT;
            }
        }
        else
        {
            ret = NOS_ERR_MPU_REGION_ATTRIBUTE;
        }
    }

    return ret;
}
