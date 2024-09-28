/**
 * @file Signal/DccDecoder.h
 *
 * @brief Wrapper for dcc::decoder
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

#ifndef PRJ_DCC_DECODER_H_
#define PRJ_DCC_DECODER_H_

#include <Dcc/Decoder.h>
#include <Rte/Rte_Type.h>
#include <Cal/CalM_Type.h>

class DccDecoder
{
protected:
  /// The decoder
  dcc::Decoder decoder;

  /// The interrupt pin
  static constexpr uint8 kIntPin = 2U;

  /// Returns true if pCal is valid. Returns false otherwise.
  bool cal_valid(const cal::dcc_cal_type* pCal) { return pCal != nullptr; }
public:
  DccDecoder() = default;

  void init();
  void cycle();
};
#endif // PRJ_DCC_DECODER_H_