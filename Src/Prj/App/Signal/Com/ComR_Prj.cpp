/**
 * @file Prj/App/Signal/Com/ComR_Prj.cpp
 *
 * @brief COM module wiring for Signal application.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Com/ComR_Prj.h>

namespace com
{
    ComR_Prj::ComR_Prj()
    {
    }

    void ComR_Prj::init()
    {
        parent_class::init(); // Call base class init if needed (not strictly necessary here since we override it completely)
        // Register the project-specific command handler with AsciiCom
        myAsciiCom.set_command_handler(mySignalAsciiCommandHandler);
    }

    void ComR_Prj::cycle()
    {
        parent_class::cycle(); // Call base class cycle if needed (not strictly necessary here since we override it completely)
    }
}
