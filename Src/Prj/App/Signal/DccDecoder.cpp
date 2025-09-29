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
#include <Cal/CalM_config.h>

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

    // --------------------------------------------------------------------------
    /// @brief Returns true if the coding data are valid.
    // --------------------------------------------------------------------------
    static inline bool is_valid(const cal::base_cv_cal_type *cal_ptr)
    {
        return cal_ptr != nullptr;
    }

    // --------------------------------------------------------------------------
    /// @brief Returns the DCC output address
    // --------------------------------------------------------------------------
    static inline uint16 calc_output_address(const cal::base_cv_cal_type *cal_ptr)
    {
        return (static_cast<uint16>(cal_ptr->CV1_address_LSB & cal::kMaskCV1_address_LSB)) |
               (static_cast<uint16>(cal_ptr->CV9_address_MSB & cal::kMaskCV9_address_MSB) << 6);
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
        // The pass filter owns a copy of CV29 from the decoder configuration data.
        // It is used for address calculation.
        const size_t pos = pkt.get_address(pass_accessory_filter.get_cv29()) - get_first_output_address();
        hal::serial::print("DCC packet received: addr=");
        hal::serial::println(pkt.get_address(pass_accessory_filter.get_cv29()));
        hal::serial::print(" pos=");
        hal::serial::println(static_cast<int>(pos));
        if (rte::ifc_dcc_commands::boundaryCheck(pos))
        {
            // TBD could be improved by defining sub classes for packet_type
            switch (pkt.get_type())
            {
            case packet_type::packet_type::BasicAccessory:
                // We store the command as 11 bit output address
                rte::ifc_dcc_commands::writeElement(pos, pkt.ba_get_output_direction());
                break;
            case packet_type::packet_type::ExtendedAccessory:
                rte::ifc_dcc_commands::writeElement(pos, pkt.ea_get_aspect());
                break;
            default:
                break;
            }
        }
        toggle_led_pin();
    }

    // --------------------------------------------------------------------------
    /// @brief Init after power on
    // --------------------------------------------------------------------------
    void DccDecoder::init()
    {
        const cal::base_cv_cal_type *cal_ptr = rte::ifc_cal_base_cv::call();

        hal::pinMode(kBlinkLedPin, OUTPUT);

        decoder.init(kIntPin);

        if (is_valid(cal_ptr))
        {
            first_output_address = calc_output_address(cal_ptr);
            pass_accessory_filter.set_lo(first_output_address);
            pass_accessory_filter.set_hi(first_output_address + cfg::kNrAddresses);
            pass_accessory_filter.set_cv29(cal_ptr->CV29_configuration);
            decoder.set_filter(pass_accessory_filter);
        }
    }

    // --------------------------------------------------------------------------
    /// Function is called cyclicly. Fetches new packets and handles them.
    // --------------------------------------------------------------------------
    void DccDecoder::cycle()
    {
        const cal::base_cv_cal_type *cal_ptr = rte::ifc_cal_base_cv::call();

        if (is_valid(cal_ptr))
        {
            // recalculate address because coding data might have changed.
            // TBD: Can be optimized if CalM informs about coding data changes or 
            // if DCC address is publicly available.
            first_output_address = calc_output_address(cal_ptr);
            if ((pass_accessory_filter.get_lo() != first_output_address) ||
                (pass_accessory_filter.get_hi() != (first_output_address + cfg::kNrAddresses)) ||
                (pass_accessory_filter.get_cv29() != cal_ptr->CV29_configuration))
            {
                hal::serial::println("Update filter");
                pass_accessory_filter.set_lo(first_output_address);
                pass_accessory_filter.set_hi(first_output_address + cfg::kNrAddresses);
                pass_accessory_filter.set_cv29(cal_ptr->CV29_configuration);
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
                hal::serial::println(pkt.get_address(pass_accessory_filter.get_cv29()));
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
        else
        {
            toggle_led_pin(kBlinkLedPeriodInvalid_ms);
        }
    }

} // namespace signal
