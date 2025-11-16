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
#include <Rte/Rte.h>
#include <Hal/Gpio.h>
#include <Hal/Serial.h>
#include <Cal/CalM_Types.h>
#include <Util/bitset.h>
#include <Rte/Rte_Cfg_Cod.h>

namespace signal_cal
{
    /**
     * @brief Returns the DCC output address from the decoder configuration.
     * 
     * @return uint16 DCC output address
     */
    static inline uint16 calc_output_address()
    {
        return (util::bits::apply_mask_as<uint16,uint16>(rte::get_cv(cal::cv::kDecoderAddressLSB), cal::base_cv::bitmask::kCV1_address_LSB)) |
               (util::bits::apply_mask_as<uint16,uint16>(rte::get_cv(cal::cv::kDecoderAddressMSB), cal::base_cv::bitmask::kCV9_address_MSB) << 8);
    }
    /**
     * @brief Get CV29 (configuration)
     * 
     * @return uint8 CV29 (configuration)
     */
    static inline uint8 get_cv29()
    {
        return rte::get_cv(cal::cv::kConfiguration);
    }
}

namespace signal
{

    // --------------------------------------------------------------------------
    /// @brief Toggle the alive LED
    // --------------------------------------------------------------------------
    static void toggle_led_pin()
    {
        static uint8 lastWrite = LOW;

        if (lastWrite == LOW)
        {
            lastWrite = HIGH;
        }
        else
        {
            lastWrite = LOW;
        }

        hal::digitalWrite(DccDecoder::kBlinkLedPin, lastWrite);
    }

    // --------------------------------------------------------------------------
    /// @brief If current timer is elapsed, toggle LED and reload timer with period_ms
    /// @param period_ms New blink period [ms]
    // --------------------------------------------------------------------------
    static bool toggle_led_pin(util::MilliTimer::time_type period_ms)
    {
        static util::MilliTimer LedTimer;
        // alive LED
        if (LedTimer.timeout())
        {
            toggle_led_pin();
            LedTimer.start(period_ms);
            return true;
        }
        return false;
    }

    /**
     * @brief Handles the reception of basic DCC packets.
     * 
     * Each signal uses `@ref cfg::kNrDccAddressesPerSignal` DCC addresses, and the signals
     * use consecutive DCC addresses.
     * 
     * For example, with `@ref cfg::kNrDccAddressesPerSignal` = 4:
     * - Signal 0 uses DCC addresses `first_output_address` - `first_output_address + 3`
     * - Signal 1 uses DCC addresses `first_output_address + 4` - `first_output_address + 7`
     * - ...
     * 
     * The command is calculated from the output direction:
     * - first_output_address     R = command 0
     * - first_output_address     G = command 1
     * - first_output_address + 1 R = command 2
     * - first_output_address + 1 G = command 3
     * - first_output_address + 2 R = command 4
     * - first_output_address + 2 G = command 5
     * - ...
     * 
     * @param pkt Reference to the received DCC packet
     */
    void DccDecoder::basic_packet_received(packet_type& pkt)
    {
        // The pass filter owns a copy of CV29 from the decoder configuration data.
        const uint16 pkt_address = pkt.get_address(get_cv29());
        if (pkt_address >= get_first_output_address())
        {
            // pair index
            // 0 - 3: Signal 0
            // 4 - 7: Signal 1
            // ...
            const size_t idx = (pkt_address - get_first_output_address()) % cfg::kNrDccAddressesPerSignal;
            // and position on RTE
            const size_t pos = (pkt_address - get_first_output_address()) / cfg::kNrDccAddressesPerSignal;
            // command: 0 = 1R, 1 = 1G, 2 = 2R, 3 = 2G, ...
            const uint8 cmd = static_cast<uint8>(2U*idx + pkt.ba_get_output_direction());
            hal::serial::print("Basic Accessory Packet received: addr=");
            hal::serial::print(pkt_address);
            hal::serial::print(" pos=");
            hal::serial::print(static_cast<int>(pos));
            hal::serial::print(" cmd=");
            hal::serial::print(static_cast<int>(cmd));
            if (rte::ifc_dcc_commands::boundaryCheck(pos))
            {
                rte::ifc_dcc_commands::writeElement(pos, cmd);
                hal::serial::print(" update RTE");
            }
            hal::serial::println();
        }
    }

