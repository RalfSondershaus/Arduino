/**
 * @file Prj/App/Signal/Com/ComR_Prj.cpp
 *
 * @brief COM module wiring for Signal application.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Com/ComR.h>

namespace com
{
    ComR::ComR()
    {
    }

    void ComR::init()
    {
        mySerAsciiTP.init();
        mySerAsciiTP.setDriver(mySerDrv);
        myAsciiCom.listen_to(mySerAsciiTP);
        myAsciiCom.set_command_handler(mySignalAsciiCommandHandler);
    }

    void ComR::cycle()
    {
        mySerAsciiTP.cycle();
        myAsciiCom.cycle();
    }
}
