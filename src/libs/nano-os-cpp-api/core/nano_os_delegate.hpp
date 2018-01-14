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

#ifndef NANO_OS_DELEGATE_HPP
#define NANO_OS_DELEGATE_HPP

#include "nano_os_types.h"
#include "nano_os_cpp_api_cfg.h"


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

// 0 parameter delegate
#define DELEGATE_PARAM_COUNT                0
#define DELEGATE_PARAM_SEP
#define DELEGATE_TEMPLATE_PARAM_DEF_LIST
#define DELEGATE_TEMPLATE_PARAM_TYPES_LIST
#define DELEGATE_METHOD_PARAM_TYPES_LIST
#define DELEGATE_METHOD_PARAM_LIST

#include "nano_os_delegate.tpl"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_PARAM_SEP
#undef DELEGATE_TEMPLATE_PARAM_DEF_LIST
#undef DELEGATE_TEMPLATE_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_LIST

// 1 parameter delegate
#define DELEGATE_PARAM_COUNT                1
#define DELEGATE_PARAM_SEP                  ,
#define DELEGATE_TEMPLATE_PARAM_DEF_LIST    typename Arg0Type
#define DELEGATE_TEMPLATE_PARAM_TYPES_LIST  Arg0Type
#define DELEGATE_METHOD_PARAM_TYPES_LIST    Arg0Type arg0
#define DELEGATE_METHOD_PARAM_LIST          arg0

#include "nano_os_delegate.tpl"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_PARAM_SEP
#undef DELEGATE_TEMPLATE_PARAM_DEF_LIST
#undef DELEGATE_TEMPLATE_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_LIST

// 2 parameters delegate
#define DELEGATE_PARAM_COUNT                2
#define DELEGATE_PARAM_SEP                  ,
#define DELEGATE_TEMPLATE_PARAM_DEF_LIST    typename Arg0Type, typename Arg1Type
#define DELEGATE_TEMPLATE_PARAM_TYPES_LIST  Arg0Type, Arg1Type
#define DELEGATE_METHOD_PARAM_TYPES_LIST    Arg0Type arg0, Arg1Type arg1
#define DELEGATE_METHOD_PARAM_LIST          arg0, arg1

#include "nano_os_delegate.tpl"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_PARAM_SEP
#undef DELEGATE_TEMPLATE_PARAM_DEF_LIST
#undef DELEGATE_TEMPLATE_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_LIST

// 3 parameters delegate
#define DELEGATE_PARAM_COUNT                3
#define DELEGATE_PARAM_SEP                  ,
#define DELEGATE_TEMPLATE_PARAM_DEF_LIST    typename Arg0Type, typename Arg1Type, typename Arg2Type
#define DELEGATE_TEMPLATE_PARAM_TYPES_LIST  Arg0Type, Arg1Type, Arg2Type
#define DELEGATE_METHOD_PARAM_TYPES_LIST    Arg0Type arg0, Arg1Type arg1, Arg2Type arg2
#define DELEGATE_METHOD_PARAM_LIST          arg0, arg1, arg2

#include "nano_os_delegate.tpl"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_PARAM_SEP
#undef DELEGATE_TEMPLATE_PARAM_DEF_LIST
#undef DELEGATE_TEMPLATE_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_LIST

// 4 parameters delegate
#define DELEGATE_PARAM_COUNT                4
#define DELEGATE_PARAM_SEP                  ,
#define DELEGATE_TEMPLATE_PARAM_DEF_LIST    typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type
#define DELEGATE_TEMPLATE_PARAM_TYPES_LIST  Arg0Type, Arg1Type, Arg2Type, Arg3Type
#define DELEGATE_METHOD_PARAM_TYPES_LIST    Arg0Type arg0, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3
#define DELEGATE_METHOD_PARAM_LIST          arg0, arg1, arg2, arg3

#include "nano_os_delegate.tpl"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_PARAM_SEP
#undef DELEGATE_TEMPLATE_PARAM_DEF_LIST
#undef DELEGATE_TEMPLATE_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_LIST

// 5 parameters delegate
#define DELEGATE_PARAM_COUNT                5
#define DELEGATE_PARAM_SEP                  ,
#define DELEGATE_TEMPLATE_PARAM_DEF_LIST    typename Arg0Type, typename Arg1Type, typename Arg2Type, typename Arg3Type, typename Arg4Type
#define DELEGATE_TEMPLATE_PARAM_TYPES_LIST  Arg0Type, Arg1Type, Arg2Type, Arg3Type, Arg4Type
#define DELEGATE_METHOD_PARAM_TYPES_LIST    Arg0Type arg0, Arg1Type arg1, Arg2Type arg2, Arg3Type arg3, Arg4Type arg4
#define DELEGATE_METHOD_PARAM_LIST          arg0, arg1, arg2, arg3, arg4

#include "nano_os_delegate.tpl"

#undef DELEGATE_PARAM_COUNT
#undef DELEGATE_PARAM_SEP
#undef DELEGATE_TEMPLATE_PARAM_DEF_LIST
#undef DELEGATE_TEMPLATE_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_TYPES_LIST
#undef DELEGATE_METHOD_PARAM_LIST


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // NANO_OS_DELEGATE_HPP
