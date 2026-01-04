/**
 * @file App/Signal/Cal/CalM_config.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Define calibration datasets
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CALM_CONFIG_H_
#define CALM_CONFIG_H_

#include <Cfg_Prj.h> // number of signals, number of classifiers and classifier classes
#include <Cal/CalM_Types.h>
#include <Util/bitset.h>

namespace cal
{
    namespace eeprom
    {
        // Depends on hardware platform.
        /// EEPROM indices.
        /// @note MEGA: max 4 KB
        /// @note NANO: max 1 KB
        constexpr uint16 kDecoderAddressLSB                 = cv::kDecoderAddressLSB;
        constexpr uint16 kAuxiliaryActivation               = cv::kAuxiliaryActivation;
        constexpr uint16 kTimeOnBase                        = cv::kTimeOnBase;
        constexpr uint16 kManufacturerVersionID             = cv::kManufacturerVersionID;
        constexpr uint16 kManufacturerID                    = cv::kManufacturerID;
        constexpr uint16 kDecoderAddressMSB                 = cv::kDecoderAddressMSB;
        constexpr uint16 kConfiguration                     = cv::kConfiguration;
        constexpr uint16 kManufacturerCVStructureID         = cv::kManufacturerCVStructureID;
        constexpr uint16 kDccAddressingMethod               = cv::kDccAddressingMethod;
        constexpr uint16 kMaximumNumberOfSignals            = cv::kMaximumNumberOfSignals;
        constexpr uint16 kMaximumNumberOfBuiltInSignalIDs   = cv::kMaximumNumberOfBuiltInSignalIDs;
        constexpr uint16 kSignalIDBase                      = cv::kSignalIDBase;
        constexpr uint16 kSignalFirstOutputBase             = cv::kSignalFirstOutputBase;
        constexpr uint16 kSignalInputBase                   = cv::kSignalInputBase;
        constexpr uint16 kSignalInputClassifierTypeBase     = cv::kSignalInputClassifierTypeBase;
        constexpr uint16 kSignalOutputConfigBase            = cv::kSignalOutputConfigBase;
        constexpr uint16 kClassifierBase                    = cv::kClassifierBase;
        constexpr uint16 kUserDefinedSignalBase             = cv::kUserDefinedSignalBase;
        constexpr uint16 kSizeOfData                        = cv::kLastCV;
        ///< One past last element = number of bytes in EEPROM
    }

    constexpr uint8 kAddressLSB = 1; /* DCC Address LSB */
    constexpr uint8 kAddressMSB = 0; /* DCC Address MSB */
    constexpr uint8 kAuxAct = 0;
    constexpr uint8 kTimeOn = 0;
    constexpr uint8 kManufacturerVersionID = cfg::kManufacturerVersionID;
    constexpr uint8 kManufacturerID = 'S';                                      /* Sondershaus */
    constexpr uint8 kManufacturerCVStructureID = 0x10;                          /* v1.0 */
    constexpr uint8 kConfiguration = configuration::kDecoderType_BasicAccessory /* Configuration CV 29 */
                                     | configuration::kAddressingMethod_OutputAddress | configuration::kAccessoryDecoder;
    constexpr uint8 kDccAddressingMode = kRCN123; /* CV 39 */

    constexpr uint8 kChangeOverTime = 10;
    constexpr uint8 kChangeOverTimeBlink = 0;

/// Converts ADC values (0-1023) to classifier limit values (0-255) by dividing by 4.
#define V2P(v)  ((v) / 4U)

    constexpr uint8 kAdDebounce = 5; ///< [10 ms] Debounce time
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

