/**
 * @file Com/ComR.cpp
 *
 * @brief 
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Com/ComR.h>
#include <Util/Algorithm.h>
#include <Util/Array.h>

namespace com
{
  // -----------------------------------------------------------------------------------
  /// Construct
  // -----------------------------------------------------------------------------------
  ComR::ComR()
  {}

  // -----------------------------------------------------------------------------------
  /// Initialization
  // -----------------------------------------------------------------------------------
  void ComR::init()
  {
    mySerAsciiTP.init();
    mySerAsciiTP.setDriver(mySerDrv);
    myAsciiCom.listen_to(mySerAsciiTP);
  }

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void ComR::cycle()
  {
    mySerAsciiTP.cycle();
    myAsciiCom.cycle();
  }

} // namespace com
