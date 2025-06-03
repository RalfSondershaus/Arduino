/**
 * @file Ut_Signal/CalM_config.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Define calibration datasets
 *
 * @copyright Copyright 2022 Ralf Sondershaus
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

#ifndef CALM_CONFIG_H_
#define CALM_CONFIG_H_

#include <Cfg_Prj.h> // number of signals
#include <Cal/CalM_Type.h>

using classifier_type = cal::input_classifier_type::classifier_type;

namespace cal
{
  namespace cv
  {
    /// @brief CV indices
    enum
    {
      eDecoderAddressLSB          = 1,
      eAuxiliaryActivattion       = 2,
      eTimeOnBase                 = 3,
      eManufacturerVersionID      = 7,
      eManufacturerID             = 8,
      eDecoderAddressMSB          = 9,
      eConfiguration              = 29,
      eManufacturerCVStructureID  = 33,
      eSignalBase                 = 512,  ///< CV index for the first signal
      eClassifierBase             = 656   ///< CV index for the first classifiers
    };
  }

  namespace eeprom
  {
    /// 18 bytes per signal, starting at base address cv::eSignalBase (512)
    constexpr uint16 kSignalLen = 18;
    /// 12 bytes per classifier starting at base address cv::eClassifierBase (656)
    constexpr uint16 kInputClassifierLen = 12;
    /// Size of coding data for all signals depends on number of signals 
    /// and size of coding data for a single signal.
    constexpr uint16 kSignalsLen = kSignalLen*cfg::kNrSignals;
    /// Size of coding data for all classifiers depends on number of classifiers
    /// and size of coding data for a single classifier.
    constexpr uint16 kClassifierLen = kInputClassifierLen*cfg::kNrClassifiers;

    // Depends on hardware platform, values below for MEGA
    /// EEPROM indices.
    /// @note MEGA: max 4 KB
    /// @note NANO: max 1 KB
    enum
    {
      eDecoderAddressLSB          = 0,
      eAuxiliaryActivattion       = 1,
      eTimeOnBase                 = 2,
      eManufacturerVersionID      = 6,
      eManufacturerID             = 7,
      eDecoderAddressMSB          = 8,
      eConfiguration              = 28,
      eManufacturerCVStructureID  = 32,
      eSignalBase                 = 33,                              ///< EEPROM base address and kSignalLen (18) bytes per signal
      eClassifierBase             = eSignalBase + kSignalsLen,       ///< EEPROM base address and kInputClassifierLen (12) bytes per classifier
      eSizeOfData                 = eClassifierBase + kClassifierLen ///< One past last element = number of bytes in EEPROM
    };

    enum
    {
      eSignalInputBase = 0,
      eSignalAspectBase = 1,
      eSignalTargetBase = 11,
      eSignalChangeOverTime = 16,
      eSignalChangeOverTimeForBlinking = 17
    };
  }

  constexpr uint8  kAdDebounce = 5; ///< [10 ms] Debounce time
  constexpr uint16 kGreenLo = (605 - 10);
  constexpr uint16 kGreenHi = (605 + 10);
  constexpr uint16 kYellowLo = (399 - 10);
  constexpr uint16 kYellowHi = (399 + 10);
  constexpr uint16 kRedLo = (192 - 10);
  constexpr uint16 kRedHi = (192 + 10);
  constexpr uint16 kWhiteLo = (147 - 10);
  constexpr uint16 kWhiteHi = (147 + 10);
  constexpr uint16 kAdMin = 0;
  constexpr uint16 kAdMax = 1023;

  namespace configuration
  {
    namespace bitmask
    {
      constexpr uint8 kDecoderType      = 0b00100000;
      constexpr uint8 kAddressingMethod = 0b01000000;
      constexpr uint8 kAccessoryDecoder = 0b10000000;
    }
    constexpr uint8 kDecoderType_BasicAccessory     = 0b00000000;
    constexpr uint8 kDecoderType_ExtendedAccessory  = 0b00100000;
    constexpr uint8 kAddressingMethod_Decoder       = 0b00000000;
    constexpr uint8 kAddressingMethod_OutputAddress = 0b01000000;
    constexpr uint8 kAccessoryDecoder               = 0b00000000;
    constexpr uint8 kMultifunctionDecoder           = 0b10000000;
  }

  constexpr uint8 kAddressLSB = 2;                /* Accessory Output Address 1 */
  constexpr uint8 kAuxAct = 0;
  constexpr uint8 kTimeOn = 0;
  constexpr uint8 kManufacturerVersionID = 0x01;  /* v0.1 */
  constexpr uint8 kManufacturerID = 'S';
  constexpr uint8 kAddressMSB = 0;
  constexpr uint8 kConfiguration =  configuration::kDecoderType_BasicAccessory 
                                  | configuration::kAddressingMethod_OutputAddress
                                  | configuration::kAccessoryDecoder;

  constexpr uint8 kChangeOverTime       = 10;
  constexpr uint8 kChangeOverTimeBlink  = 0;

  constexpr uint8 kClassifierPin0 = 54;
  constexpr uint8 kClassifierPin1 = 55;
  constexpr uint8 kClassifierPin2 = 56;
  constexpr uint8 kClassifierPin3 = 57;
  constexpr uint8 kClassifierPin4 = 58;
  constexpr uint8 kClassifierPin5 = 59;
}

