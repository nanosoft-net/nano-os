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

#include "nano_os_console.hpp"

// Check if module is enabled
#if (NANO_OS_CONSOLE_ENABLED == 1u)

#include "nano_os_user.h"

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

/** \brief Unique instance */
NanoOsConsole* NanoOsConsole::m_singleton = NULL;

/** \brief Default constructor */
NanoOsConsole::NanoOsConsole()
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
NanoOsConsole::~NanoOsConsole()
{
    // Should never happen
    NANO_OS_ERROR_Handler(NOS_ERR_FAILURE);
}

/** \brief Register commands */
nano_os_error_t NanoOsConsole::registerCommands(INanoOsConsoleCmdGroup& command_group)
{
    return NANO_OS_CONSOLE_RegisterCommands(&command_group.getDescription());
}

/** \brief Get the next parameter in the command line */
const char* NanoOsConsole::getNextParam(const char* const current_param)
{
    return NANO_OS_CONSOLE_GetNextParam(current_param);
}

/** \brief Write a string on the console link */
nano_os_error_t NanoOsConsole::writeString(const char* const string)
{
    return NANO_OS_USER_ConsoleWriteString(string);
}


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_CONSOLE_ENABLED == 1u)
