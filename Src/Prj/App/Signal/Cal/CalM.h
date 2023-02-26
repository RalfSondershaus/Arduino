/**
 * @file CalM.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Access to calibration parameters
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

#ifndef CAL_H_
#define CAL_H_

#include <Std_Types.h>
#include <Cal/CalM_Type.h>

namespace cal
{

  class CalM
  {
  public:
    /// coding data for signals
    signal_cal_type signals;
    input_classifier_cal_type input_classifiers;
    led_cal_type leds;

  protected:
    void calcLeds();

  public:
    CalM();

    /// Init runable
    void init();
    /// Runable 100 ms
    void cycle100();

    /// references to coding parameters
    const signal_cal_type *           get_signal()             { return &signals; }
    const input_classifier_cal_type * get_input_classifiers()  { return &input_classifiers; }
    const led_cal_type *              get_leds()               { return &leds; }
  };

} // namespace cal

#endif // CAL_H_
