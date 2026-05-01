/**
 * @file Prj/App/Signal/Com/ComR.h
 *
 * @brief COM module wiring for Signal application.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMR_H_
#define COMR_H_

#include <Com/AsciiCom.h>
#include <Com/SerAsciiTP.h>
#include <Com/SerComDrv.h>
#include <Com/SignalAsciiCommandHandler.h>

namespace com
{
    class ComR
    {
    protected:
        AsciiCom myAsciiCom;
        SerAsciiTP mySerAsciiTP;
        SerComDrv mySerDrv;
        SignalAsciiCommandHandler mySignalAsciiCommandHandler;

    public:
        ComR();
        void init();
        void cycle();
    };
}

#endif // COMR_H_
