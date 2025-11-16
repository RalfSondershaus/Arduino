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
#include <Prj_Types.h>
#include <Cfg_Prj.h>
#include <Cal/CalM_Types.h>
#include <Cal/CalM_config.h>
#include <Hal/Gpio.h>
#include <Util/Array.h>

namespace cal
{

    class CalM
    {
    public:
        /**
         * @brief EEPROM data buffer
         * 
         * Holds all calibration data read from or to be written to EEPROM.
         */
        util::array<uint8, eeprom::kSizeOfData> eeprom_data_buffer;

        /**
         * @brief Built-in signal outputs
         */
        static const uint8 ROM_CONST_VAR built_in_signal_outputs[cal::cv::kSignalLength * cfg::kNrBuiltInSignals];

        /**
         * @brief GPIO configuration
         * 
         * Each pin's mode is set using the Arduino pinMode function.
         */
        static hal::GpioConfig gpio_cfg;
        
    protected:

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

        /**
         * @brief Initialize calibration manager
         * 
         *  Init runable called once at system startup.
         */
        void init();

        /**
         * @brief Runable 100 ms
         * 
         * Cycle function called every 100 ms.
         */
        void cycle100();

        /**
         * @brief configure output and input pins according to calibration data
         */
        void configure_pins();

        /**
         * @brief Check if a CV is valid
         * 
         * @param cv_id CV ID
         * @return true CV ID is valid
         * @return false CV ID is not valid
         */
        bool is_cv_id_valid(uint16 cv_id)
        {
            return eeprom_data_buffer.check_boundary(cv_id);
        }

        /**
         * @brief Get a CV
         *
         * @param cv_id [in] CV ID
         * @return uint8 CV value
         */
        uint8 get_cv(uint16 cv_id)
        {
            if (is_cv_id_valid(cv_id))
            {
                return eeprom_data_buffer[cv_id];
            }
            else
            {
                return 0;
            }
        }

        /**
         * @brief Set a CV if CV ID is valid.
         * 
         * @param cv_id [in] CV ID
         * @param val [in] CV value
         */
        void set_cv(uint16 cv_id, uint8 val)
        {
            if (is_cv_id_valid(cv_id))
            {
                eeprom_data_buffer[cv_id] = val;
                if (cv_id >= cv::kSignalInputBase &&
                    cv_id < cv::kSignalInputBase + cfg::kNrSignals)
                {
                    // reconfigure pins if input CV changed
                    configure_pins();
                }
                else if (cv_id >= cv::kSignalFirstOutputBase &&
                    cv_id < cv::kSignalFirstOutputBase + cfg::kNrSignals)
                {
                    // reconfigure pins if input CV changed
                    configure_pins();
                }
                else
                {
                    // no action required
                }
                // save to EEPROM
                update(cv_id);
            }
        }

        /**
         * @defgroup EEPROM access
         * @{
         */
        /**
         * @brief Read all configurations from EEPROM and compare CV 9 (manufacturer ID) against initial
         * value (default EEPORM value if never written before).
         *
         * @return true CV 9 has been written
         * @return false CV 9 has not been written
         */
        bool read_all();
        /**
         * @brief Store all configurations to EEPROM.
         *
         * @return true CV 9 has been written successfully
         * @return false CV 9 has not been written
         */
        bool write_all();
        /**
         * @brief Initialize configuration with ROM default values and write to EEPROM.
         *
         * @return true CV 9 has been written successfully
         * @return false CV 9 has not been written
         */
        bool set_defaults();
        /**
         * @brief Save data to EEPROM if a value differs from the value already stored in the EEPROM.
         *
         * Validate the data after write.
         *
         * @return true Validation successful
         * @return false Validation failed
         */
        bool update();
        /**
         * @brief Save a CV to EEPROM if a value differs from the value already stored in the EEPROM.
         *
         * @return true Validation successful
         * @return false Validation failed
         */
        void update(uint16 cv_id);
        /** @} */

        /**
         * @defgroup Signal helpers
         * @{
         */
        struct signal_aspect
        {
            uint8 num_targets;                                ///< Number of outputs (LEDs)
            uint8 aspect; ///< A bit per output, max. 8 outputs: 0 = 0%, 1 = 100%, LSB = 1st output, MSB = 8th output
            uint8 blink;  ///< A bit per output, max. 8 outputs: 0 = no blinking, 1 = blinking
            uint8 change_over_time_10ms;                     ///< [10 ms] dim time if aspect changes
            uint8 change_over_time_blink_10ms;               ///< [10 ms] dim time for blinking effects
        };

