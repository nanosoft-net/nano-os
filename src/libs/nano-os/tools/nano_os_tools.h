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

#ifndef NANO_OS_TOOLS_H
#define NANO_OS_TOOLS_H

#include "nano_os_types.h"

#include <stdarg.h>

#if ( (NANO_OS_USE_STD_MEMSET == 1u) || (NANO_OS_USE_STD_MEMCPY == 1u) || (NANO_OS_USE_STD_MEMCMP == 1u) || \
      (NANO_OS_USE_STD_STRNCMP == 1u) || (NANO_OS_USE_STD_STRNLEN == 1u) || (NANO_OS_USE_STD_STRNCAT == 1u) )
#include <string.h>
#endif /* NANO_OS_USE_STD_MEMSET */

#if ((NANO_OS_USE_STD_VSNPRINTF == 1u) || (NANO_OS_USE_STD_SNPRINTF == 1u))
#include <stdio.h>
#endif /* ((NANO_OS_USE_STD_VSNPRINTF == 1u) || (NANO_OS_USE_STD_SNPRINTF == 1u)) */

#if ((NANO_OS_USE_STD_ATOI == 1u) || (NANO_OS_USE_STD_ITOA == 1u))
#include <stdlib.h>
#endif /* ((NANO_OS_USE_STD_ATOI == 1u) || (NANO_OS_USE_STD_ITOA == 1u)) */



#if (NANO_OS_USE_STD_MEMSET == 1u)

/** \brief Memset macro definition */
#define MEMSET(dst, val, size) memset((dst), (val), (size))

#else

/** \brief Memset macro definition */
#define MEMSET(dst, val, size) NANO_OS_memset((dst), (val), (size))

#endif /* (NANO_OS_USE_STD_MEMSET == 1u) */


#if (NANO_OS_USE_STD_MEMCPY == 1u)

/** \brief Memcpy macro definition */
#define MEMCPY(dst, src, size) memcpy((dst), (src), (size))

#else

/** \brief Memcpy macro definition */
#define MEMCPY(dst, src, size) NANO_OS_memcpy((dst), (src), (size))

#endif /* (NANO_OS_USE_STD_MEMCPY == 1u) */


#if (NANO_OS_USE_STD_MEMCMP == 1u)

/** \brief Memcmp macro definition */
#define MEMCMP(s1, s2, size) memcmp((s1), (s2), (size))

#else

/** \brief Memcmp macro definition */
#define MEMCMP(s1, s2, size) NANO_OS_memcmp((s1), (s2), (size))

#endif /* (NANO_OS_USE_STD_MEMCMP == 1u) */


#if (NANO_OS_USE_STD_STRNCMP == 1u)

/** \brief Strncmp macro definition */
#define STRNCMP(s1, s2, size) strncmp((s1), (s2), (size))

#else

/** \brief Strncmp macro definition */
#define STRNCMP(s1, s2, size) NANO_OS_strncmp((s1), (s2), (size))

#endif /* (NANO_OS_USE_STD_STRNCMP == 1u) */


#if (NANO_OS_USE_STD_STRNLEN == 1u)

/** \brief Strnlen macro definition */
#define STRNLEN(s, maxlen) strnlen((s), (maxlen))

#else

/** \brief Strnlen macro definition */
#define STRNLEN(s, maxlen) NANO_OS_strnlen((s), (maxlen))

#endif /* (NANO_OS_USE_STD_STRNLEN == 1u) */


#if (NANO_OS_USE_STD_STRNCAT == 1u)

/** \brief Strncat macro definition */
#define STRNCAT(dest, src, size) strncat((dest), (src), (size))

#else

/** \brief Strncat macro definition */
#define STRNCAT(dest, src, size) NANO_OS_strncat((dest), (src), (size))

#endif /* (NANO_OS_USE_STD_STRNCAT == 1u) */


#if (NANO_OS_USE_STD_VSNPRINTF == 1u)

/** \brief Vsnprintf macro definition */
#define VSNPRINTF(str, n, format, ap) vsnprintf((str), (n), (format), (ap))

#else

