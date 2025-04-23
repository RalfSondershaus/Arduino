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
  static constexpr uint8 kBlinkLedPin = 13U;
  static constexpr util::MilliTimer::time_type kBlinkLedPeriodValid_ms = 1000U;
  static constexpr util::MilliTimer::time_type kBlinkLedPeriodInvalid_ms = 500U;

  // --------------------------------------------------------------------------
  /// @brief Toggle the alive LED
  // --------------------------------------------------------------------------
  static void toggleLedPin()
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

    hal::digitalWrite(kBlinkLedPin, lastWrite);
  }

  // --------------------------------------------------------------------------
  /// @brief If current timer is elapsed, toggle LED and reload timer with period_ms
  /// @param period_ms New blink period [ms]
  // --------------------------------------------------------------------------
  static bool toggleLedPin(util::MilliTimer::time_type period_ms)
  {
    static util::MilliTimer LedTimer;
    // alive LED
    if (LedTimer.timeout())
    {
      toggleLedPin();
      LedTimer.start(period_ms);
      return true;
    }
    return false;
  }

  // --------------------------------------------------------------------------
  /// @brief Returns true if the coding data are valid.
  // --------------------------------------------------------------------------
  static inline bool cal_isValid(const cal::base_cv_cal_type* pCal)
  {
    return pCal != nullptr;
  }

  // --------------------------------------------------------------------------
  /// @brief Returns true if the addressing method is Decoder Address Method.
  // --------------------------------------------------------------------------
  static inline bool cal_isDecoderAddressingMethod(const cal::base_cv_cal_type* pCal) noexcept
  {
    return (pCal->Configuration & cal::configuration::bitmask::kAddressingMethod) == cal::configuration::kAddressingMethod_Decoder;
  }
  
  // --------------------------------------------------------------------------
  /// @brief Returns the decoder address (depending on addressing method).
  // --------------------------------------------------------------------------
  static inline uint16 cal_calcAddress(const cal::base_cv_cal_type* pCal) noexcept
  {
    uint16 address;
    
    if (cal_isDecoderAddressingMethod(pCal))
    {
      address = static_cast<uint16_t>((pCal->AddressLSB & 0xb00111111U) + (static_cast<uint16>(pCal->AddressMSB & 0xb00000111U) << 6U));
    }
    else
    {
      address = static_cast<uint16>(pCal->AddressLSB + (static_cast<uint16>(pCal->AddressMSB) << 8U));
    }

    return address;
  }

  // --------------------------------------------------------------------------
  /// @brief Handle a valid packet. Precondition: coding data must be valid.
  /// @param pkt The received packet
  // --------------------------------------------------------------------------
  void DccDecoder::packet_received(const PacketType& pkt)
  {
    const size_t pos = pkt.getAddress() - get_address();
    if (rte::ifc_dcc_commands::boundaryCheck(pos))
    {
      // TBD could be improved by defining sub classes for PacketType
      switch (pkt.getType())
      {
        case PacketType::BasicAccessory:
          rte::ifc_dcc_commands::writeElement(pos, pkt.baGetDSwitch());
          break;
        case PacketType::ExtendedAccessory:
          rte::ifc_dcc_commands::writeElement(pos, pkt.eaGetAspect());
          break;
        default:
          break;
      }
    }
    toggleLedPin();
  }

  // --------------------------------------------------------------------------
  /// @brief Init after power on
  // --------------------------------------------------------------------------
  void DccDecoder::init()
  {
    const cal::base_cv_cal_type* pCal = rte::ifc_cal_base_cv::call();

    hal::pinMode(kBlinkLedPin, OUTPUT);

    decoder.init(kIntPin);

    if (cal_isValid(pCal))
    {
      address = cal_calcAddress(pCal);
      if (cal_isDecoderAddressingMethod(pCal))
      {
        decoder.disableOutputAddressMethod();
      }
      else
      {
        decoder.enableOutputAddressMethod();
      }
    }   

    //decoder.setFilter(passFilter);
  }

  // --------------------------------------------------------------------------
  /// Function is called cyclicly. Fetches new packets and handles them.
  // --------------------------------------------------------------------------
  void DccDecoder::cycle()
  {
    const cal::base_cv_cal_type* pCal = rte::ifc_cal_base_cv::call();

    if (cal_isValid(pCal))
    {
      passFilter.setLo(get_address());
      passFilter.setHi(get_address() + cfg::kNrAddresses);

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
        const PacketType& pkt = decoder.front();
        // In case the address changed at run time (e.g. reconfiguration)
        // we check here again
        if (passFilter.filter(pkt))
        {
          packet_received(pkt);
        }
        decoder.pop();
      }
      if (toggleLedPin(kBlinkLedPeriodValid_ms))
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
      toggleLedPin(kBlinkLedPeriodInvalid_ms);
    }
  }

} // namespace signal