        /**
         * @brief Get the signal id which selects the change over time and aspects to be used
         *
         * @param signal_idx Signal index in the array of signals (0 ... cfg::kNrSignals-1)
         * @return uint8 Signal id (eSignalNotUsed, eFirstBuiltInSignalId, ..., eFirstUserDefinedSignalID, ...)
         */
        uint8 get_signal_id(uint8 signal_idx)
        {
            const uint16 idx = (signal_idx < cfg::kNrSignals) ? signal_idx : 0;
            return get_cv(cal::cv::kSignalIDBase + idx);
        }

        /**
         * @brief Check if the signal id is a valid user-defined signal id
         * 
         * @param signal_id Signal id to be checked
         * @return true signal id is a valid user-defined signal id
         * @return false signal id is not a valid user-defined signal id
         */
        bool is_built_in(uint8 signal_id) noexcept 
        { 
            return (signal_id >= cal::constants::kFirstBuiltInSignalID) &&
                (signal_id < cal::constants::kFirstBuiltInSignalID + cfg::kNrBuiltInSignals);
        }

        /**
         * @brief Check if the signal id is a valid built-in signal id
         * 
         * @param signal_id Signal id to be checked
         * @return true signal id is a valid built-in signal id
         * @return false signal id is not a valid built-in signal id
         */
        bool is_user_defined(uint8 signal_id) noexcept 
        { 
            return (signal_id >= cal::constants::kFirstUserDefinedSignalID) &&
                (signal_id < cal::constants::kFirstUserDefinedSignalID + cfg::kNrUserDefinedSignals);
        }
        /**
         * @brief Returns the zero based index of signal id to be used for array indexing
         * 
         * @param id signal id
         * @return uint8 zero based index of the signal id
         * 
         * @note Use for built in signal ids only.
         */
        static inline uint8 zero_based_built_in(uint8 signal_id)  { return signal_id - cal::constants::kFirstBuiltInSignalID; }

        /**
         * @brief Returns the zero based index of signal id to be used for array indexing
         * 
         * @param id signal id
         * @return uint8 uint8 zero based index of the signal id
         * 
         * @note Use for built in signal ids only.
         */
        static inline uint8 zero_based_user_defined(uint8 signal_id) { return signal_id - cal::constants::kFirstUserDefinedSignalID; }

        /** @brief Get the signal aspect for a user-defined signal ID
         * 
         * @param signal_id Signal id (user-defined)
         * @param cmd Command index (0 ... cfg::kNrSignalAspects-1)
         * @param aspect Output: signal aspect configuration
         */
        void get_signal_aspect(uint8 signal_id, uint8 cmd, signal::signal_aspect& aspect);

        /**
         * @brief Get the number of outputs for the signal
         * @param signal_id Signal id
         * @return uint8 Number of outputs
         * 
         * @note Returns 0 if signal id is invalid
         */
        uint8 get_number_of_outputs(uint8 signal_id)
        {
            uint8 num_targets;
            if (is_user_defined(signal_id))
            {
                uint16 index = zero_based_user_defined(signal_id);
                index = cal::cv::kUserDefinedSignalBase + index * cal::cv::kSignalLength;
                num_targets = util::bits::masked_shift(
                    get_cv(index),
                    cal::constants::bitmask::kNumberOfOutputs,
                    cal::constants::bitshift::kNumberOfOutputs);
            }
            else if (is_built_in(signal_id))
            {
                uint16 index = zero_based_built_in(signal_id);
                num_targets = util::bits::masked_shift(
                    ROM_READ_BYTE(&built_in_signal_outputs[index]),
                    cal::constants::bitmask::kNumberOfOutputs,
                    cal::constants::bitshift::kNumberOfOutputs);
            }
            else
            {
                num_targets = 0U;
            }
            return num_targets;
        }

        /**
         * @brief Get the input configuration for the signal
         * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
         * @return uint8 CV value for input configuration
         */
        signal::input_cal get_input(uint8 signal_idx)
        {
            const uint8 cv_value = get_cv(cal::cv::kSignalInputBase + signal_idx);
            struct signal::input_cal input;
            input.type = util::bits::masked_shift(
                            cv_value,
                            cal::constants::bitmask::kInputType,
                            cal::constants::bitshift::kInputType);
            input.pin = util::bits::masked_shift(
                            cv_value,
                            cal::constants::bitmask::kAdcPin,
                            cal::constants::bitshift::kAdcPin);
            return input;
        }
        /**
         * @brief Get the input configuration for the signal
         * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
         * @return uint8 CV value for input configuration
         */
        inline struct signal::target get_first_output(uint8 signal_idx)
        {
            struct signal::target output = get_cv(cal::cv::kSignalFirstOutputBase + signal_idx);
            return output;
        }
        /** @} */

        /**
         * @defgroup General helpers
         * @{
         */
        inline bool is_output_pin(uint8 pin)
        {
            return (gpio_cfg.pin_modes.check_boundary(pin) && (gpio_cfg.pin_modes[pin] == OUTPUT));
        }
        /** @} */
    };

} // namespace cal

#endif // CAL_H_
