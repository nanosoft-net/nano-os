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

#ifndef NANO_OS_HEAP_H
#define NANO_OS_HEAP_H

#include "nano_os_types.h"

/* Check if module is enabled */
#if (NANO_OS_HEAP_ENABLED == 1u)


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */



/** \brief Heap block header */
typedef struct _nano_os_heap_block_header_t
{
    /** \brief Start magic */
    uint32_t start_magic;
    /** \brief Block size */
    uint32_t size;

    #if (NANO_OS_HEAP_FREE_ENABLED == 1u)
    /** \brief Next block in list */
    struct _nano_os_heap_block_header_t* next_in_list;

    #if (NANO_OS_HEAP_MERGE_FREE_BLOCKS_ENABLED == 1u)
    /** \brief Previous block */
    struct _nano_os_heap_block_header_t* previous;
    /** \brief Next block */
    struct _nano_os_heap_block_header_t* next;
    #endif /* (NANO_OS_HEAP_MERGE_FREE_BLOCKS_ENABLED == 1u) */

    #endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */
} nano_os_heap_block_header_t;


#if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
/** \brief Heap statistics */
typedef struct _nano_os_heap_stats_t
{
    /** \brief Total allocated memory size in bytes */
    uint32_t allocated_memory;
    /** \brief Total allocated block count */
    uint32_t allocated_blocks;
    /** \brief Total free memory size in bytes */
    uint32_t free_memory;
    /** \brief Total free block count */
    uint32_t free_blocks;
} nano_os_heap_stats_t;
#endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */


/** \brief Initialize the heap module */
nano_os_error_t NANO_OS_HEAP_Init(void* const heap_area_start, const size_t heap_area_size);

/** \brief Allocate memory from the heap */
void* NANO_OS_HEAP_Alloc(const size_t mem_size);

#if (NANO_OS_HEAP_FREE_ENABLED == 1u)
/** \brief De-allocate previously allocated memory from the heap */
void NANO_OS_HEAP_Free(void* mem);
#endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */

#if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
/** \brief Get the heap module statistics */
nano_os_error_t NANO_OS_HEAP_GetStats(nano_os_heap_stats_t* const heap_stats);
#endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* (NANO_OS_HEAP_ENABLED == 1u) */

#endif /* NANO_OS_HEAP_H */
