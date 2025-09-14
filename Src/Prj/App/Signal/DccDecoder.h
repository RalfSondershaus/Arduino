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
#include <Dcc/Filter.h>
#include <Rte/Rte_Type.h>
#include <Util/Timer.h>
#include <Cal/CalM_Type.h>

namespace signal
{
  class DccDecoder
  {
  protected:
    using PacketType = dcc::Decoder::PacketType;
    using PassAddrFltr = dcc::PassAddressFilter<PacketType>;
    
    /// The decoder
    dcc::Decoder decoder;
    PassAddrFltr passFilter;
    /// The address of the decoder. Calculated from coding data.
    /// If the decoder supports a series of addresses, this variable stores
    /// the first address.
    uint16 address;

    void packet_received(const PacketType& pkt);

  public:
    /// The interrupt pin
    static constexpr uint8 kIntPin = 2U;
    static constexpr uint8 kBlinkLedPin = 13U;
    static constexpr util::MilliTimer::time_type kBlinkLedPeriodValid_ms = 1000U;
    static constexpr util::MilliTimer::time_type kBlinkLedPeriodInvalid_ms = 500U;

    DccDecoder() = default;

    /// Returns the DCC address of the decoder.
    uint16 get_address() const noexcept { return address; }

    void init();
    void cycle();
  };
}

#endif // PRJ_DCC_DECODER_H_