    /**
     * @brief Handles the reception of extended DCC accessory packets.
     * 
     * This function processes extended DCC packets. It forwards the aspect value to the RTE.
     * The position on RTE is calculated from the DCC address of the packet minus the first output
     * address.
     * 
     * @param pkt Reference to the received DCC packet
     */
    void DccDecoder::extended_packet_received(packet_type& pkt)
    {
        const size_t pos = pkt.get_address(get_cv29()) - get_first_output_address();
        hal::serial::print("Extended Accessory Packet received: addr=");
        hal::serial::println(pkt.get_address(get_cv29()));
        hal::serial::print(" pos=");
        hal::serial::println(static_cast<int>(pos));
        if (rte::ifc_dcc_commands::boundaryCheck(pos))
        {
            rte::ifc_dcc_commands::writeElement(pos, pkt.ea_get_aspect());
        }
    }

    /**
     * @brief Processes received DCC packets for accessory decoders
     * 
     * This function handles both basic and extended accessory DCC packets.
     * It forwards the output direction or the aspect value to the RTE.
     * The position on RTE is calculated from the DCC address of the packet minus the first output 
     * address.
     * 
     * @param pkt Reference to the received DCC packet
     * 
     * @details For basic accessory packets, forwards the output direction.
     *          For extended accessory packets, forwards the aspect value.
     *          Toggles an LED pin after processing to indicate activity.
     *          Ignores packets if the calculated position is out of bounds.
     * 
     * @note The address calculation depends on CV29 configuration stored in pass_filter
     */
    void DccDecoder::packet_received(packet_type &pkt)
    {
        // TBD could be improved by defining sub classes for packet_type
        switch (pkt.get_type())
        {
        case packet_type::packet_type::BasicAccessory:
            basic_packet_received(pkt);
            break;
        case packet_type::packet_type::ExtendedAccessory:
            extended_packet_received(pkt);
            break;
        default:
            break;
        }

        toggle_led_pin();
    }

    // --------------------------------------------------------------------------
    /// @brief Init after power on
    // --------------------------------------------------------------------------
    void DccDecoder::init()
    {
        hal::pinMode(kBlinkLedPin, OUTPUT);

        decoder.init(kIntPin);

        first_output_address = signal_cal::calc_output_address();
        pass_accessory_filter.set_lo(first_output_address);
        pass_accessory_filter.set_hi(first_output_address + cfg::kNrAddresses);
        pass_accessory_filter.set_cv29(signal_cal::get_cv29());
        decoder.set_filter(pass_accessory_filter);
    }

    // --------------------------------------------------------------------------
    /// Function is called cyclicly. Fetches new packets and handles them.
    // --------------------------------------------------------------------------
    void DccDecoder::cycle()
    {
        // recalculate address because coding data might have changed.
        // TBD: Can be optimized if CalM informs about coding data changes or 
        // if DCC address is publicly available.
        first_output_address = signal_cal::calc_output_address();
        if ((pass_accessory_filter.get_lo() != first_output_address) ||
            (pass_accessory_filter.get_hi() != (first_output_address + cfg::kNrAddresses)) ||
            (pass_accessory_filter.get_cv29() != signal_cal::get_cv29()))
        {
            hal::serial::println("Update filter");
            pass_accessory_filter.set_lo(first_output_address);
            pass_accessory_filter.set_hi(first_output_address + cfg::kNrAddresses);
            pass_accessory_filter.set_cv29(signal_cal::get_cv29());
            decoder.set_filter(pass_accessory_filter);
        }

        if (decoder.isrOverflow())
        {
            hal::serial::println("ISR OVERFLOW");
        }

        if (decoder.fifoOverflow())
        {
            hal::serial::println("FIFO OVERFLOW");
        }

        decoder.fetch();
        while (!decoder.empty())
        {
            packet_type &pkt = decoder.front();
            hal::serial::print("Packet type=");
            hal::serial::print(static_cast<uint8>(pkt.get_type()));
            hal::serial::print(" Packet address=");
            hal::serial::println(pkt.get_address(get_cv29()));
            if (pass_accessory_filter.filter(pkt))
            {
                packet_received(pkt);
            }

            decoder.pop();
        }
        if (toggle_led_pin(kBlinkLedPeriodValid_ms))
        {
#if 0
            hal::serial::print("[");
            hal::serial::print(hal::micros());
            hal::serial::print("]");
            hal::serial::print(" isr=");
            hal::serial::print(decoder.getNrInterrupts());
            hal::serial::print(" packets=");
            hal::serial::println(decoder.getPacketCount());
#endif
        }
    }

} // namespace signal
