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
        constexpr uint8 kInputType_DCC   = 0;
        constexpr uint8 kInputType_ADC   = 1;
        constexpr uint8 kInputType_DI    = 2;
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
  constexpr uint8 kConfiguration =  configuration::kDecoderType_BasicAccessory      /* Configuration CV 29 */
                                  | configuration::kAddressingMethod_OutputAddress
                                  | configuration::kAccessoryDecoder;
  constexpr uint8 kDccAddressingMode = kRCN123;   /* CV 39 */

  constexpr uint8 kChangeOverTime       = 10;
  constexpr uint8 kChangeOverTimeBlink  = 0;
  
#define V2P(v) classifier_type::convertInput(v)

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

  constexpr uint8 kClassifierPin0 = 54;
  constexpr uint8 kClassifierPin1 = 55;
  constexpr uint8 kClassifierPin2 = 56;
  constexpr uint8 kClassifierPin3 = 57;
  constexpr uint8 kClassifierPin4 = 58;
  constexpr uint8 kClassifierPin5 = 59;

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
        kManufacturerVersionID,     /* CV 33 */                             \
        0, 0, 0, 0, 0,              /* 34 - 38 */                           \
        kDccAddressingMode,         /* CV 39 */                             \
        cfg::kNrSignals,            /* CV 40 */                             \
        cfg::kNrBuiltInSignals,     /* CV 41 */                             \
        kSignalNotUsed, kSignalNotUsed, kSignalNotUsed, kSignalNotUsed,     \
        kSignalNotUsed, kSignalNotUsed, kSignalNotUsed, kSignalNotUsed, /* CV 42 - 49 (signal IDs) */  \
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
        kRedLo, kGreenLo, kYellowLo, kWhiteLo, kAdMax, /* CV 113 - 117 */   \
        kRedHi, kGreenHi, kYellowHi, kWhiteHi, kAdMax, /* CV 118 - 122 */   \
        kAdDebounce,                /* CV 123 */                            \
        kRedLo, kGreenLo, kYellowLo, kWhiteLo, kAdMax, /* CV 124 - 128 */   \
        kRedHi, kGreenHi, kYellowHi, kWhiteHi, kAdMax, /* CV 129 - 133 */   \
        0,                          /* CV 134 */                            \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 135 - 142 */                      \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 143 - 150 */                      \
        0, 0,                       /* CV 151, 152 */                       \
        0,                          /* CV 153 */                            \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 154 - 161 */                      \
        0, 0, 0, 0, 0, 0, 0, 0,     /* CV 162 - 169 */                      \
        0, 0                        /* CV 170, 171 */                       \
      }
//#define INPUT0 { cal::input_type::eNone, 0 }
//#define INPUT1 { cal::input_type::eNone, 1 }
//#define INPUT2 { cal::input_type::eNone, 2 }
//#define INPUT3 { cal::input_type::eNone, 3 }
//#define INPUT4 { cal::input_type::eNone, 4 }
//#define INPUT5 { cal::input_type::eNone, 0 }

#define NR_TARGETS0  5
#define NR_TARGETS1  2
#define ASPECTS0 { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 }, { 0b00011111, 0b00000000 }, { 0b00011111, 0b00000000 }, { 0b00011111, 0b00000000 } }
#define ASPECTS1 { { 0b00000001, 0b00000000 }, { 0b00000010, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 }, { 0b00000011, 0b00000000 } }

//#define TARGET0 { { cal::target_type::eNone, 14 }, { cal::target_type::eNone, 12 }, { cal::target_type::eNone, 11 }, { cal::target_type::eNone, 10 }, { cal::target_type::eNone,  9 } }
//#define TARGET1 { { cal::target_type::eNone,  8 }, { cal::target_type::eNone,  7 }, { cal::target_type::eNone,  6 }, { cal::target_type::eNone,  5 }, { cal::target_type::eNone,  4 } }
//#define TARGET2 { { cal::target_type::eNone, 18 }, { cal::target_type::eNone, 17 }, { cal::target_type::eNone, 16 }, { cal::target_type::eNone, 15 }, { cal::target_type::eNone, 14 } }
//#define TARGET3 { { cal::target_type::eNone, 23 }, { cal::target_type::eNone, 22 }, { cal::target_type::eNone, 21 }, { cal::target_type::eNone, 20 }, { cal::target_type::eNone, 19 } }
//#define TARGET4 { { cal::target_type::eNone,  0 }, { cal::target_type::eNone,  0 }, { cal::target_type::eNone,  0 }, { cal::target_type::eNone, 28 }, { cal::target_type::eNone, 27 } }
//#define TARGET5 { { cal::target_type::eNone, 30 }, { cal::target_type::eNone, 31 }, { cal::target_type::eNone, 32 }, { cal::target_type::eNone, 33 }, { cal::target_type::eNone, 34 } }

#define CAL_BUILT_IN_SIGNAL_OUTPUTS \
{ { \
  { NR_TARGETS0, { ASPECTS0 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
  { NR_TARGETS1, { ASPECTS1 }, cal::kChangeOverTime, cal::kChangeOverTimeBlink }, \
} }

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
