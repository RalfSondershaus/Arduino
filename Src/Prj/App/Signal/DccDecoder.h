/**
 * @file Signal/DccDecoder.h
 *
 * @brief Wrapper for dcc::decoder
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PRJ_DCC_DECODER_H_
#define PRJ_DCC_DECODER_H_

#include <Dcc/Decoder.h>
#include <Dcc/Filter.h>
#include <Rte/Rte_Types.h>
#include <Util/Array.h>
#include <Util/Timer.h>
#include <Cal/CalM_Types.h>

namespace signal
{
  class DccDecoder
  {
  protected:
    using packet_type = dcc::decoder::packet_type;
    using filter_type = dcc::pass_accessory_address_filter<packet_type>;

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
     * @brief Handles the reception of basic DCC accessory packets.
     * 
     * This function processes basic DCC packets. It calculates the command from the packet's
     * address and the output direction, and forwards the result to the RTE. The position on RTE 
     * is calculated from the DCC address of the packet minus the first output address.
     * 
     * @param pkt Reference to the received DCC packet
     */
    void basic_packet_received(packet_type& pkt);
    
    /**
     * @brief Handles the reception of extended DCC accessory packets.
     * 
     * This function processes extended DCC packets. It forwards the aspect value to the RTE.
     * The position on RTE is calculated from the DCC address of the packet minus the first output
     * address.
     * 
     * @param pkt Reference to the received DCC packet
     */
    void extended_packet_received(packet_type& pkt);

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

    uint8 get_cv29() const noexcept { return pass_accessory_filter.get_cv29(); }

    /// Returns the first DCC output address of the decoder.
    uint16 get_first_output_address() const noexcept { return first_output_address; }

    void init();
    void cycle();
  };
}

#endif // PRJ_DCC_DECODER_H_