#define INPUT0 { cal::input_type::eNone, 0 }
#define INPUT1 { cal::input_type::eNone, 1 }
#define INPUT2 { cal::input_type::eNone, 2 }
#define INPUT3 { cal::input_type::eNone, 3 }
#define INPUT4 { cal::input_type::eNone, 4 }
#define INPUT5 { cal::input_type::eNone, 0 }

#define ASPECTS0 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS1 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS2 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS3 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS4 { { 0b00000010, 0b00000000 }, { 0b00000001, 0b00000000 }, { 0b00000000, 0b00000000 }, { 0b00000000, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS5 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } }

#define TARGET0 { { cal::target_type::eNone, 14 }, { cal::target_type::eNone, 12 }, { cal::target_type::eNone, 11 }, { cal::target_type::eNone, 10 }, { cal::target_type::eNone,  9 } }
#define TARGET1 { { cal::target_type::eNone,  8 }, { cal::target_type::eNone,  7 }, { cal::target_type::eNone,  6 }, { cal::target_type::eNone,  5 }, { cal::target_type::eNone,  4 } }
#define TARGET2 { { cal::target_type::eNone, 18 }, { cal::target_type::eNone, 17 }, { cal::target_type::eNone, 16 }, { cal::target_type::eNone, 15 }, { cal::target_type::eNone, 14 } }
#define TARGET3 { { cal::target_type::eNone, 23 }, { cal::target_type::eNone, 22 }, { cal::target_type::eNone, 21 }, { cal::target_type::eNone, 20 }, { cal::target_type::eNone, 19 } }
#define TARGET4 { { cal::target_type::eNone,  0 }, { cal::target_type::eNone,  0 }, { cal::target_type::eNone,  0 }, { cal::target_type::eNone, 28 }, { cal::target_type::eNone, 27 } }
#define TARGET5 { { cal::target_type::eNone, 30 }, { cal::target_type::eNone, 31 }, { cal::target_type::eNone, 32 }, { cal::target_type::eNone, 33 }, { cal::target_type::eNone, 34 } }

