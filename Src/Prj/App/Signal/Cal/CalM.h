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
#include <Rte/Rte_Type.h>

namespace cal
{

  class CalM
  {
  public:
    /// coding data for signals
    signal_cal_type signals;
    input_classifier_cal_type input_classifiers;
    led_cal_type leds;
    base_cv_cal_type base_cv;

  protected:
    /// Calc data for leds from signals
    void calcLeds();

    /// Initialize internal data structures and EEPROM values with default values
    /// from ROM.    
    void initAll();

    /// Load values from EEPROM
    bool readAll();
    void readSignals();
    void readClassifiers();
    void readBaseCV();

    /// Save data to EEPROM if a value differs from the value already stored in the EEPROM
    bool updateSignals();
    bool updateClassifiers();
    bool updateBaseCV();

    /// Initialize internal data structures and EEPROM values with default values
    /// from ROM.
    void initSignals();
    void initClassifiers();
    void initBaseCV();

    /// Returns true if the EEPROM coding data are valid.
    /// That is, if 
    /// - eeprom::eManufacturerID is not EEPROM initial value (FF)
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
    const signal_cal_type *           get_signal()             { return &signals; }
    const input_classifier_cal_type * get_input_classifiers()  { return &input_classifiers; }
    const led_cal_type *              get_leds()               { return &leds; }
    const base_cv_cal_type *          get_base_cv()            { return &base_cv; }

    /// Server function: Store data in RAM. If doUpdate is true, stores data
    /// in EEPROM. Otherwise, call update() for this.
    /// Return OK is successful, returns NOK otherwise.
    rte::ret_type set_signal(uint8 ucSignalId, const signal_type& values, bool doUpdate = false);
    rte::ret_type set_input_classifier(uint8 ucClassifierId, const input_classifier_single_type& values, bool doUpdate = false);
    rte::ret_type set_base_cv(const base_cv_cal_type& p, bool doUpdate = false);
    
    /// Save data to EEPROM if a value differs from the value already stored in the EEPROM.
    /// Validate the data after write.
    /// Returns true if successful, returns false otherwise.
    bool update();
  };

} // namespace cal

#endif // CAL_H_
