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
  SerTP::size_type SerTP::decodeMessage(const_data_pointer data, size_type count)
  {
    // number of consumed bytes
    size_type cnt = 0;

    // (1) consume header data
    if (header.ucIdx == 0U)
    {
      header.ucProtocol = util::bits::extract<uint8>(data[cnt], 4, 4);
      header.ucCode = util::bits::extract<uint8>(data[cnt], 0, 4);
      cnt++;
      header.ucIdx++;
    }
    if (header.ucIdx == 1U)
    {
      header.ucLength = data[cnt];
      cnt++;
      header.ucIdx++;
    }

    if (header.ucCode == kCodeFirstFrame)
    {
      if (header.ucIdx == 2U)
      {
        header.unTotalLength = util::make_number<uint16, uint8, uint8>(data[cnt], 0U);
        cnt++;
        header.ucIdx++;
      }
      if (header.ucIdx == 3U)
      {
        header.unTotalLength += util::make_number<uint16, uint8, uint8>(0U, data[cnt]);
        cnt++;
        header.ucIdx++;
      }
    }
    // (2) If header data are complete, forward PDU to upper layer
    if ((header.ucIdx > 3U) || ((header.ucIdx > 1U) && (header.ucCode != kCodeFirstFrame)))
    {
      forwardMessage();
    }

    return cnt;
  }

  void SerTP::forwardMessage()
  {
    // (3a) If message length is greater than buffer size, the message is invalid.
    //      Discard message (discard remaining bytes).
    if (header.ucLength < header.ucIdx)
    {
      // (3a) header length is less than header size
    }
    else
    {
      // remaining bytes are payload bytes
      const size_type unPayloadLen = static_cast<size_type>(header.ucLength - header.ucIdx);

      if (unPayloadLen > payload.aBuffer.max_size())
      {
        cnt += util::min(count - cnt, len - position);
      }
      else
      {
        while ((cnt < count) && (position < len))
        {
          aPayload[position++] = data[cnt++];
        }

        // (3b) If message is complete, execute message and prepare next message
        if (position >= len)
        {
          forwardMessage();
          position = 0U;
        }
      }

  }

  // -----------------------------------------------------------------------------------
  /// Initialization
  // -----------------------------------------------------------------------------------
  void SerTP::init()
  {
    decodeState = INIT;
    header.ucIdx = 0U;
    unIdxPayload = 0U;
  }

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void SerTP::dataRx(const_data_pointer data, size_type count)
  {
    size_type cnt = 0U;

    while (cnt < count)
    {
      cnt += decodeMessage(&data[cnt], count - cnt);
    }
  }

} // namespace com
