/**
 * @file CalM.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief  Access to calibration parameters
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

#include <Cal/CalM.h>
#include <Rte/Rte.h>
#include <Arduino.h>

using classifier_type = cal::input_classifier_type::classifier_type;

static constexpr uint8  kAdDebounce = 5; ///< [10 ms] Debounce time
static constexpr uint16 kGreenLo = (605 - 10);
static constexpr uint16 kGreenHi = (605 + 10);
static constexpr uint16 kYellowLo = (399 - 10);
static constexpr uint16 kYellowHi = (399 + 10);
static constexpr uint16 kRedLo = (192 - 10);
static constexpr uint16 kRedHi = (192 + 10);
static constexpr uint16 kWhiteLo = (147 - 10);
static constexpr uint16 kWhiteHi = (147 + 10);
static constexpr uint16 kAdMin = 0;
static constexpr uint16 kAdMax = 1023;

static constexpr uint8 kClassified = cal::input_type::kClassified;
static constexpr uint8 kOnboard = cal::target_type::kOnboard;
static constexpr uint8 kNone = cal::target_type::kNone;

#define CAL_SIGNAL_ARRAY \
  { { \
                      /* HP0                        HP1                         HP2                         HP3                         ALL */ \
                      /* red1                       red2                        green                       yellow                      white */ \
    { /* 0 */  \
      /* input */ { kClassified, 0 }, \
      /* aspects */ { { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } } }, \
      /* targets */ { { { kOnboard, 13 }, { kOnboard, 12 }, { kOnboard, 11 }, { kOnboard, 10 }, { kOnboard, 9 } } }, \
      /* dim times */ 0, 0 \
    }, { /* 1 */  \
      /* input */ { kClassified, 0 }, \
      /* aspects */ { { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } } }, \
      /* targets */ { { { kOnboard, 13 }, { kOnboard, 12 }, { kOnboard, 11 }, { kOnboard, 10 }, { kOnboard, 9 } } }, \
      /* dim times */ 0, 0 \
    }, { /* 2 */  \
      /* input */ { kClassified, 0 }, \
      /* aspects */ { { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } } }, \
      /* targets */ { { { kOnboard, 13 }, { kOnboard, 12 }, { kOnboard, 11 }, { kOnboard, 10 }, { kOnboard, 9 } } }, \
      /* dim times */ 0, 0 \
    }, { /* 3 */  \
      /* input */ { kClassified, 0 }, \
      /* aspects */ { { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } } }, \
      /* targets */ { { { kOnboard, 13 }, { kOnboard, 12 }, { kOnboard, 11 }, { kOnboard, 10 }, { kOnboard, 9 } } }, \
      /* dim times */ 0, 0 \
    }, { /* 4 */  \
      /* input */ { kClassified, 0 }, \
      /* aspects */ { { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } } }, \
      /* targets */ { { { kOnboard, 13 }, { kOnboard, 12 }, { kOnboard, 11 }, { kOnboard, 10 }, { kOnboard, 9 } } }, \
      /* dim times */ 0, 0 \
    }, { /* 5 */  \
      /* input */ { kClassified, 0 }, \
      /* aspects */ { { { 0b00011000, 0b00000000 }, { 0b00000100, 0b00000000 }, { 0b00000110, 0b00000000 }, { 0b00011001, 0b00000000 }, { 0b00011111, 0b00000000 } } }, \
      /* targets */ { { { kOnboard, 13 }, { kOnboard, 12 }, { kOnboard, 11 }, { kOnboard, 10 }, { kOnboard, 9 } } }, \
      /* dim times */ 0, 0 \
    }, \
  } }

#define V2P(v) classifier_type::convertInput(v)

#define CAL_INPUT_CLASSIFIER_CFG \
  { /* struct input_classifier_cfg */ \
    { /* struct util::array */ \
      { /* array */ \
          { A0 \
            , { /* struct classifier_limits */ \
              kAdDebounce \
              , { /* struct util::array */ { V2P(kGreenLo), V2P(kYellowLo), V2P(kRedLo), V2P(kWhiteLo), V2P(kAdMax) } } /* Lower limit for class x [0, 1023] */ \
              , { /* struct util::array */ { V2P(kGreenHi), V2P(kYellowHi), V2P(kRedHi), V2P(kWhiteHi), V2P(kAdMin) } } /* Upper limit for class x [0, 1023] */ \ 
} \
} \
, { A1 \
    , { /* struct classifier_limits */ \
      kAdDebounce \
      , { /* struct util::array */ { V2P(kGreenLo), V2P(kYellowLo), V2P(kRedLo), V2P(kWhiteLo), V2P(kAdMax) } } /* Lower limit for class x [0, 1023] */ \
      , { /* struct util::array */ { V2P(kGreenHi), V2P(kYellowHi), V2P(kRedHi), V2P(kWhiteHi), V2P(kAdMin) } } /* Upper limit for class x [0, 1023] */ \
      } \
} \
, { A2 \
    , { /* struct classifier_limits */ \
      kAdDebounce \
        , { /* struct util::array */ { V2P(kGreenLo), V2P(kYellowLo), V2P(kRedLo), V2P(kWhiteLo), V2P(kAdMax) } } /* Lower limit for class x [0, 1023] */ \
        , { /* struct util::array */ { V2P(kGreenHi), V2P(kYellowHi), V2P(kRedHi), V2P(kWhiteHi), V2P(kAdMin) } } /* Upper limit for class x [0, 1023] */ \
      } \
} \
, { A3 \
    , { /* struct classifier_limits */ \
      kAdDebounce \
        , { /* struct util::array */ { V2P(kGreenLo), V2P(kYellowLo), V2P(kRedLo), V2P(kWhiteLo), V2P(kAdMax) } } /* Lower limit for class x [0, 1023] */ \
        , { /* struct util::array */ { V2P(kGreenHi), V2P(kYellowHi), V2P(kRedHi), V2P(kWhiteHi), V2P(kAdMin) } } /* Upper limit for class x [0, 1023] */ \
      } \
} \
, { A4 \
    , { /* struct classifier_limits */ \
      kAdDebounce \
        , { /* struct util::array */ { V2P(kGreenLo), V2P(kYellowLo), V2P(kRedLo), V2P(kWhiteLo), V2P(kAdMax) } } /* Lower limit for class x [0, 1023] */ \
        , { /* struct util::array */ { V2P(kGreenHi), V2P(kYellowHi), V2P(kRedHi), V2P(kWhiteHi), V2P(kAdMin) } } /* Upper limit for class x [0, 1023] */ \
      } \
} \
} \
} \
}

namespace cal
{

  signal_array cod_signal_array = CAL_SIGNAL_ARRAY;
  input_classifier_cfg_type cal_input_classifier_cfg = CAL_INPUT_CLASSIFIER_CFG;

  // -----------------------------------------------
  /// Init runable
  // -----------------------------------------------
  void CalM::init()
  {
    rte::ifc_cal_codm_ledr.call(&cod_signal_array);
    rte::ifc_cal_codm_inputclassifier.call(&cal_input_classifier_cfg);
  }

  // -----------------------------------------------
  /// Runable 10 ms
  // -----------------------------------------------
  void CalM::cycle100()
  {
  }

} // namespace cal