#define EEPROM_INIT {                                                                                                                                         \
    0,                                                                                                                  /* CV 0 does not exist */             \
    kAddressLSB,                                                                                                        /* CV 1 */                            \
    kAuxAct,                                                                                                            /* CV 2 */                            \
    kTimeOn,                                                                                                            /* CV 3 */                            \
    kTimeOn,                                                                                                            /* CV 4 */                            \
    kTimeOn,                                                                                                            /* CV 5 */                            \
    kTimeOn,                                                                                                            /* CV 6 */                            \
    kManufacturerVersionID,                                                                                             /* CV 7 */                            \
    kManufacturerID,                                                                                                    /* CV 8 */                            \
    kAddressMSB,                                                                                                        /* CV 9 */                            \
    0, 0, 0, 0, 0,                                                                                                      /* 10 - 14 */                         \
    0, 0, 0, 0, 0,                                                                                                      /* 15 - 19 */                         \
    0, 0, 0, 0, 0,                                                                                                      /* 20 - 24 */                         \
    0, 0, 0,                                                                                                            /* 25 - 27 */                         \
    0,                                                                                                                  /* CV 28 */                           \
    kConfiguration,                                                                                                     /* CV 29 */                           \
    0, 0, 0,                                                                                                            /* CV 30 - 32 */                      \
    kManufacturerCVStructureID,                                                                                         /* CV 33 */                           \
    0, 0, 0, 0, 0,                                                                                                      /* 34 - 38 */                         \
    kDccAddressingMode,                                                                                                 /* CV 39 */                           \
    cfg::kNrSignals,                                                                                                    /* CV 40 */                           \
    cfg::kNrBuiltInSignals,                                                                                             /* CV 41 */                           \
    cal::constants::kSignalNotUsed, cal::constants::kSignalNotUsed, cal::constants::kSignalNotUsed, cal::constants::kSignalNotUsed, /* CV 42 - 45 (signal IDs) */         \
    cal::constants::kSignalNotUsed, cal::constants::kSignalNotUsed, cal::constants::kSignalNotUsed, cal::constants::kSignalNotUsed, /* CV 46 - 49 (signal IDs) */         \
    0, 0, 0, 0, 0, 0, 0, 0,                                                                                             /* CV 50 - 57 (signal output pins) */ \
    0, 0, 0, 0, 0, 0, 0, 0,                                                                                             /* CV 58 - 65 (signal inputs) */      \
    0, 0, 0, 0, 0, 0, 0, 0,                                                                                             /* CV 66 - 73 (signal classifiers) */ \
    0, 0, 0, 0, 0,                                                                                                      /* 74 - 78 */                         \
    0, 0, 0,                                                                                                            /* 79 - 81 */                         \
    0, 0, 0, 0, 0,                                                                                                      /* 82 - 86 */                         \
    0, 0, 0, 0, 0,                                                                                                      /* 87 - 91 */                         \
    0, 0, 0, 0, 0,                                                                                                      /* 92 - 96 */                         \
    0, 0, 0, 0, 0,                                                                                                      /* 97 - 101 */                        \
    0, 0, 0, 0, 0,                                                                                                      /* 102 - 106 */                       \
    0, 0, 0, 0, 0,                                                                                                      /* 107 - 111 */                       \
    kAdDebounce,                                                                                                        /* CV 112 */                          \
    V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax),                                             /* CV 113 - 117 lower limits */       \
    V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin),                                             /* CV 118 - 122 upper limits */       \
    kAdDebounce,                                                                                                        /* CV 123 */                          \
    V2P(kRedLo), V2P(kGreenLo), V2P(kYellowLo), V2P(kWhiteLo), V2P(kAdMax),                                             /* CV 124 - 128 lower limits */       \
    V2P(kRedHi), V2P(kGreenHi), V2P(kYellowHi), V2P(kWhiteHi), V2P(kAdMin),                                             /* CV 129 - 133 upper limits */       \
    0,                                                                                                                  /* CV 134 */                          \
    0, 0, 0, 0, 0, 0, 0, 0,                                                                                             /* CV 135 - 142 */                    \
    0, 0, 0, 0, 0, 0, 0, 0,                                                                                             /* CV 143 - 150 */                    \
    0, 0,                                                                                                               /* CV 151, 152 */                     \
    0,                                                                                                                  /* CV 153 */                          \
    0, 0, 0, 0, 0, 0, 0, 0,                                                                                             /* CV 154 - 161 */                    \
    0, 0, 0, 0, 0, 0, 0, 0,                                                                                             /* CV 162 - 169 */                    \
    0, 0                                                                                                                /* CV 170, 171 */                     \
}

} // namespace cal

#endif // CALM_CONFIG_H_
