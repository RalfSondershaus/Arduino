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
    using packet_type = dcc::Decoder::PacketType;
    using filter_type = dcc::PassAccessoryAddressFilter<packet_type>;
    
    /**
     * @brief The DCC decoder instance.
     */
    dcc::Decoder decoder;
    /**
     * @brief The address filter for the decoder. Only packets that pass this filter are stored 
     * in the FIFO buffer.
     * @note The filter owns a copy of CV29 from the decoder configuration data so DccDecoder does
     * not need to store its own copy of CV29.
     */
    filter_type pass_accessory_filter;
    /**
     * @brief The first output address of the decoder as calculated from CV1/CV9.
     * 
     * If the decoder supports a series of addresses, this variable stores the first address.
     */
    uint16 first_output_address;

    /**
     * @brief Processes received DCC packets for accessory decoders.
     * 
     * This function handles both basic and extended accessory DCC packets.
     * It forwards the output direction or the aspect value to the RTE.
     * The position on RTE is calculated from the DCC address of the packet minus the first output 
     * address.
     * 
     * @param pkt Reference to the received DCC packet
     * @note Can modify the packet (e.g. for addressing calculation), so pkt is not const.
     */
    void packet_received(packet_type& pkt);

  public:
    /// The interrupt pin
    static constexpr uint8 kIntPin = 2U;
    static constexpr uint8 kBlinkLedPin = 13U;
    static constexpr util::MilliTimer::time_type kBlinkLedPeriodValid_ms = 1000U;
    static constexpr util::MilliTimer::time_type kBlinkLedPeriodInvalid_ms = 500U;

    DccDecoder() = default;

    /// Returns the first DCC output address of the decoder.
    uint16 get_first_output_address() const noexcept { return first_output_address; }

    void init();
    void cycle();
  };
}

#endif // PRJ_DCC_DECODER_H_