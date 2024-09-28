/**
 * @file CalM.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Access to calibration parameters for project signal
 *
 * @copyright Copyright 2022 - 2024 Ralf Sondershaus
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
    dcc_cal_type dcc;

    bool valid;

  protected:
    /// Calc data for leds from signals
    void calcLeds();

    /// Load values from EEPROM
    bool readAll();
    void readSignals();
    void readClassifiers();

    /// Save data to EEPROM if a value differs from the value already stored in the EEPROM
    bool updateSignals();
    bool updateClassifiers();

    /// Returns true if the EEPROM coding data are valid
    bool isValid();

    /// Returns checksum for all calibration data
    uint8 calcChecksum();
  public:
    CalM();

    /// Init runable
    void init();
    /// Runable 100 ms
    void cycle100();

    /// references to coding parameters
    const signal_cal_type *           get_signal()             { return (valid) ? (&signals) : (nullptr); }
    const input_classifier_cal_type * get_input_classifiers()  { return (valid) ? (&input_classifiers) : (nullptr); }
    const led_cal_type *              get_leds()               { return (valid) ? (&leds) : (nullptr); }
    const dcc_cal_type *              get_dcc()                { return (valid) ? (&dcc) : (nullptr); }

    /// Save data to RAM. Doesn't not store data in EEPROM (call update() for this).
    /// Return true is successful, returns false otherwise.
    bool set_signal(uint8 ucSignalId, const signal_type& values);
    bool set_classifier(uint8 ucClassifierId, const input_classifier_single_type& values);

    /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
    /// Validate the data after write.
    /// Returns true if successful, returns false otherwise.
    bool update();
  };

} // namespace cal

#endif // CAL_H_