/** \brief Vsnprintf macro definition */
#define VSNPRINTF(str, n, format, ap) NANO_OS_vsnprintf((str), (n), (format), (ap))

#endif /* (NANO_OS_USE_STD_VSNPRINTF == 1u) */


#if (NANO_OS_USE_STD_SNPRINTF == 1u)

/** \brief Snprintf macro definition */
#define SNPRINTF(str, n, format, ...) snprintf((str), (n), (format), ##__VA_ARGS__)

#else

/** \brief Snprintf macro definition */
#define SNPRINTF(str, n, format, ...) NANO_OS_snprintf((str), (n), (format), ##__VA_ARGS__)

#endif /* (NANO_OS_USE_STD_SNPRINTF == 1u) */


#if (NANO_OS_USE_STD_ATOI == 1u)

/** \brief Atoi macro definition */
#define ATOI(str) atoi((str))

#else

/** \brief Atoi macro definition */
#define ATOI(str) NANO_OS_atoi((str))

#endif /* (NANO_OS_USE_STD_ATOI == 1u) */


#if (NANO_OS_USE_STD_ITOA == 1u)

/** \brief Itoa macro definition */
#define ITOA(value, str, base) itoa((value), (str), (base))

#else

/** \brief Itoa macro definition */
#define ITOA(value, str, base) NANO_OS_itoa((value), (str), (base))

#endif /* (NANO_OS_USE_STD_ITOA == 1u) */



#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#if (NANO_OS_USE_STD_MEMSET == 0u)
/** \brief Highly portable but non-efficient memset function */
void* NANO_OS_memset(void* const dst, const int val, size_t size);
#endif /* (NANO_OS_USE_STD_MEMSET == 0u) */

#if (NANO_OS_USE_STD_MEMCPY == 0u)
/** \brief Highly portable but non-efficient memcpy function */
void* NANO_OS_memcpy(void* const dst, const void* const src, size_t size);
#endif /* (NANO_OS_USE_STD_MEMCPY == 0u) */

#if (NANO_OS_USE_STD_MEMCMP == 0u)
/** \brief Highly portable but non-efficient memcmp function */
int NANO_OS_memcmp(const void* const s1, const void* const s2, size_t size);
#endif /* (NANO_OS_USE_STD_MEMCMP == 0u) */

#if (NANO_OS_USE_STD_STRNCMP == 0u)
/** \brief Highly portable but non-efficient strncmp function */
int NANO_OS_strncmp(const char* s1, const char* s2, size_t size);
#endif /* (NANO_OS_USE_STD_STRNCMP == 0u) */

#if (NANO_OS_USE_STD_STRNLEN == 0u)
/** \brief Highly portable but non-efficient strnlen function */
size_t NANO_OS_strnlen(const char* s, size_t maxlen);
#endif /* (NANO_OS_USE_STD_STRNLEN == 0u) */

#if (NANO_OS_USE_STD_STRNCAT == 0u)
/** \brief Highly portable but non-efficient strncat function */
char* NANO_OS_strncat(char *dest, const char *src, size_t size);
#endif /* (NANO_OS_USE_STD_STRNCAT == 0u) */

#if (NANO_OS_USE_STD_VSNPRINTF == 0u)
/** \brief Highly portable but non-efficient vsnprintf function */
int NANO_OS_vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif /* (NANO_OS_USE_STD_VSNPRINTF == 0u) */

#if (NANO_OS_USE_STD_SNPRINTF == 0u)
/** \brief Highly portable but non-efficient snprintf function */
int NANO_OS_snprintf(char *str, size_t size, const char *format, ...);
#endif /* (NANO_OS_USE_STD_SNPRINTF == 0u) */

#if (NANO_OS_USE_STD_ATOI == 0u)
/** \brief Higly portable but non-efficient atoi function */
int NANO_OS_atoi(const char* str);
#endif /* (NANO_OS_USE_STD_ATOI == 0u) */

#if (NANO_OS_USE_STD_ITOA == 0u)
/** \brief Higly portable but non-efficient itoa function */
char* NANO_OS_itoa(int value, char * str, int base);
#endif /* (NANO_OS_USE_STD_ITOA == 0u) */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NANO_OS_TOOLS_H */
