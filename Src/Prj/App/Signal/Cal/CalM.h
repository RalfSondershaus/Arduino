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
#include <Cfg_Prj.h>
#include <Cal/CalM_Type.h>
#include <Rte/Rte_Type.h>
#include <Util/Array.h>

namespace cal
{

  class CalM
  {
  public:
    /// @brief User-defined coding data for signals
    util::array<output_type, cfg::kNrUserDefinedSignals> user_defined_signal_outputs;
    /// @brief User-defined classifiers
    util::array<classifier_type, cfg::kNrUserDefinedClassifierTypes> user_defined_classifier_types;

    /**
     * @defgroup Type safety
     * @{
     * Use these types for type safety.
     */
    struct Pin
    {
        uint8 val;
    };
    struct SignalId
    {
        uint8 val;
    };
    /** @} */

    /// @brief An invalid classifier type. A valid type is any of 0 ... cfg::kNrUserDefinedClassifierTypes
    static constexpr uint8 kInvalidClassifierType = 255;

    static constexpr Pin kInvalidPin = Pin { cal::kInvalidPin };

    /// coding data for signals
    signal_cal_type signals;
    classifier_array_cal_type classifier_array;
    led_cal_type leds;
    base_cv_cal_type base_cv;

  protected:
    /// Calc data for leds from signals
    void calc_leds();

    bool is_user_defined(SignalId signal_id) const noexcept 
    { 
        return (signal_id.val >= kFirstUserDefinedSignalID) && 
               (signal_id.val <  user_defined_signal_outputs.max_size());
    }
    util::ptr<const cal::output_type> getBuiltInSignal(SignalId signal_id);
    util::ptr<const cal::output_type> getUserDefinedSignal(SignalId signal_id);

    uint8 find_classifier_type(util::ptr<const classifier_type> limits_ptr);

    /**
     * @defgroup Helper functions to update configuration.
     * @{
     */
    void set_first_target(cal::signal_type& sig_type, uint8 raw_val);
    void set_output(signal_type& sig_type, CalM::SignalId signal_id);
    /**
     * @note Digital inputs are not supported yet.
     */
    void set_input_and_classifier(
        uint8 signal_pos,
        classifier_array_element_type& classifier_array_element,
        signal_type& sig_type,
        uint8 signal_input_raw_val, 
        uint8 classifier_type_raw_val);
    /** @} */

    /**
     * @defgroup EEPROM access
     * @{
     */
    /** Read data from EEPROM */
    bool read_all();
    void read_signals();
    void read_classifiers();
    void read_base_CV();

    /** Save data to EEPROM if a value differs from the value already stored in the EEPROM */
    void update_signals();
    void update_user_defined_classifiers();
    void update_base_CV();
    /** @} */

    /**
     * @defgroup Initializer from ROM
     * @{
     * Initialize data structures and EEPROM with default values from ROM
     */
    void init_signals();
    void init_user_defined_classifiers();
    void init_base_CV();
    /** @} */

    /**
     * @brief Returns true if eeprom::eManufacturerID is not EEPROM initial value (FF)
     * 
     * @return true ManufacturerID in EEPROM is valid
     * @return false ManufacturerID in EEPROM is invalid
     */
    bool is_valid();

    /**
     * @brief Returns a checksum for calibration data
     * 
     * @return uint8 The checksum.
     */
    uint8 calc_checksum();
  public:
    CalM();

    /// Init runable
    void init();
    /// Runable 100 ms
    void cycle100();

    /**
     * @defgroup Server functions
     * @{
     */
    /** @brief Read access to coding parameters */ 
    const signal_cal_type *           get_signal()             { return &signals; }
    const classifier_array_cal_type * get_classifiers_array()  { return &classifier_array; }
    const led_cal_type *              get_leds()               { return &leds; }
    const base_cv_cal_type *          get_base_cv()            { return &base_cv; }

