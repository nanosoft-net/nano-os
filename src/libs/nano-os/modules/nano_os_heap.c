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

#include "nano_os_heap.h"

/* Check if module is enabled */
#if (NANO_OS_HEAP_ENABLED == 1u)

#include "nano_os_data.h"
#include "nano_os_user.h"
#include "nano_os_syscall.h"
#include "nano_os_tools.h"

/** \brief Magic number for the start of an allocated block */
#define NANO_OS_HEAP_ALLOC_START_MAGIC          0x8BADF00Du

/** \brief Magic number for the start of a free block */
#define NANO_OS_HEAP_FREE_START_MAGIC           0xDEADBABEu

/** \brief Minimum allocation size in bytes (to preserve memory alignment) */
#define NANO_OS_HEAP_MIN_ALLOC_SIZE             4u


#if (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u)

/** \brief Handle the 'heap' console command */
static void NANO_OS_HEAP_ConsoleCmdHandler(void* const user_data, const uint32_t command_id, const char* const params);

/** \brief Console commands for the heap module */
static const nano_os_console_cmd_desc_t heap_module_commands[] = {
                                                                    {"heap", "Display heap statistics", NANO_OS_HEAP_ConsoleCmdHandler},
                                                                 };

#endif /* (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u) */




/** \brief Initialize the heap module */
nano_os_error_t NANO_OS_HEAP_Init(void* const heap_area_start, const size_t heap_area_size)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Check parameters */
    if ((heap_area_start != NULL) && (heap_area_size > sizeof(nano_os_heap_block_header_t)))
    {
        /* Save heap description */
        g_nano_os.heap_start = heap_area_start;
        g_nano_os.heap_size = heap_area_size;

        #if (NANO_OS_HEAP_FREE_ENABLED == 1u)
        g_nano_os.first_allocated = NULL;
        #endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */

        /* Create first block */
        g_nano_os.first_free = NANO_OS_CAST(nano_os_heap_block_header_t*, heap_area_start);
        (void)MEMSET(g_nano_os.first_free, 0, sizeof(nano_os_heap_block_header_t));
        g_nano_os.first_free->start_magic = NANO_OS_HEAP_FREE_START_MAGIC;
        g_nano_os.first_free->size = heap_area_size - sizeof(nano_os_heap_block_header_t);

        #if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
        /* Initialize stats */
        g_nano_os.heap_stats.free_blocks = 1u;
        g_nano_os.heap_stats.free_memory = heap_area_size - sizeof(nano_os_heap_block_header_t);
        g_nano_os.heap_stats.allocated_blocks = 0u;
        g_nano_os.heap_stats.allocated_memory = 0u;
        #endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */

        /* Initialize heap mutex */
        ret = NANO_OS_MUTEX_Create(&g_nano_os.heap_mutex, QT_PRIORITY);

        #if (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u)
        /* Register heap console commands */
        if (ret == NOS_ERR_SUCCESS)
        {
            g_nano_os.heap_cmd_group.commands = heap_module_commands;
            g_nano_os.heap_cmd_group.command_count = sizeof(heap_module_commands) / sizeof(nano_os_console_cmd_desc_t);
            ret = NANO_OS_CONSOLE_RegisterCommands(&g_nano_os.heap_cmd_group);
        }
        #endif /* (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u) */
    }

    return ret;
}

