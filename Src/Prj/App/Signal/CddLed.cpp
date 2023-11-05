/**
 * @file Signal/CddLedr.cpp
 *
 * @brief Driver for LEDs, very simple version
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
#include <CddLed.h>
// A CDD can include Arduino.h
#include <Arduino.h>

namespace cdd
{

  // -----------------------------------------------------------------------------------
  /// Write from RTE to pins
  // -----------------------------------------------------------------------------------
  void CddLed::writeOutputs()
  {
    using size_type = rte::Ifc_OnboardTargetDutyCycles::size_type;

    size_type pos;
    rte::intensity8_t u8Int;
    const cal::led_cal_type * pCalLeds = rte::ifc_cal_leds::call();

    if (pCalLeds != nullptr)
    {
      //for (auto it = rte::ifc_onboard_target_intensities::begin(); it != rte::ifc_onboard_target_intensities::end(); it++)
      for (pos = 0U; pos < rte::ifc_onboard_target_duty_cycles::size(); pos++)
      {
        // if the output pin is used by a LED
        if (pCalLeds->test(pos))
        {
          (void)rte::ifc_onboard_target_duty_cycles::readElement(pos, u8Int);
          pinMode(pos, OUTPUT);
          if (digitalPinHasPWM(pos))
          {
            analogWrite(pos, static_cast<int>(u8Int));
          }
          else
          {
            digitalWrite(pos, (u8Int > static_cast<rte::intensity8_t>(0U)) ? HIGH : LOW);
          }
        }
      }
    }
  }

  // -----------------------------------------------------------------------------------
  /// Init at system start
  // -----------------------------------------------------------------------------------
  void CddLed::init()
  {
  }

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void CddLed::cycle()
  {
    writeOutputs();
  }

} // namespace cdd

