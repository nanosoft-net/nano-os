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

#ifndef NANO_OS_CPP_API_CFG_H
#define NANO_OS_CPP_API_CFG_H

/*********************************************************/
/*          Configuration of Nano OS C++ API             */
/*********************************************************/


/** \brief Enable the use of a dedicated namespace to put all the API classes */
#define NANO_OS_CPP_API_NAMESPACE_ENABLED       1u

/** \brief Name of namspace to to put all the API classes */
#define NANO_OS_CPP_API_NAMESPACE               nano_os_ns

/** \brief Overload new and delete operators to use Nano OS heap module */
#define NANO_OS_CPP_API_OVERLOAD_NEW_DELETE     1u




/** \brief Re-define NULL macro */
#define NANO_OS_CPP_API_NULL_MACRO_REDEFINE     1u

/* NULL macro re-definition */
#if (NANO_OS_CPP_API_NULL_MACRO_REDEFINE == 1u)
#ifdef NULL
#undef NULL
#endif // NULL
#if (__cplusplus >= 201103L)    
/** \brief NULL macro definition */
#define NULL    nullptr
#else
/** \brief NULL macro definition */
#define NULL    0
#endif // (__cplusplus >= 201103L)
#endif // (NANO_OS_CPP_API_NULL_MACRO_REDEFINE == 1u)


#endif // NANO_OS_CPP_API_CFG_H
