/**
 * @file Prj/App/Signal/Com/ComR_Prj.h
 *
 * @brief COM module wiring for Signal application.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMR_PRJ_H_
#define COMR_PRJ_H_

#include <Com/ComR.h>
#include <Com/SignalAsciiCommandHandler.h>

namespace com
{
    class ComR_Prj : public ComR
    {
        using parent_class = ComR;
        
    protected:
        AsciiCom myAsciiCom;
        SerAsciiTP mySerAsciiTP;
        SerComDrv mySerDrv;
        SignalAsciiCommandHandler mySignalAsciiCommandHandler;

    public:
        ComR_Prj();
        void init();
        void cycle();
    };
}

#endif // COMR_PRJ_H_