    /**
     * @brief Set the first target of signal @ref signal_pos.
     * 
     * @param signal_pos Index of the signal [0 ... cfg::kNrSignals). 
     * @param first_target The first target (output pin number and type - internal/external)
     * @param do_update If true, values are stored in EEPROM
     * @return rte::ret_type E_OK Values are update successfully.
     * @return rte::ret_type E_NOK Index out of bounds.
     * 
     * @note If do_update is false, EEPROM is not updated. You can use @ref update() to store
     *       data in EEPROM later.
     */
    rte::ret_type set_signal_first_target(
        uint8 signal_pos, 
        const target_type& first_target, 
        bool do_update = false);

    /**
     * @brief Set the signal id of signal @ref signal_pos.
     * 
     * @param signal_pos Index of the signal [0 ... cfg::kNrSignals). 
     * @param signal_id The signal ID that shall be used (1 ... 127: built-in, 128 ... 255: user defined)
     * @param do_update If true, values are stored in EEPROM
     * @return rte::ret_type E_OK Values are update successfully.
     * @return rte::ret_type E_NOK Index out of bounds.
     * 
     * @note If do_update is false, EEPROM is not updated. You can use @ref update() to store
     *       data in EEPROM later.
     */
    rte::ret_type set_signal_id(uint8 signal_pos, const uint8 signal_id, bool do_update = false);

    /**
     * @brief Server function: Configure signal @ref signal_pos to use an ADC pin and a
     *        classifier as input. If do_update is true, data is stored in EEPROM.
     * 
     * @param signal_pos Index of the signal [0 ... cfg::kNrSignals). It is the position
     *                   of the classified values on RTE.
     * @param adc_pin ADC pin number
     * @param classifier_type The classifier type.
     * @param do_update If true, values are stored in EEPROM.
     * @return rte::ret_type E_OK Values are update successfully.
     * @return rte::ret_type E_NOK Index out of bounds.
     * 
     * @note If do_update is false, EEPROM is not updated. You can use @ref update() to store
     *       data in EEPROM later.
     */
    rte::ret_type set_signal_input_classifier(uint8 signal_pos, 
                                              const uint8 adc_pin, 
                                              const uint8 classifier_type_val,
                                              bool do_update = false);

    /**
     * @brief Server function: Configure signal @ref signal_pos to use a DCC commands as input. 
     *        If do_update is true, data is stored in EEPROM.
     * 
     * @param signal_pos Index of the signal [0 ... cfg::kNrSignals).
     * @param do_update If true, values are stored in EEPROM.
     * @return rte::ret_type E_OK Values are update successfully.
     * @return rte::ret_type E_NOK Index out of bounds.
     * 
     * @note If do_update is false, EEPROM is not updated. You can use @ref update() to store
     *       data in EEPROM later.
     */
    rte::ret_type set_signal_input_dcc(uint8 signal_pos, 
                                       bool do_update = false);
    /**
    * @brief Set the base CVs
    * 
    * @param new_base_cv The base CVs
    * @param do_update If true, values are stored in EEPROM.
    * @return rte::ret_type OK success.
    */
    rte::ret_type set_base_cv(const base_cv_cal_type& new_base_cv, bool do_update = false);
    
    /**
     * @brief Set a CV
     * 
     * @param cv_new 
     * @return rte::ret_type OK CV value was updated in EEPROM successfully. Internal structures 
     *         were updated.
     * @return rte::ret_type NOK CV with invalid id.
     */
    //rte::ret_type set_cv(CV cv_new) { return set_cv(cv_new.id, cv_new.val); }

    /**
     * @brief Set a CV
     * 
     * @param cv_id CV ID
     * @param val The new value for the CV
     * @return rte::ret_type OK CV value was updated in EEPROM successfully. Internal structures 
     *         were updated.
     * @return rte::ret_type NOK CV with invalid id.
     */
    rte::ret_type set_cv(uint16 cv_id, uint8 val);

    /**
     * @brief initialize EEPROM with ROM default values
     * 
     * @return rte::ret_type OK success.
     * @return rte::ret_type NOK Data validation ok (written manufacturer ID invalid)
     */
    rte::ret_type set_defaults();
    
    /**
     * @brief Save data to EEPROM if a value differs from the value already stored in the EEPROM.
     * 
     * Validate the data after write.
     * 
     * @return true Validation successful
     * @return false Validation failed
     */
    bool update();
    /** @} */
  };

} // namespace cal

#endif // CAL_H_
