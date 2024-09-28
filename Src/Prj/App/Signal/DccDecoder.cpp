/**
 * @file Signal/DccDecoder.cpp
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

#include "DccDecoder.h"

void DccDecoder::init()
{
  decoder.init(kIntPin);
}

/// Function is called cyclicly
void DccDecoder::cycle()
{
  const cal::dcc_cal_type* pCal = rte::ifc_cal_dcc::call();
  if (cal_valid(pCal))
  {
    decoder.fetch();
  }
}
