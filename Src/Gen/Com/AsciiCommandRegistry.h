/**
 * @file Gen/Com/AsciiCommandRegistry.h
 *
 * @brief Generic ASCII command registry lookup for AsciiCom.
 *
 * @copyright Copyright 2026 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ASCIICOMMANDREGISTRY_H_
#define ASCIICOMMANDREGISTRY_H_

#include <Std_Types.h>
#include <Com/AsciiCom.h>
#include <Util/Sstream.h>

namespace com
{
    namespace ascii_com_registry
    {
        using string_type = AsciiCom::string_type;
        using stream_type = util::basic_istringstream<SerAsciiTP::kMaxLenTelegram, AsciiCom::char_type>;

        enum ret_type
        {
            eOK = 0,
            eINV_CMD,
            eINV_CV_ID,
            eCV_VALUE_OUT_OF_RANGE,
            eINV_MONITOR_START_PARAM,
            eINV_MONITOR_START_IFC_NAME,
            eERR_UNKNOWN
        };

        using command_handler_type = ret_type (*)(stream_type &st, string_type &response);
        using project_find_command_type = boolean (*)(const char *cmd, command_handler_type &handler);

        static constexpr size_t kMaxLenToken = 20U;

        boolean find_command(const char *cmd, command_handler_type &handler);

        /**
         * @brief Register a project-level command registry lookup function.
         *
         * This callback is invoked during dispatch if generic commands are not found.
         * Using a callback avoids the need for virtual dispatch and maintains the registry pattern.
         *
         * @param finder Function that looks up project-specific commands by name.
         *              Returns true if command found (handler pointer populated), false otherwise.
         */
        void set_project_command_finder(project_find_command_type finder);

        /**
         * @brief Lookup a project command (internal use by AsciiCom).
         *
         * @param cmd Command token to lookup.
         * @param handler [out] Handler function pointer if found.
         * @return true Command found, handler is valid.
         * @return false Command not found.
         */
        boolean find_project_command(const char *cmd, command_handler_type &handler);

        /**
         * @brief Format a response by combining status text and optional sub-response.
         *
         * Assembles a full response from a status message and an optional sub-response payload.
         * This helper eliminates duplication between generic and project-specific handlers.
         *
         * @param response_text The status message (typically read from ROM via ROM_READ_PTR).
         * @param sub_response Optional additional response data (may be empty).
         * @param response [out] The formatted response string.
         */
        void format_response(const char *response_text, const string_type &sub_response, string_type &response);
    } // namespace ascii_com_registry
} // namespace com

#endif // ASCIICOMMANDREGISTRY_H_
