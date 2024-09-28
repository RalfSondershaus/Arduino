/**
 * @file Signal/Com/SerDrv.h
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

#ifndef SERDRV_H_
#define SERDRV_H_

#include <Std_Types.h>
#include <Util/bitset.h>

namespace com
{
  
  // For serial communication, Arduino uses a 16 byte buffer (for systems with RAM smaller 
  // than 1 KB) or a 64 byte buffer (else).
  // 
  // 
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class SerDrv
  {
  public:
    using size_type = size_t;
    typedef const uint8* const_data_pointer;

    /// Size of payload buffer
    static constexpr size_t kMaxLenPayload = 256U;

    static constexpr uint8 kModeAscii = 0;
    static constexpr uint8 kModeSerTP = 1;

    uint8 ucMode;

  protected:

  public:
    /// Construct.
    SerDrv() = default;

    /// Initialization
    void init();
    /// Receive data from low level drivers and process them
    void cycle();
  };

} // namespace com

#endif // SERDRV_H_