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

#include <Cfg_Prj.h> // number of signals, number of classifiers and classifier classes
#include <Cal/CalM_Type.h>
#include <Util/bitset.h>

namespace cal
{
  namespace eeprom
  {
    /// 19 bytes per user defined signal (with 8 aspects)
    constexpr uint16 kUserDefinedSignalLength = cv::kUserDefinedSignalLength;

    // Depends on hardware platform.
    /// EEPROM indices.
    /// @note MEGA: max 4 KB
    /// @note NANO: max 1 KB
    enum
    {
      eDecoderAddressLSB               = cv::eDecoderAddressLSB,
      eAuxiliaryActivattion            = cv::eAuxiliaryActivattion,
      eTimeOnBase                      = cv::eTimeOnBase,
      eManufacturerVersionID           = cv::eManufacturerVersionID,
      eManufacturerID                  = cv::eManufacturerID,
      eDecoderAddressMSB               = cv::eDecoderAddressMSB,
      eConfiguration                   = cv::eConfiguration,
      eManufacturerCVStructureID       = cv::eManufacturerCVStructureID,
      eDccAddressingMethod             = cv::eDccAddressingMethod,
      eMaximumNumberOfSignals          = cv::eMaximumNumberOfSignals,
      eMaximumNumberOfBuiltInSignalIDs = cv::eMaximumNumberOfBuiltInSignalIDs,
      eSignalIDBase                    = cv::eSignalIDBase,
      eSignalFirstOutputBase           = cv::eSignalFirstOutputBase,
      eSignalInputBase                 = cv::eSignalInputBase,
      eSignalInputClassifierTypeBase   = cv::eSignalInputClassifierTypeBase,
      eClassifierBase                  = cv::eClassifierBase,
      eUserDefinedSignalBase           = cv::eUserDefinedSignalBase,
      eSizeOfData                      = cv::eLastCV 
                                         ///< One past last element = number of bytes in EEPROM
    };

    // enum
    // {
    //   eUserDefinedSignalNrOutputs   = 0,
    //   eUserDefinedSignalAspectsBase = 1,
    //   eUserDefinedSignalChangeOverTime = 18
    // };
  }

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

  namespace signal
  {
    namespace bitmask
    {
        constexpr uint8 kFirstOutputType    = 0b11000000;
        constexpr uint8 kFirstOutputPin     = 0b00111111;
        constexpr uint8 kInputType          = 0b11000000;
        constexpr uint8 kAdcPin             = 0b00111111;
        constexpr uint8 kClassifierType     = 0b00000011;
    }
    namespace bitshift
    {
        constexpr uint8 kFirstOutputType = 6;
        constexpr uint8 kFirstOutputPin  = 0;
        constexpr uint8 kInputType       = 6;
        constexpr uint8 kAdcPin          = 0;
        constexpr uint8 kClassifierType  = 0;
    }
    namespace values
    {
        constexpr uint8 kOutputType_Onboard   = target_type::eOnboard;
        constexpr uint8 kOutputType_External  = target_type::eExternal;

        constexpr uint8 kInputType_DCC   = input_type::eDcc;
        constexpr uint8 kInputType_ADC   = input_type::eAdc;
        constexpr uint8 kInputType_DI    = input_type::eDig;
    }
    constexpr uint8 make_signal_input(uint8 input_type, uint8 input_pin) 
    {
        return (input_type << cal::signal::bitshift::kInputType) |
               (input_pin  << cal::signal::bitshift::kAdcPin);
    }
    constexpr uint8 make_signal_first_output(uint8 output_type, uint8 output_pin) 
    {
        return (output_type << cal::signal::bitshift::kFirstOutputType) |
               (output_pin  << cal::signal::bitshift::kFirstOutputPin);
    }
  }

  namespace user_defined_signal
  {
    namespace bitmask
    {
        constexpr uint8 kNumberOfOutputs    = 0b00001111;
    }
    namespace bitshift
    {
        constexpr uint8 kNumberOfOutputs    = 0;
    }
  }

  constexpr uint8 kAddressLSB = 2;      /* DCC Address LSB */
  constexpr uint8 kAddressMSB = 0;      /* DCC Address MSB */
  constexpr uint8 kAuxAct = 0;
  constexpr uint8 kTimeOn = 0;
  constexpr uint8 kManufacturerVersionID = 0x01;  /* v0.1 */
  constexpr uint8 kManufacturerID = 'S';          /* Sondershaus */
  constexpr uint8 kManufacturerCVStructureID = 0x10;   /* v1.0 */
  constexpr uint8 kConfiguration =  configuration::kDecoderType_BasicAccessory      /* Configuration CV 29 */
                                  | configuration::kAddressingMethod_OutputAddress
                                  | configuration::kAccessoryDecoder;
  constexpr uint8 kDccAddressingMode = kRCN123;   /* CV 39 */

