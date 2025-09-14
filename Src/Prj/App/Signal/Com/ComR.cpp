/**
 * @file Com/ComR.cpp
 *
 * @brief 
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * See <https://www.gnu.org/licenses/>.
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
