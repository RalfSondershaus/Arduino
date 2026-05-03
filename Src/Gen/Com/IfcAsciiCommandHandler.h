/**
 * @file Gen/Com/IfcAsciiCommandHandler.h
 *
 * @brief Interface for project-specific ASCII command extensions.
 *
 * @copyright Copyright 2026 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IFCASCIICOMMANDHANDLER_H_
#define IFCASCIICOMMANDHANDLER_H_

#include <Com/SerAsciiTP.h>
#include <Util/Sstream.h>

namespace com
{
    class IfcAsciiCommandHandler
    {
    public:
        using char_type = SerAsciiTP::telegram_base_type;
        using string_type = SerAsciiTP::string_type;
        using stringstream_type = util::basic_istringstream<SerAsciiTP::kMaxLenTelegram, char_type>;

        /**
         * @brief Destroy the command handler.
         * 
         * The destructor should be virtual to allow proper cleanup of derived classes.
         * BUT the AVR GCC compiler throws 'undefined reference to `operator delete(void*, unsigned 
         * int)' when using virtual destructors. Since dynamic memory allocation is not used and 
         * objects are destructed at shut down only (-> never), we can safely avoid virtual 
         * destructors in this case.
         */
        ~IfcAsciiCommandHandler() = default;

        /**
         * @brief Process a project-specific command.
         *
         * @param cmd Command token, e.g. "SET_SIGNAL".
         * @param st Stream with command parameters (token already consumed).
         * @param response Full response telegram to transmit.
         * @return true Command token recognized (response is valid).
         * @return false Command token not recognized.
         */
        virtual bool process_command(const char *cmd, stringstream_type &st, string_type &response) = 0;
    };
}

#endif // IFCASCIICOMMANDHANDLER_H_
