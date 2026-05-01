/**
 * @file Prj/App/Signal/Com/SignalAsciiCommandHandler.h
 *
 * @brief Signal project specific ASCII command handler.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SIGNALASCIICOMMANDHANDLER_H_
#define SIGNALASCIICOMMANDHANDLER_H_

#include <Com/IfcAsciiCommandHandler.h>
#include <Com/AsciiCommandRegistry.h>

namespace com
{
    namespace signal_com_registry
    {
        /**
         * @brief Lookup a project command by name and return its handler.
         *
         * This function is registered with AsciiCom as a callback to enable
         * direct registry-based dispatch without virtual calls.
         *
         * @param cmd Command token to lookup.
         * @param handler [out] Handler function pointer if found.
         * @return true Command found.
         * @return false Command not found.
         */
        boolean find_command(const char *cmd, ascii_com_registry::command_handler_type &handler);
    }

    class SignalAsciiCommandHandler : public IfcAsciiCommandHandler
    {
    public:
        bool process_command(const char *cmd, stringstream_type &st, string_type &response) override;
    };
}

#endif // SIGNALASCIICOMMANDHANDLER_H_