#define CAL_SIGNAL_ARRAY \
{ \
  { INPUT0, { ASPECTS0 }, { TARGET0 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
  { INPUT1, { ASPECTS1 }, { TARGET1 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
  { INPUT2, { ASPECTS2 }, { TARGET2 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
  { INPUT3, { ASPECTS3 }, { TARGET3 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
  { INPUT4, { ASPECTS4 }, { TARGET4 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
  { INPUT5, { ASPECTS5 }, { TARGET5 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
}

#define V2P(v) classifier_type::convertInput(v)

#define CLASSIFIER_LO0 { V2P(cal::kRedLo), V2P(cal::kGreenLo), V2P(cal::kYellowLo), V2P(cal::kWhiteLo), V2P(cal::kAdMax) }
#define CLASSIFIER_LO1 { V2P(cal::kRedLo), V2P(cal::kGreenLo), V2P(cal::kYellowLo), V2P(cal::kWhiteLo), V2P(cal::kAdMax) }
#define CLASSIFIER_LO2 { V2P(cal::kRedLo), V2P(cal::kGreenLo), V2P(cal::kYellowLo), V2P(cal::kWhiteLo), V2P(cal::kAdMax) }
#define CLASSIFIER_LO3 { V2P(cal::kRedLo), V2P(cal::kGreenLo), V2P(cal::kYellowLo), V2P(cal::kWhiteLo), V2P(cal::kAdMax) }
#define CLASSIFIER_LO4 { V2P(cal::kRedLo), V2P(cal::kGreenLo), V2P(cal::kAdMax   ), V2P(cal::kAdMax  ), V2P(cal::kAdMax) }
#define CLASSIFIER_LO5 { V2P(cal::kAdMax), V2P(cal::kAdMax  ), V2P(cal::kAdMax   ), V2P(cal::kAdMax  ), V2P(cal::kAdMax) }

#define CLASSIFIER_HI0 { V2P(cal::kRedHi), V2P(cal::kGreenHi), V2P(cal::kYellowHi), V2P(cal::kWhiteHi), V2P(cal::kAdMin) }
#define CLASSIFIER_HI1 { V2P(cal::kRedHi), V2P(cal::kGreenHi), V2P(cal::kYellowHi), V2P(cal::kWhiteHi), V2P(cal::kAdMin) }
#define CLASSIFIER_HI2 { V2P(cal::kRedHi), V2P(cal::kGreenHi), V2P(cal::kYellowHi), V2P(cal::kWhiteHi), V2P(cal::kAdMin) }
#define CLASSIFIER_HI3 { V2P(cal::kRedHi), V2P(cal::kGreenHi), V2P(cal::kYellowHi), V2P(cal::kWhiteHi), V2P(cal::kAdMin) }
#define CLASSIFIER_HI4 { V2P(cal::kRedHi), V2P(cal::kGreenHi), V2P(cal::kAdMin   ), V2P(cal::kAdMin  ), V2P(cal::kAdMin) }
#define CLASSIFIER_HI5 { V2P(cal::kAdMin), V2P(cal::kAdMin  ), V2P(cal::kAdMin   ), V2P(cal::kAdMin  ), V2P(cal::kAdMin) }

#define LIMITS0 { kAdDebounce, { CLASSIFIER_LO0 }, {CLASSIFIER_HI0 } }
#define LIMITS1 { kAdDebounce, { CLASSIFIER_LO1 }, {CLASSIFIER_HI1 } }
#define LIMITS2 { kAdDebounce, { CLASSIFIER_LO2 }, {CLASSIFIER_HI2 } }
#define LIMITS3 { kAdDebounce, { CLASSIFIER_LO3 }, {CLASSIFIER_HI3 } }
#define LIMITS4 { kAdDebounce, { CLASSIFIER_LO4 }, {CLASSIFIER_HI4 } }
#define LIMITS5 { kAdDebounce, { CLASSIFIER_LO5 }, {CLASSIFIER_HI5 } }

#define CLASSIFIER_SINGLE0 { cal::kClassifierPin0, LIMITS0 }
#define CLASSIFIER_SINGLE1 { cal::kClassifierPin1, LIMITS1 }
#define CLASSIFIER_SINGLE2 { cal::kClassifierPin2, LIMITS2 }
#define CLASSIFIER_SINGLE3 { cal::kClassifierPin3, LIMITS3 }
#define CLASSIFIER_SINGLE4 { cal::kClassifierPin4, LIMITS4 }
#define CLASSIFIER_SINGLE5 { cal::kClassifierPin5, LIMITS5 }

#define CAL_INPUT_CLASSIFIER_CFG { { CLASSIFIER_SINGLE0 , CLASSIFIER_SINGLE1, CLASSIFIER_SINGLE2, CLASSIFIER_SINGLE3, CLASSIFIER_SINGLE4, CLASSIFIER_SINGLE5 } }

#define CAL_LEDS_CFG { }

#define CAL_BASE_CV_CFG   .AddressLSB = cal::kAddressLSB,                        \
                          .AuxAct = cal::kAuxAct,                                \
                          .TimeOn = { cal::kTimeOn, cal::kTimeOn, cal::kTimeOn, cal::kTimeOn }, \
                          .ManufacturerVersionID = cal::kManufacturerVersionID,  \
                          .ManufacturerID = cal::kManufacturerID,                \
                          .AddressMSB = cal::kAddressMSB,                        \
                          .Configuration = cal::kConfiguration

#endif // CALM_CONFIG_H_
