 /**
 * @file Cal/CalM_Type_Prj.h
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
#include <Util/Array.h>
#include <Util/Classifier.h>
#include <Util/bitset.h>
#include <Util/Ptr.h>

namespace cal
{
  /// @brief An invalid pin number.
  static constexpr uint8 kInvalidPin = 255;

  /// Define inputs: classified AD values, commands received via busses (such as DCC), etc
  typedef struct
  {
    enum
    {
      eDcc  = 0,
      eAdc  = 1,
      eDig  = 2
    };

    uint8 type; ///< [eDcc, eAdc, eDig] type of command sources
    uint8 idx;  ///< Index of the input element on RTE
  } input_type;

  /// Define target outputs such as onboard or external and which output pin
  /// 1 byte (8 bits)
  typedef struct
  {
    enum
    {
      eOnboard = 0,
      eExternal = 1
    };

    explicit operator uint8() const { return static_cast<uint8>((idx << (8U - cfg::kCalTgtNrBits)) | type); }

    uint8 idx  : (cfg::kCalTgtNrBits);      ///< output pin number
    uint8 type : (8U - cfg::kCalTgtNrBits); ///< type of target (eOnboard, eExternal)
  } target_type;

  /// Define signal target intensities
  /// 2 bytes (16 bits)
  typedef struct
  {
    uint8 aspect; ///< A bit per output, max. 8 outputs: 0 = 0%, 1 = 100%, LSB = 1st output, MSB = 8th output
    uint8 blink;  ///< A bit per output, max. 8 outputs: 0 = no blinking, 1 = blinking
  } aspect_type;

  /// @brief Define data that are related to LED output
  typedef struct
  {
    uint8 num_targets; ///< Number of outputs (LEDs)
    util::array<aspect_type, cfg::kNrSignalAspects> aspects; ///< Aspects and Blink
    uint8 change_over_time;       ///< [10 ms] dim time if aspect changes
    uint8 change_over_time_blink; ///< [10 ms] dim time for blinking effects
  } output_type;

  /**
   * @brief Data type for a signal
   */
  typedef struct
  {
    enum
    {
        eSignalNotUsed = 0,
        eFirstBuiltInSignalId = 1,
        eFirstUserDefinedSignalID = 128
    };

    input_type input;
    target_type first_target;
    util::ptr<const output_type> output_ptr; ///< If empty, the signal is not used
    uint8 signal_id; ///< Remember the signal ID of output_ptr: eSignalNotUsed, 
                     ///< eFirstBuiltInSignalId, ..., eFirstUserDefinedSignalID, ...
  } signal_type;

  /// Calibration data type for each signal
  using signal_cal_type = util::array<signal_type, cfg::kNrSignals>;

  /// Base CVs for this decoder
  typedef struct
  {
    uint8 CV1_address_LSB;
    uint8 CV7_manufacturer_version_ID;
    uint8 CV8_manufacturer_ID;
    uint8 CV9_address_MSB;
    uint8 CV29_configuration;
  } base_cv_type;

  /// Base CVs for this decoder
  using base_cv_cal_type = base_cv_type;

  /// Calibration data for an array of classifiers.
  using classifier_array_cal_type = util::cal::classifier_array<cfg::kNrClassifiers, cfg::kNrClassifierClasses>;
  using classifier_array_element_type = classifier_array_cal_type::classifier_array_element_type;
  using classifier_type = classifier_array_element_type::classifier_type;

  /// Calibration data type for LED complex device drivers
  /// A bit for each pin: 1 = is output, 0 = is not output
  using led_output_rw_type =  util::bitset<uint32_t, cfg::kCalTgtNrBits>;
  using led_cal_type = led_output_rw_type;

  namespace cv
  {
    /// 19 bytes per user defined signal (with 8 aspects)
    constexpr uint16 kUserDefinedSignalLength = 19;

    /// @brief CVs and CV base addresses
    enum
    {
      eDecoderAddressLSB                = 1,
      eAuxiliaryActivattion             = 2,
      eTimeOnBase                       = 3,
      eManufacturerVersionID            = 7,
      eManufacturerID                   = 8,
      eDecoderAddressMSB                = 9,
      eConfiguration                    = 29,
      eManufacturerCVStructureID        = 33,
      eDccAddressingMethod              = 39,
      eMaximumNumberOfSignals           = 40,
      eMaximumNumberOfBuiltInSignalIDs  = 41,
      eSignalIDBase                     = 42,   // up to cfg::kNrSignals signals
      eSignalFirstOutputBase            = 50,
      eSignalInputBase                  = 58,
      eSignalInputClassifierTypeBase    = 66,
      eClassifierBase                   = 112, // Number of classifiers: cfg::kNrClassifiers
                                               // with number of classes: cfg::kNrClassifierClasses
      eClassifierType1LoLimitBase       = 113,
      eClassifierType1HiLimitBase       = 118,
      eClassifierType2LoLimitBase       = 124,
      eClassifierType2HiLimitBase       = 129,
      eUserDefinedSignalBase            = 134,
      eLastCV                           = static_cast<uint16>(eUserDefinedSignalBase)
                                        + kUserDefinedSignalLength*cfg::kNrUserDefinedSignals 
                                         ///< One past last element = number of bytes
    };
  }

  constexpr uint8 kRCN123 = 1;          /* for CV 39 kDccAddressingMode */
  constexpr uint8 kRoco = 0;            /* for CV 39 kDccAddressingMode */

  constexpr uint8 kSignalNotUsed = 0;
  constexpr uint8 kFirstBuiltInSignalID = 1;
  constexpr uint8 kFirstUserDefinedSignalID = 128;

  /* CV1 contains the eight least significant bits of the Output Address */
  constexpr uint8 kMaskCV1_address_LSB = 0xFF; 
  /* CV9 contains the three most significant bits of the Output Address */
  constexpr uint8 kMaskCV9_address_MSB = 0x07; 
} // namespace cal

#endif // CALM_TYPE_PRJ_H_
