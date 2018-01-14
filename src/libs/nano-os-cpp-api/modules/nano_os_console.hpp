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

#ifndef NANO_OS_CONSOLE_HPP
#define NANO_OS_CONSOLE_HPP

#include "nano_os_console.h"
#include "nano_os_cpp_api_cfg.h"

// Check if module is enabled
#if (NANO_OS_CONSOLE_ENABLED == 1u)

#include "nano_os_delegate.hpp"

#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
namespace NANO_OS_CPP_API_NAMESPACE
{
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

// Pre-declarations
class INanoOsConsoleCmdGroup;

/** \brief Nano Os console module */
class NanoOsConsole
{
    public:

        /** \brief Default constructor */
        NanoOsConsole();

        /** \brief Destructor */
        ~NanoOsConsole();

        
        /** \brief Register commands */
        nano_os_error_t registerCommands(INanoOsConsoleCmdGroup& command_group);

        /** \brief Get the next parameter in the command line */
        const char* getNextParam(const char* const current_param);

        /** \brief Write a string on the console link */
        nano_os_error_t writeString(const char* const string);


        /** \brief Delegate type for console command handler method */
        typedef NanoOsDelegate1<void, const char*> CommandHandlerMethod;


    private:

        /** \brief Copy constructor => forbidden */
        NanoOsConsole(const NanoOsConsole& copy) { NANO_OS_UNUSED(copy); }

        /** \brief Unique instance */
        static NanoOsConsole* m_singleton;
};



/** \brief Console command group interface */
class INanoOsConsoleCmdGroup
{
    public:

        /** \brief Destructeur */
        virtual ~INanoOsConsoleCmdGroup() {}

        /** \brief Get the group description */
        virtual nano_os_console_cmd_group_desc_t& getDescription() = 0;
};

/** \brief Console command group */
template <uint32_t MAX_CMD_COUNT>
class NanoOsConsoleCmdGroup : public INanoOsConsoleCmdGroup
{
    public:

        /** \brief Default constructor */
        NanoOsConsoleCmdGroup()
            : INanoOsConsoleCmdGroup()
            , m_cmd_group_desc()
        {
            m_cmd_group_desc.commands = m_cmd_descs;
            m_cmd_group_desc.command_count = 0u;
            m_cmd_group_desc.user_data = this;
        }

        /** \brief Destructor */
        virtual ~NanoOsConsoleCmdGroup()
        {}

        /** \brief Get the group description */
        virtual nano_os_console_cmd_group_desc_t& getDescription()
        {
            return m_cmd_group_desc;
        }

        /** \brief Add a command */
        nano_os_error_t addCommand(const char* name, const char* help,
                                   const NanoOsConsole::CommandHandlerMethod& cmd_delegate)
        {
            nano_os_error_t ret = NOS_ERR_FAILURE;

            // Check command count
            if (m_cmd_group_desc.command_count < MAX_CMD_COUNT)
            {
                // Fill command description
                nano_os_console_cmd_desc_t& cmd_desc = m_cmd_descs[m_cmd_group_desc.command_count];
                cmd_desc.name = name;
                cmd_desc.help = help;
                cmd_desc.handler = &NanoOsConsoleCmdGroup<MAX_CMD_COUNT>::cmdHandler;

                // Save delegate
                m_cmd_delegates[m_cmd_group_desc.command_count] = cmd_delegate;

                // Update command count
                m_cmd_group_desc.command_count++;
            }

            return ret;
        }

    private:

        /** \brief Console command group description */
        nano_os_console_cmd_group_desc_t m_cmd_group_desc;

        /** \brief Console command descriptions */
        nano_os_console_cmd_desc_t m_cmd_descs[MAX_CMD_COUNT];

        /** \brief Console command delegates */
        NanoOsConsole::CommandHandlerMethod m_cmd_delegates[MAX_CMD_COUNT];


        /** \brief Generic console command handler */
        static void cmdHandler(void* const user_data, const uint32_t command_id, const char* const params)
        {
            // Get command group
            NanoOsConsoleCmdGroup<MAX_CMD_COUNT>* const cmd_group = reinterpret_cast<NanoOsConsoleCmdGroup<MAX_CMD_COUNT>*>(user_data);
            if (cmd_group != NULL)
            {
                // Check command id range
                if (command_id < cmd_group->m_cmd_group_desc.command_count)
                {
                    // Call command handler
                    cmd_group->m_cmd_delegates[command_id].invoke(params);
                }
            }
        }
};


#if (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)
}
#endif // (NANO_OS_CPP_API_NAMESPACE_ENABLED == 1u)

#endif // (NANO_OS_CONSOLE_ENABLED == 1u)

#endif // NANO_OS_CONSOLE_HPP