  constexpr uint8 kChangeOverTime       = 10;
  constexpr uint8 kChangeOverTimeBlink  = 0;
  
#define V2P(v) util::classifier<cfg::kNrClassifierClasses>::convert_input(v)

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

#define EEPROM_INIT {                                                       \
        0,              /* CV 0 does not exist */                           \
        kAddressLSB,    /* CV 1 */                                          \
        kAuxAct,        /* CV 2 */                                          \
        kTimeOn,        /* CV 3 */                                          \
        kTimeOn,        /* CV 4 */                                          \
        kTimeOn,        /* CV 5 */                                          \
        kTimeOn,        /* CV 6 */                                          \
        kManufacturerVersionID, /* CV 7 */                                  \
        kManufacturerID,        /* CV 8 */                                  \
        kAddressMSB,            /* CV 9 */                                  \
        0, 0, 0, 0, 0,  /* 10 - 14 */                                       \
        0, 0, 0, 0, 0,  /* 15 - 19 */                                       \
        0, 0, 0, 0, 0,  /* 20 - 24 */                                       \
        0, 0, 0,        /* 25 - 27 */                                       \
        0,              /* CV 28 */                                         \
        kConfiguration, /* CV 29 */                                         \
        0, 0, 0,        /* CV 30 - 32 */                                    \
        kManufacturerCVStructureID, /* CV 33 */                             \
        0, 0, 0, 0, 0,              /* 34 - 38 */                           \
        kDccAddressingMode,         /* CV 39 */                             \
        cfg::kNrSignals,            /* CV 40 */                             \
        cfg::kNrBuiltInSignals,     /* CV 41 */                             \
        kSignalNotUsed, kSignalNotUsed, kSignalNotUsed, kSignalNotUsed, /* CV 42 - 45 (signal IDs) */ \
        kSignalNotUsed, kSignalNotUsed, kSignalNotUsed, kSignalNotUsed, /* CV 46 - 49 (signal IDs) */ \
        0, 0, 0, 0,  0, 0, 0, 0,    /* CV 50 - 57 (signal outputs) */       \
        0, 0, 0, 0,  0, 0, 0, 0,    /* CV 58 - 65 (signal inputs) */        \
        0, 0, 0, 0,  0, 0, 0, 0,    /* CV 66 - 73 (signal classifiers) */   \
        0, 0, 0, 0, 0,              /* 74 - 78 */                           \
        0, 0, 0,                    /* 79 - 81 */                           \
        0, 0, 0, 0, 0,              /* 82 - 86 */                           \
        0, 0, 0, 0, 0,              /* 87 - 91 */                           \
        0, 0, 0, 0, 0,              /* 92 - 96 */                           \
        0, 0, 0, 0, 0,              /* 97 - 101 */                          \
        0, 0, 0, 0, 0,              /* 102 - 106 */                         \
        0, 0, 0, 0, 0,              /* 107 - 111 */                         \
        kAdDebounce,                /* CV 112 */                            \
        V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax), /* CV 113 - 117 lower limits */   \
        V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin), /* CV 118 - 122 upper limits */   \
        kAdDebounce,                /* CV 123 */                            \
        V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax), /* CV 124 - 128 lower limits */   \
        V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin), /* CV 129 - 133 upper limits */   \
        0,                          /* CV 134 */                            \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 135 - 142 */                      \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 143 - 150 */                      \
        0, 0,                       /* CV 151, 152 */                       \
        0,                          /* CV 153 */                            \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 154 - 161 */                      \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 162 - 169 */                      \
        0, 0                        /* CV 170, 171 */                       \
      }

#define NR_TARGETS0  5
#define NR_TARGETS1  2
#define ASPECTS0 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 }, { 0b00011111, 0b00000000 }, { 0b00011111, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS1 { { 0b00000001, 0b00000000 }, { 0b00000010, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 } }

#define CAL_BUILT_IN_SIGNAL_OUTPUTS \
{ { \
  { NR_TARGETS0, { ASPECTS0 }, kChangeOverTime, kChangeOverTimeBlink }, \
  { NR_TARGETS1, { ASPECTS1 }, kChangeOverTime, kChangeOverTimeBlink }, \
} }

} // namespace cal

#endif // CALM_CONFIG_H_
