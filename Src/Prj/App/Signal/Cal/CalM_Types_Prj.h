/**
 * @file Cal/CalM_Types_Prj.h
 *
 * @brief Defines project specific calibration types.
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

#ifndef CALM_TYPE_PRJ_H_
#define CALM_TYPE_PRJ_H_

#include <Cfg_Prj.h>
#include <Util/bitset.h>

namespace cal
{
    namespace cv
    {
        /// 19 bytes per signal (with 8 aspects)
        constexpr uint16 kSignalLength = 19;
        constexpr uint16 kClassifierLength = 11; // 11 bytes per classifier type (with 5 classes)

        /**
         * @brief CVs and CV base addresses
         *
         * The CV numbers are independent of the hardware platform.
         */
        constexpr uint16 kDecoderAddressLSB = 1;
        constexpr uint16 kAuxiliaryActivation = 2;
        constexpr uint16 kTimeOnBase = 3;
        constexpr uint16 kManufacturerVersionID = 7;
        constexpr uint16 kManufacturerID = 8;
        constexpr uint16 kDecoderAddressMSB = 9;
        constexpr uint16 kConfiguration = 29;
        constexpr uint16 kManufacturerCVStructureID = 33;
        constexpr uint16 kDccAddressingMethod = 39;
        constexpr uint16 kMaximumNumberOfSignals = 40;
        constexpr uint16 kMaximumNumberOfBuiltInSignalIDs = 41;
        constexpr uint16 kSignalIDBase = 42; // up to cfg::kNrSignals signals
        constexpr uint16 kSignalFirstOutputBase = 50;
        constexpr uint16 kSignalInputBase = 58;
        constexpr uint16 kSignalInputClassifierTypeBase = 66;
        constexpr uint16 kClassifierBase = 112; // Number of classifiers: cfg::kNrClassifiers
                                                // with number of classes: cfg::kNrClassifierClasses
        constexpr uint16 kUserDefinedSignalBase = 134;
        constexpr uint16 kLastCV = static_cast<uint16>(kUserDefinedSignalBase) + kSignalLength * cfg::kNrUserDefinedSignals;
        ///< One past last element = number of bytes
    }

    /** @brief An invalid pin number. */
    static constexpr uint8 kInvalidPin = 255;

    /**
     * @brief Bit masks, bit shifts and values for signal configuration
     */
    namespace constants
    {
        constexpr uint8 kDcc = 0; /**< DCC input type */
        constexpr uint8 kAdc = 1; /**< ADC input type */
        constexpr uint8 kDig = 2; /**< Digital input type */

        constexpr uint8 kOnboard = 0;  /**< Onboard output type */
        constexpr uint8 kExternal = 1; /**< External output type */

        constexpr uint8 kSignalNotUsed = 0;              /**< Signal not used ID */
        constexpr uint8 kFirstBuiltInSignalID = 1;       /**< First built-in signal ID */
        constexpr uint8 kFirstUserDefinedSignalID = 128; /**< First user-defined signal ID */

        namespace bitmask
        {
            constexpr uint8 kFirstOutputType = 0b11000000; /**< First output type mask */
            constexpr uint8 kFirstOutputPin = 0b00111111;  /**< First output pin mask */
            constexpr uint8 kInputType = 0b11000000;       /**< Input type mask */
            constexpr uint8 kAdcPin = 0b00111111;          /**< ADC pin mask */
            constexpr uint8 kClassifierType = 0b00000011;  /**< Classifier type mask */
            constexpr uint8 kNumberOfOutputs = 0b00001111;  /**< Number of outputs mask */
        }
        namespace bitshift
        {
            constexpr uint8 kFirstOutputType = 6; /**< First output type shift */
            constexpr uint8 kFirstOutputPin = 0;  /**< First output pin shift */
            constexpr uint8 kInputType = 6;       /**< Input type shift */
            constexpr uint8 kAdcPin = 0;          /**< ADC pin shift */
            constexpr uint8 kClassifierType = 0;  /**< Classifier type shift */
            constexpr uint8 kNumberOfOutputs = 0;  /**< Number of outputs shift */
        }
        /** @brief Create a CV for signal input value. */
        constexpr uint8 make_signal_input(uint8 input_type, uint8 input_pin) noexcept
        {
            return (input_type << cal::constants::bitshift::kInputType) |
                   (input_pin << cal::constants::bitshift::kAdcPin);
        }
        /** @brief Create a CV for signal first output value. */
        constexpr uint8 make_signal_first_output(uint8 output_type, uint8 output_pin) noexcept
        {
            return (output_type << cal::constants::bitshift::kFirstOutputType) |
                   (output_pin << cal::constants::bitshift::kFirstOutputPin);
        }
        /** @brief Extract the signal input type from a CV. */
        constexpr uint8 extract_signal_input_type(uint8 cv_value) noexcept
        {
            return util::bits::masked_shift(
                cv_value,
                cal::constants::bitmask::kInputType,
                cal::constants::bitshift::kInputType);
        }
        /** @brief Extract the signal input pin from a CV. */
        constexpr uint8 extract_signal_input_pin(uint8 cv_value) noexcept
        {
            return util::bits::masked_shift(
                cv_value,
                cal::constants::bitmask::kAdcPin,
                cal::constants::bitshift::kAdcPin);
        }
        /** @brief Extract the signal first output pin type from a CV. */
        constexpr uint8 extract_signal_first_output_type(uint8 cv_value) noexcept
        {
            return util::bits::masked_shift(
                cv_value,
                cal::constants::bitmask::kFirstOutputType,
                cal::constants::bitshift::kFirstOutputType);
        }
        /** @brief Extract the signal first output pin from a CV. */
        constexpr uint8 extract_signal_first_output_pin(uint8 cv_value) noexcept
        {
            return util::bits::masked_shift(
                cv_value,
                cal::constants::bitmask::kFirstOutputPin,
                cal::constants::bitshift::kFirstOutputPin);
        }
    }

    namespace base_cv
    {
        namespace bitmask
        {
            /** CV1 contains the eight least significant bits of the Output Address */
            constexpr uint8 kCV1_address_LSB = 0xFF;
            /** CV9 contains the three most significant bits of the Output Address */
            constexpr uint8 kCV9_address_MSB = 0x07;
        }
        namespace bitshift
        {
            constexpr uint8 kCV1_address_LSB = 0;
            constexpr uint8 kCV9_address_MSB = 0;
        }
    }
    /**
     * @brief Bit masks and values for configuration CV 29
     */
    namespace configuration
    {
        namespace bitmask
        {
            constexpr uint8 kDecoderType = 0b00100000;
            constexpr uint8 kAddressingMethod = 0b01000000;
            constexpr uint8 kAccessoryDecoder = 0b10000000;
        }
        constexpr uint8 kDecoderType_BasicAccessory = 0b00000000;
        constexpr uint8 kDecoderType_ExtendedAccessory = 0b00100000;
        constexpr uint8 kAddressingMethod_Decoder = 0b00000000;
        constexpr uint8 kAddressingMethod_OutputAddress = 0b01000000;
        constexpr uint8 kAccessoryDecoder = 0b00000000;
        constexpr uint8 kMultifunctionDecoder = 0b10000000;
    }

    constexpr uint8 kRCN123 = 1; /* for CV 39 kDccAddressingMode */
    constexpr uint8 kRoco = 0;   /* for CV 39 kDccAddressingMode */

} // namespace cal

#endif // CALM_TYPE_PRJ_H_
