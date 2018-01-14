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

#ifndef NANO_OS_PORT_MPU_H
#define NANO_OS_PORT_MPU_H

#include "nano_os_types.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief MPU region attribute AP field : All accesses generate a permission fault */
#define NANO_OS_PORT_MPU_ATTR_AP_NO_ACCESS                  0x00u

/** \brief MPU region attribute AP field : Access from privileged software only */
#define NANO_OS_PORT_MPU_ATTR_AP_PRIVILEDGED_ONLY           0x01u

/** \brief MPU region attribute AP field : Writes by unprivileged software generate a permission fault  */
#define NANO_OS_PORT_MPU_ATTR_AP_UNPRIVILEDGED_READ_ONLY    0x02u

/** \brief MPU region attribute AP field : Full access  */
#define NANO_OS_PORT_MPU_ATTR_AP_FULL_ACCESS                0x03u

/** \brief MPU region attribute AP field : Reserved, do not use this value  */
#define NANO_OS_PORT_MPU_ATTR_AP_RESERVED                   0x04u

/** \brief MPU region attribute AP field : Reads by privileged software only  */
#define NANO_OS_PORT_MPU_ATTR_AP_PRIVILEDGED_READ_ONLY      0x05u

/** \brief MPU region attribute AP field : Read only, by privileged or unprivileged software  */
#define NANO_OS_PORT_MPU_ATTR_AP_READ_ONLY                  0x06u

/** \brief MPU region attribute AP field : Read only, by privileged or unprivileged software  */
#define NANO_OS_PORT_MPU_ATTR_AP_READ_ONLY2                 0x07u



/** \brief MPU region attribute memory access attribute fields : Strongly ordered - Shareable */
#define NANO_OS_PORT_MPU_ATTR_MEM_STRONGLY_ORDERED_SHAREABLE                                0x000u

/** \brief MPU region attribute memory access attribute fields : Device - Shareable */
#define NANO_OS_PORT_MPU_ATTR_MEM_DEVICE_SHAREABLE                                          0x001u

/** \brief MPU region attribute memory access attribute fields : Outer and inner write-through. No write allocate. */
#define NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_TROUGH_NO_WRITE_ALLOC                   0x002u

/** \brief MPU region attribute memory access attribute fields : Outer and inner write-back. No write allocate. */
#define NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_NO_WRITE_ALLOC                     0x003u

/** \brief MPU region attribute memory access attribute fields : Outer and inner noncacheable. */
#define NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_NON_CACHEABLE                                 0x008u

/** \brief MPU region attribute memory access attribute fields : Outer and inner write-back. Write and read allocate. */
#define NANO_OS_PORT_MPU_ATTR_MEM_OUTER_INNER_WRITE_BACK_READ_WRITE_ALLOC                   0x00Bu

/** \brief MPU region attribute memory access attribute fields : Nonshared Device. */
#define NANO_OS_PORT_MPU_ATTR_MEM_DEVICE_NOT_SHAREABLE                                      0x010u

/** \brief MPU region attribute memory access attribute fields : Cached memory */
#define NANO_OS_PORT_MPU_ATTR_MEM_CACHED(outer_policy, inner_policy)                        (0x020u | (((outer_policy) & 0x03u) << 3u) | ((inner_policy) & 0x03u))


/** \brief MPU region attribute cache policy for cached memory : Non-cacheable */
#define NANO_OS_PORT_MPU_CACHE_POLICY_NON_CACHEABLE             0x00u

/** \brief MPU region attribute cache policy for cached memory : Write back, write and read allocate */
#define NANO_OS_PORT_MPU_CACHE_POLICY_WRITE_BACK_RW_ALLOC       0x01u

/** \brief MPU region attribute cache policy for cached memory : Write through, no write allocate */
#define NANO_OS_PORT_MPU_CACHE_POLICY_WRITE_THROUGH_R_ALLOC     0x02u

/** \brief MPU region attribute cache policy for cached memory : Write back, no write allocate */
#define NANO_OS_PORT_MPU_CACHE_POLICY_WRITE_BACK_NO_ALLOC       0x03u


/** \brief MPU region, subregions bits : enable all subregions */
#define NANO_OS_PORT_MPU_SUBREGION_ENABLE_ALL       0xFFu


/** \brief Minimum size in bytes for an MPU region */
#define NANO_OS_PORT_MPU_MIN_REGION_SIZE            32u




/** \brief Initialize the MPU */
nano_os_error_t NANO_OS_PORT_MPU_Init(void);

/** \brief Enable MPU */
void NANO_OS_MPU_Enable(void);

/** \brief Disable MPU */
void NANO_OS_MPU_Disable(void);

/** \brief Configure an MPU region */
void NANO_OS_MPU_ConfigureRegion(const uint32_t base_address,
                                 uint32_t attributes);

/** \brief Compute the attributes fields of an MPU region */
nano_os_error_t NANO_OS_MPU_ComputeRegionAttributes(uint32_t* const attributes, const bool execute_never,
                                                    const uint8_t access_permission, const uint8_t memory_access_attributes,
                                                    const bool shareable, const uint8_t subregion_enabled_bits,
                                                    const uint32_t size);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_PORT_MPU_H */
