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

namespace com
{
    class SignalAsciiCommandHandler : public IfcAsciiCommandHandler
    {
    public:
        bool process_command(const char *cmd, stringstream_type &st, string_type &response) override;
    };
}

#endif // SIGNALASCIICOMMANDHANDLER_H_
