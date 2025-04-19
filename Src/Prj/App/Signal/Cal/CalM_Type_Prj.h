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

namespace cal
{
  /// Define inputs: classified AD values, commands received via busses (such as DCC), etc
  typedef union
  {
    enum
    {
      eNone = 0,
      eClassified = 1,
      eDcc = 2
    };

    struct
    {
      uint8 type : 2; ///< [eNone, eClassified, eDcc] type of command sources, more in future
      uint8 idx : 6; ///< Index of the input element on RTE
    } bits;

    uint8 raw;
  } input_type;

  /// Define target outputs such as onboard or external and which output pin
  /// 1 byte (8 bits)
  typedef struct
  {
    enum
    {
      eNone = 0,
      eOnboard = 1,
      eExternal = 2
    };

    uint8 type : (8U - cfg::kCalTgtNrBits); ///< type of target such as eNone, eOnboard, eExternal
    uint8 idx  : (cfg::kCalTgtNrBits);      ///< output pin number
  } target_type;

  /// Define signal target intensities
  /// 2 bytes (16 bits)
  typedef struct
  {
    uint8 aspect; ///< A bit per output, max. 8 outputs: 0 = 0%, 1 = 100%, LSB = 1st output, MSB = 8th output
    uint8 blink;  ///< A bit per output, max. 8 outputs: 0 = no blinking, 1 = blinking
  } aspect_type;

  /// Coding data type for a signal
  /// 
  /// Example byte layout with kNrSignalAspects = 5, kNrSignalTargets = 5
  /// and 18 bytes alltogether
  /// <CODE>
  ///  0     input
  ///  1, 2  aspect 0   (aspect, blink)
  ///  3, 4  aspect 1   (aspect, blink)
  ///  5, 6  aspect 2   (aspect, blink)
  ///  7, 8  aspect 3   (aspect, blink)
  ///  9,10  aspect 4   (aspect, blink)
  /// 11     target 0   (type, idx)
  /// 12     target 1   (type, idx)
  /// 13     target 2   (type, idx)
  /// 14     target 3   (type, idx)
  /// 15     target 4   (type, idx)
  /// 16     changeOverTime
  /// 17     blinkChangeOverTime;
  /// </CODE>
  typedef struct
  {
    input_type input;
    util::array<aspect_type, cfg::kNrSignalAspects> aspects;
    util::array<target_type, cfg::kNrSignalTargets> targets;
    uint8 changeOverTime; ///< [10 ms] dim time if aspect changes
    uint8 blinkChangeOverTime; ///< [10 ms] dim time for blinking effects
  } signal_type;

  /// Calibration data type for each signal
  using signal_cal_type = util::array<signal_type, cfg::kNrSignals>;

  /// Calibration data for DccDecoder
  typedef struct
  {
    uint8 AddressLSB;
    uint8 AuxAct;
    uint8 TimeOn[4];
    uint8 ManufacturerVersionID;
    uint8 ManufacturerID;
    uint8 AddressMSB;
    uint8 Configuration;
  } base_cv_type;

  /// Calibration data for DccDecoder
  using base_cv_cal_type = base_cv_type;

  /// Calibration data for input classifiers is taken over from util::input_classifier
  using input_classifier_type        = util::input_classifier<cfg::kNrClassifiers, cfg::kNrClassifierClasses>;
  using input_classifier_cal_type    = input_classifier_type::input_classifier_cal_type;
  using input_classifier_single_type = input_classifier_cal_type::input_classifier_single_type;

  /// Calibration data type for LED complex device drivers
  /// A bit for each pin: 1 = is output, 0 = is not output
  using led_output_rw_type =  util::bitset<uint32_t, cfg::kCalTgtNrBits>;
  using led_cal_type = led_output_rw_type;
} // namespace cal

#endif // CALM_TYPE_PRJ_H_
