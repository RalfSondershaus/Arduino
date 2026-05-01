/**
 * @file Gen/Com/AsciiCommandRegistry.cpp
 *
 * @brief Generic ASCII command registry lookup for AsciiCom.
 *
 * @copyright Copyright 2026 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Compiler.h>
#include <Com/AsciiCommandRegistry.h>
#include <Util/Array.h>
#include <Util/String_view.h>

namespace com
{
    // Wrappers implemented in AsciiCom.cpp and used by registry entries.
    ascii_com_registry::ret_type ascii_com_cmd_set_cv(ascii_com_registry::stream_type &st, ascii_com_registry::string_type &response);
    ascii_com_registry::ret_type ascii_com_cmd_get_cv(ascii_com_registry::stream_type &st, ascii_com_registry::string_type &response);
    ascii_com_registry::ret_type ascii_com_cmd_monitor_list(ascii_com_registry::stream_type &st, ascii_com_registry::string_type &response);
    ascii_com_registry::ret_type ascii_com_cmd_monitor_start(ascii_com_registry::stream_type &st, ascii_com_registry::string_type &response);
    ascii_com_registry::ret_type ascii_com_cmd_monitor_stop(ascii_com_registry::stream_type &st, ascii_com_registry::string_type &response);

    namespace
    {
        ascii_com_registry::project_find_command_type gProjectFinder = nullptr;
    }

    namespace ascii_com_registry
    {
        struct command_type
        {
            const char *cmd;
            command_handler_type handler;
        };

        const char cmd_set_cv[] ROM_CONST_VAR = "SET_CV";
        const char cmd_get_cv[] ROM_CONST_VAR = "GET_CV";
        const char cmd_mon_list[] ROM_CONST_VAR = "MON_LIST";
        const char cmd_mon_start[] ROM_CONST_VAR = "MON_START";
        const char cmd_mon_stop[] ROM_CONST_VAR = "MON_STOP";

        using command_array_type = util::array<command_type, 5>;

        const command_array_type commands ROM_CONST_VAR =
            {{{cmd_set_cv, ascii_com_cmd_set_cv},
              {cmd_get_cv, ascii_com_cmd_get_cv},
              {cmd_mon_list, ascii_com_cmd_monitor_list},
              {cmd_mon_start, ascii_com_cmd_monitor_start},
              {cmd_mon_stop, ascii_com_cmd_monitor_stop}}};

        boolean find_command(const char *cmd, command_handler_type &handler)
        {
            size_t idx;
            char cmd_rom[kMaxLenToken];
            util::string_view sv(cmd);

            for (idx = 0U; idx < commands.size(); idx++)
            {
                command_type item;
                ROM_READ_STRUCT(&item, &commands[idx], sizeof(command_type));
                ROM_READ_STRING(cmd_rom, item.cmd);
                if (sv.compare(cmd_rom) == 0)
                {
                    handler = item.handler;
                    return true;
                }
            }

            handler = nullptr;
            return false;
        }

        void format_response(const char *response_text, const string_type &sub_response, string_type &response)
        {
            response = response_text;
            if (sub_response.size() > 0)
            {
                response.append(" ");
                response.append(sub_response);
            }
        }

        void set_project_command_finder(project_find_command_type finder)
        {
            gProjectFinder = finder;
        }

        boolean find_project_command(const char *cmd, command_handler_type &handler)
        {
            if (gProjectFinder != nullptr)
            {
                return gProjectFinder(cmd, handler);
            }
            handler = nullptr;
            return false;
        }
    } // namespace ascii_com_registry
} // namespace com
