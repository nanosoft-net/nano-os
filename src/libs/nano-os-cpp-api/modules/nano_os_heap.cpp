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

#include "nano_os_heap.hpp"

// Check if module is enabled
#if (NANO_OS_HEAP_ENABLED == 1u)

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

/** \brief Unique instance */
NanoOsHeap* NanoOsHeap::m_singleton = NULL;

/** \brief Default constructor */
NanoOsHeap::NanoOsHeap()
{
    // Check singleton
    if (m_singleton == NULL)
    {
        m_singleton = this;
    }
    else
    {
        // Only one instance allowed
        NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);
    }
}

/** \brief Destructor */
NanoOsHeap::~NanoOsHeap()
{
    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);
}

/** \brief Initialize the heap module */
nano_os_error_t NanoOsHeap::init(void* const heap_area_start, const size_t heap_area_size)
{
    return NANO_OS_HEAP_Init(heap_area_start, heap_area_size);
}

/** \brief Allocate memory from the heap */
void* alloc(const size_t mem_size)
{
    return NANO_OS_HEAP_Alloc(mem_size);
}

#if (NANO_OS_HEAP_FREE_ENABLED == 1u)
/** \brief De-allocate previously allocated memory from the heap */
void free(void* mem)
{
    NANO_OS_HEAP_Free(mem);
}
#endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */

#if (NANO_OS_HEAP_GETSTATS_ENABLED == 1u)
/** \brief Get the heap module statistics */
nano_os_error_t getStats(nano_os_heap_stats_t& heap_stats)
{
    return NANO_OS_HEAP_GetStats(&heap_stats);
}
#endif /* (NANO_OS_HEAP_GETSTATS_ENABLED == 1u) */




#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)


// New and delete operators overload
#if (NANO_OS_CPP_API_OVERLOAD_NEW_DELETE == 1u)

/** \brief new operator overload */
void* operator new(size_t count)
{
    return NANO_OS_HEAP_Alloc(count);
}

/** \brief new operator for arrays overload */
void* operator new[](size_t count)
{
    return NANO_OS_HEAP_Alloc(count);
}

#if (NANO_OS_HEAP_FREE_ENABLED == 1u)

/** \brief delete operator overload */
void operator delete(void* ptr)
{
    return NANO_OS_HEAP_Free(ptr);
}

/** \brief delete operator for arrays overload */
void operator delete[](void* ptr)
{
    return NANO_OS_HEAP_Free(ptr);
}

#endif /* (NANO_OS_HEAP_FREE_ENABLED == 1u) */

#endif /* (NANO_OS_CPP_API_OVERLOAD_NEW_DELETE == 1u) */

#endif // (NANO_OS_HEAP_ENABLED == 1u)