/** \brief Allocate memory from the heap */
void* NANO_OS_HEAP_Alloc(const size_t mem_size)
{
    void* mem_allocated = NULL;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if (mem_size != 0u)
    {
        nano_os_error_t ret;

        /* Compute the block size to allocate */
        uint32_t total_free_size_needed = sizeof(nano_os_heap_block_header_t);
        uint32_t allocate_size = NANO_OS_CAST(uint32_t, mem_size);
        if (allocate_size < NANO_OS_HEAP_MIN_ALLOC_SIZE)
        {
            allocate_size = NANO_OS_HEAP_MIN_ALLOC_SIZE;
        }
        /* Align allocated size on 32bits */
        while ((allocate_size & 3u) != 0u)
        {
            allocate_size++;
        }
        total_free_size_needed += allocate_size;

        /* Lock the heap */
        ret = NANO_OS_MUTEX_Lock(&g_nano_os.heap_mutex, 0xFFFFFFFFu);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Look for a free block with at least the needed size */
            #if (NANO_OS_HEAP_FREE_ENABLED == 1u)
            nano_os_heap_block_header_t* previous_free_block = NULL;
            nano_os_heap_block_header_t* free_block = g_nano_os.first_free;
            while ((free_block != NULL) &&
                   (free_block->size < allocate_size))
            {
                previous_free_block = free_block;
                free_block = free_block->next_in_list;
            }
            #else
            nano_os_heap_block_header_t* free_block = g_nano_os.first_free;
            if ((free_block == NULL) ||
                (free_block->size < total_free_size_needed))
            {
                free_block = NULL;
            }
            #endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */
            if (free_block != NULL)
            {
                uint32_t size_left_in_block;

                /* Check block validity */
                NANO_OS_ERROR_ASSERT((free_block->start_magic == NANO_OS_HEAP_FREE_START_MAGIC), NOS_ERR_CORRUPTED_HEAP);

                /* Compute the size left in the free block */
                size_left_in_block = free_block->size - allocate_size;
                if (size_left_in_block > sizeof(nano_os_heap_block_header_t))
                {
                    /* Allocate a new block at the end of the free block */
                    nano_os_heap_block_header_t* new_free_block = NANO_OS_CAST(nano_os_heap_block_header_t*,
                                                                               NANO_OS_CAST(uint8_t*, free_block) + total_free_size_needed);
                    new_free_block->start_magic = NANO_OS_HEAP_FREE_START_MAGIC;
                    new_free_block->size = size_left_in_block - sizeof(nano_os_heap_block_header_t);
                    free_block->size = allocate_size;

                    #if (NANO_OS_HEAP_FREE_ENABLED == 1u)
                    new_free_block->next_in_list = free_block->next_in_list;
                    free_block->next_in_list = new_free_block;

                    #if (NANO_OS_HEAP_MERGE_FREE_BLOCKS_ENABLED == 1u)
                    new_free_block->previous = free_block;
                    new_free_block->next = free_block->next;
                    free_block->next = new_free_block;
                    #endif /* (NANO_OS_HEAP_MERGE_FREE_BLOCKS_ENABLED == 1u) */

                    #else
                    g_nano_os.first_free = new_free_block;
                    #endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */

                    #if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
                    /* Update stats */
                    g_nano_os.heap_stats.free_blocks++;
                    g_nano_os.heap_stats.free_memory -= sizeof(nano_os_heap_block_header_t);
                    #endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */
                }
                #if (NANO_OS_HEAP_FREE_ENABLED != 1u)
                else
                {
                    g_nano_os.first_free = NULL;
                }
                #endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */

                /* Allocate data in the free block */
                free_block->start_magic = NANO_OS_HEAP_ALLOC_START_MAGIC;
                mem_allocated = NANO_OS_CAST(nano_os_heap_block_header_t*,
                                             NANO_OS_CAST(uint8_t*, free_block) + sizeof(nano_os_heap_block_header_t));

                /* Update the free and allocated lists */
                #if (NANO_OS_HEAP_FREE_ENABLED == 1u)
                if (previous_free_block != NULL)
                {
                    previous_free_block->next_in_list = free_block->next_in_list;
                }
                else
                {
                    g_nano_os.first_free = free_block->next_in_list;
                }
                free_block->next_in_list = g_nano_os.first_allocated;
                g_nano_os.first_allocated = free_block;
                #endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */

                #if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
                /* Update stats */
                g_nano_os.heap_stats.allocated_blocks++;
                g_nano_os.heap_stats.allocated_memory += allocate_size;
                g_nano_os.heap_stats.free_blocks--;
                g_nano_os.heap_stats.free_memory -= allocate_size;
                #endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */
            }

            /* Unlock the heap */
            (void)NANO_OS_MUTEX_Unlock(&g_nano_os.heap_mutex);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return mem_allocated;
}


#if (NANO_OS_HEAP_FREE_ENABLED == 1u)

/** \brief De-allocate previously allocated memory from the heap */
void NANO_OS_HEAP_Free(void* mem)
{
    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if (mem != NULL)
    {
        nano_os_error_t ret;

        /* Check memory area validity */
        nano_os_heap_block_header_t* mem_block = NANO_OS_CAST(nano_os_heap_block_header_t*,
                                                              NANO_OS_CAST(uint8_t*, mem) - sizeof(nano_os_heap_block_header_t));
        NANO_OS_ERROR_ASSERT((mem_block->start_magic == NANO_OS_HEAP_ALLOC_START_MAGIC), NOS_ERR_CORRUPTED_HEAP);

        /* Lock the heap */
        ret = NANO_OS_MUTEX_Lock(&g_nano_os.heap_mutex, 0xFFFFFFFFu);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Look for the block in allocated list */
            nano_os_heap_block_header_t* previous_block = NULL;
            nano_os_heap_block_header_t* current_block = g_nano_os.first_allocated;
            while ((current_block != NULL) &&
                   (current_block != mem_block))
            {
                previous_block = current_block;
                current_block = current_block->next_in_list;
            }
            if (current_block != NULL)
            {
                /* Remove block from allocated list */
                mem_block->start_magic = NANO_OS_HEAP_FREE_START_MAGIC;
                if (previous_block != NULL)
                {
                    previous_block->next_in_list = mem_block->next_in_list;
                }
                else
                {
                    g_nano_os.first_allocated = mem_block->next_in_list;
                }
                mem_block->next_in_list = g_nano_os.first_free;
                g_nano_os.first_free = mem_block;

                #if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
                /* Update stats */
                g_nano_os.heap_stats.allocated_blocks--;
                g_nano_os.heap_stats.allocated_memory -= mem_block->size;
                g_nano_os.heap_stats.free_blocks++;
                g_nano_os.heap_stats.free_memory += mem_block->size;
                #endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */

                #if (NANO_OS_HEAP_MERGE_FREE_BLOCKS_ENABLED == 1u)
                /* Try to merge this memory block with the previous in memory
                   to obtain a larger free block */
                if ((mem_block->previous != NULL) &&
                    (mem_block->previous->start_magic == NANO_OS_HEAP_FREE_START_MAGIC))
                {
                    g_nano_os.first_free = mem_block->next_in_list;

                    mem_block->previous->size += mem_block->size + sizeof(nano_os_heap_block_header_t);
                    mem_block->previous->next = mem_block->next;
                    if (mem_block->next != NULL)
                    {
                        mem_block->next->previous = mem_block->previous;
                    }
                    mem_block->start_magic = 0u;
                    mem_block = mem_block->previous;

                    #if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
                    /* Update stats */
                    g_nano_os.heap_stats.free_blocks--;
                    g_nano_os.heap_stats.free_memory += sizeof(nano_os_heap_block_header_t);
                    #endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */
                }

                /* Try to merge this memory block with the next in memory
                   to obtain a larger free block */
                if ((mem_block->next != NULL) &&
                    (mem_block->next->start_magic == NANO_OS_HEAP_FREE_START_MAGIC))
                {
                    previous_block = NULL;
                    current_block = g_nano_os.first_free;
                    while ((current_block != NULL) &&
                           (current_block != mem_block->next))
                    {
                        previous_block = current_block;
                        current_block = current_block->next_in_list;
                    }
                    NANO_OS_ERROR_ASSERT(previous_block != NULL, NOS_ERR_CORRUPTED_HEAP);
                    previous_block->next_in_list = mem_block->next->next_in_list;

                    mem_block->next->start_magic = 0u;
                    mem_block->size += mem_block->next->size + sizeof(nano_os_heap_block_header_t);
                    mem_block->next = mem_block->next->next;
                    if (mem_block->next != NULL)
                    {
                        mem_block->next->previous = mem_block;
                    }

                    #if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
                    /* Update stats */
                    g_nano_os.heap_stats.free_blocks--;
                    g_nano_os.heap_stats.free_memory += sizeof(nano_os_heap_block_header_t);
                    #endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */
                }
                #endif /* (NANO_OS_HEAP_MERGE_FREE_BLOCKS_ENABLED == 1u) */
            }
            else
            {
                /* This block was not previously allocated or heap is corrupted */
                NANO_OS_ERROR_Handler(NOS_ERR_CORRUPTED_HEAP);
            }

            /* Unlock the heap */
            (void)NANO_OS_MUTEX_Unlock(&g_nano_os.heap_mutex);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return;
}

#endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */


#if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)

/** \brief Get the heap module statistics */
nano_os_error_t NANO_OS_HEAP_GetStats(nano_os_heap_stats_t* const heap_stats)
{
    nano_os_error_t ret = NOS_ERR_INVALID_ARG;

    /* Syscall entry */
    NANO_OS_SYSCALL_Enter(true);

    /* Check parameters */
    if (heap_stats != NULL)
    {
        /* Lock the heap */
        ret = NANO_OS_MUTEX_Lock(&g_nano_os.heap_mutex, 0xFFFFFFFFu);
        if (ret == NOS_ERR_SUCCESS)
        {
            /* Copy statistics */
            (void)MEMCPY(heap_stats, &g_nano_os.heap_stats, sizeof(nano_os_heap_stats_t));

            /* Unlock the heap */
            ret = NANO_OS_MUTEX_Unlock(&g_nano_os.heap_mutex);
        }
    }

    /* Syscall exit */
    NANO_OS_SYSCALL_Exit();

    return ret;
}

#endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */


#if (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u)

/** \brief Handle the 'heap' console command */
static void NANO_OS_HEAP_ConsoleCmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
{
    nano_os_error_t ret;
    nano_os_heap_stats_t heap_stats;
    NANO_OS_UNUSED(user_data);
    NANO_OS_UNUSED(command_id);

    /* Get the statistics */
    ret = NANO_OS_HEAP_GetStats(&heap_stats);
    if (ret == NOS_ERR_SUCCESS)
    {
        char temp_str[10u];

        /* Display statistics */
        (void)NANO_OS_USER_ConsoleWriteString("Heap statistics :\r\n");
        (void)NANO_OS_USER_ConsoleWriteString(" - Total size : ");
        (void)ITOA(g_nano_os.heap_size, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Allocated memory : ");
        (void)ITOA(heap_stats.allocated_memory, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Allocated blocks : ");
        (void)ITOA(heap_stats.allocated_blocks, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Free memory : ");
        (void)ITOA(heap_stats.free_memory, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        (void)NANO_OS_USER_ConsoleWriteString("\r\n - Free blocks : ");
        (void)ITOA(heap_stats.free_blocks, temp_str, 10u);
        (void)NANO_OS_USER_ConsoleWriteString(temp_str);
        (void)NANO_OS_USER_ConsoleWriteString("\r\n");
    }
    else
    {
        (void)NANO_OS_USER_ConsoleWriteString("Unable to get heap statistics\r\n");
    }
}

#endif /* (NANO_OS_HEAP_CONSOLE_CMD_ENABLED == 1u) */

#endif /* (NANO_OS_HEAP_ENABLED == 1u) */
