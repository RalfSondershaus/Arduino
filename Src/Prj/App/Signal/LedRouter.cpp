/**
 * @file Signal/LedRouter.cpp
 *
 * @brief Routes classified values (ifc_classified_values) to target values.
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

#include <Rte/Rte.h>
#include <Util/Algorithm.h>
#include <LedRouter.h>
#include <Hal/Serial.h>

namespace signal
{
  // --------------------------------------------------------------------------------------------
  /// Gamma correction
  /// 
  /// Intensity [0 ... 255] to PWM [0 ... 255]
  // --------------------------------------------------------------------------------------------
  static const LedRouter::intensity8_255_type::base_type aunIntensity2Pwm[256u] =
  {
    /*          0    1    2    3    4    5    6    7    8    9  */
    /*   0 */   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,
    /*  10 */   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,
    /*  20 */   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    /*  30 */   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    /*  40 */   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    /*  50 */   3,   3,   3,   3,   3,   3,   3,   3,   4,   4,
    /*  60 */   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    /*  70 */   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,
    /*  80 */   6,   6,   6,   6,   6,   6,   6,   7,   7,   7,
    /*  90 */   7,   7,   7,   8,   8,   8,   8,   8,   8,   9,
    /* 100 */   9,   9,   9,   9,  10,  10,  10,  10,  10,  11,
    /* 110 */  11,  11,  11,  12,  12,  12,  12,  13,  13,  13,
    /* 120 */  14,  14,  14,  14,  15,  15,  15,  16,  16,  16,
    /* 130 */  17,  17,  18,  18,  18,  19,  19,  20,  20,  21,
    /* 140 */  21,  21,  22,  22,  23,  23,  24,  24,  25,  25,
    /* 150 */  26,  27,  27,  28,  28,  29,  30,  30,  31,  32,
    /* 160 */  32,  33,  34,  35,  35,  36,  37,  38,  39,  39,
    /* 170 */  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
    /* 180 */  50,  51,  52,  53,  55,  56,  57,  58,  59,  61,
    /* 190 */  62,  63,  65,  66,  68,  69,  71,  72,  74,  76,
    /* 200 */  77,  79,  81,  82,  84,  86,  88,  90,  92,  94,
    /* 210 */  96,  98, 100, 102, 105, 107, 109, 112, 114, 117,
    /* 220 */ 119, 122, 124, 127, 130, 133, 136, 139, 142, 145,
    /* 230 */ 148, 151, 155, 158, 162, 165, 169, 172, 176, 180,
    /* 240 */ 184, 188, 192, 196, 201, 205, 210, 214, 219, 224,
    /* 250 */ 229, 234, 239, 244, 250, 255
  };

  // -----------------------------------------------------------------------------------
  /// Calculate dim ramps
  // -----------------------------------------------------------------------------------
  void LedRouter::doRamps()
  {
    rte::Ifc_OnboardTargetDutyCycles::size_type pos = static_cast<rte::Ifc_OnboardTargetDutyCycles::size_type>(0);

    for (auto it = ramps_onboard.begin(); it != ramps_onboard.end(); it++)
    {
      const intensity16_type intensity16{ it->step() };
      const intensity8_255_type intensity{ rte::convert<intensity8_255_type, intensity16_type>(intensity16) };
      const intensity8_255_type pwm{ aunIntensity2Pwm[intensity] };
      rte::ifc_onboard_target_duty_cycles::writeElement(pos, pwm);
      pos++;
    }
  }

  // -----------------------------------------------------------------------------------
  /// Server function: 
  /// - Initialize ramp for an output port with given intensity and slope / speed.
  /// - Step size of dim ramp is re-calculated.
  /// 
  /// @param tgt Output port
  /// @param intensity [0x0000 - 0x8000] Target intensity with 0x0000 = 0%, 0x8000 = 100%
  /// @param slope [(0x0000 - 0x8000)/ms] Slope / speed
  // -----------------------------------------------------------------------------------
  LedRouter::ret_type LedRouter::setIntensityAndSpeed(const target_type tgt, const intensity16_type intensity, const speed16_ms_type slope)
  {
    switch (tgt.type)
    {
    case target_type::eOnboard:
    {
      if (ramps_onboard.check_boundary(tgt.idx))
      {
        ramps_onboard[tgt.idx].init_from_slope(intensity, slope, kCycleTime);
      }
    }
    break;
    case target_type::eExternal:
      break;
    default:
      break;
    }

    return rte::ifc_base::OK;
  }

  // -----------------------------------------------------------------------------------
  /// Server function: Set the slope / speed to reach the target intensity but do not change the target intensity.
  /// 
  /// @param tgt Output port
  /// @param slope [(0x0000 - 0x8000)/ms] Slope / speed
  // -----------------------------------------------------------------------------------
  LedRouter::ret_type LedRouter::setSpeed(const target_type tgt, const speed16_ms_type slope)
  {
    switch (tgt.type)
    {
    case target_type::eOnboard:
    {
      if (ramps_onboard.check_boundary(tgt.idx))
      {
        ramps_onboard[tgt.idx].set_slope(slope, kCycleTime);
      }
    }
    break;
    case target_type::eExternal:
      break;
    default:
      break;
    }
    return rte::ifc_base::OK;
  }

  // -----------------------------------------------------------------------------------
  /// Server function: Set the target intensity but do not change the current speed.
  /// 
  /// @param tgt Output port
  /// @param intensity [0x0000 - 0x8000] Target intensity with 0x0000 = 0%, 0x8000 = 100%
  // -----------------------------------------------------------------------------------
  LedRouter::ret_type LedRouter::setIntensity(const target_type tgt, const intensity16_type intensity)
  {
    switch (tgt.type)
    {
    case target_type::eOnboard:
    {
      if (ramps_onboard.check_boundary(tgt.idx))
      {
        ramps_onboard[tgt.idx].set_tgt(intensity);
      }
    }
    break;
    case target_type::eExternal:
      break;
    default:
      break;
    }
    return rte::ifc_base::OK; 
  }

  // -----------------------------------------------------------------------------------
  /// Init at system start
  // -----------------------------------------------------------------------------------
  void LedRouter::init()
  {
    for (auto it = ramps_onboard.begin(); it != ramps_onboard.end(); it++)
    {
      it->clear();
    }
    for (auto it = ramps_external.begin(); it != ramps_external.end(); it++)
    {
      it->clear();
    }
  }

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void LedRouter::cycle()
  {
    doRamps();
  }
} // namespace signal

