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
#include <Cal/CalM_Types.h>
#include <Cal/CalM_config.h>
#include <Util/Array.h>

namespace cal
{

    class CalM
    {
    public:
        /// @brief An invalid classifier type. A valid type is any of 0 ... cfg::kNrUserDefinedClassifierTypes
        static constexpr uint8 kInvalidClassifierType = 255;

        util::array<uint8, eeprom::kSizeOfData> eeprom_data_buffer; ///< EEPROM data buffer

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
        /** @} */
    };

} // namespace cal

#endif // CAL_H_
