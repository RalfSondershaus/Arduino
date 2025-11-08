/**
 * @file Signal/Cal/CalM.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief  
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
#include <Hal/EEPROM.h>
#include <Hal/Gpio.h>
#include <Rte/Rte.h>

namespace cal
{
    /// CV Name                                     CV#    CV#       Required  Default  Read
    ///                                                    optional            Value    Only
    /// Decoder Address LSB                         1      513       M         1        Y     LSB of accessory decoder address
    /// Auxiliary Activation                        2      514       O                        Auxiliary activation of outputs
    /// Time On F1                                  3      515       O
    /// Time On F2                                  4      516       O
    /// Time On F3                                  5      517       O
    /// Time On F4                                  6      518       O
    /// Manufacturer Version Info                   7      519       M
    /// ManufacturerID                              8      520       M                  Y     Values assigned by NMRA
    /// Decoder Address MSB                         9      521       M         0        Y     3 MSB of accessory decoder address
    /// Bi-Directional Communication Configuration 28      540       O
    /// Accessory Decoder Configuration            29      541       M                        similar to CV#29; for acc. decoders
    /// Indexed Area Pointers                      31, 32                                     Index High and Low Address
    /// Manufacturer Unique                        112-128 (17 bytes)
    /// Manufacturer Unique                        129-256 (128 bytes)
    /// Manufacturer Unique                        513-895 (383 bytes)

    // 129        Checksum

    namespace eeprom
    {
        namespace default_values
        {
            static const uint8_t ROM_CONST_VAR init_values[] = EEPROM_INIT;
        }
    }

    /**
     * @brief Returns the byte-wise sum of all bytes modulo 256.
     * 
     * @param src Pointer to first memory location
     * @param unLen Length in bytes for sum
     * @param ucCrc Starting value
     * @return uint8 Sum over all bytes modulo 256.
     */
    uint8 calc_sum(const void *src, uint16 unLen, uint8 ucCrc)
    {
        const uint8 *pSrc = static_cast<const uint8 *>(src);

        for (; unLen > 0U; unLen--)
        {
            ucCrc += *pSrc;
            pSrc++;
        }

        return ucCrc;
    }

    /**
     * @brief Construct a new CalM object
     */
    CalM::CalM()
    {
    }

    /**
     * @brief configure output and input pins according to calibration data
     */
    void CalM::configure_pins()
    {
        // input pins
        for (uint16 cv_id = cal::cv::kSignalInputBase; cv_id < cfg::kNrSignals; cv_id++)
        {
            uint8 cv_value = get_cv(cv_id);
            if (cal::signal::extract_signal_input_type(cv_value) == cal::signal::kAdc)
            {
                uint8 adc_pin = cal::signal::extract_signal_input_pin(cv_value);
                hal::pinMode(adc_pin, INPUT);
            }
        }
        // output pins
        for (uint16 cv_id = cal::cv::kSignalFirstOutputBase; cv_id < cfg::kNrSignals; cv_id++)
        {
            uint8 cv_value = get_cv(cv_id);
            if (cal::signal::extract_signal_first_output_type(cv_value) == cal::signal::kOnboard)
            {
                uint8 adc_pin = cal::signal::extract_signal_first_output_pin(cv_value);
                hal::pinMode(adc_pin, OUTPUT);
            }
        }
    }

    /**
     * @brief Read CV 9 (manufacturer ID) and compare against initial value (default EEPORM value
     *  if never written before).
     * @return true ManufacturerID in EEPROM is valid
     * @return false ManufacturerID in EEPROM is invalid
     */
    bool CalM::is_valid()
    {
        return hal::eeprom::read(eeprom::kManufacturerID) != hal::eeprom::kInitial;
    }

    /**
     * @brief Calculate checksum over calibration data
     * @return uint8 The checksum.
     */
    uint8 CalM::calc_checksum()
    {
        uint8 ucCrc = 0; // start value

        ucCrc = calc_sum(eeprom_data_buffer.data(), eeprom_data_buffer.size(), ucCrc);

        return ucCrc;
    }

    /**
     * @brief Read all configurations from EEPROM and compare CV 9 (manufacturer ID) against initial
     * value (default EEPORM value if never written before).
     * 
     * @note call @ref configure_pins() afterwards to setup pins according to calibration data
     * 
     * @return @ref is_valid()
     */
    bool CalM::read_all()
    {
        for (size_t i = 0; i < eeprom_data_buffer.size(); i++)
        {
            eeprom_data_buffer[i] = hal::eeprom::read(static_cast<int>(i));
        }
        return is_valid();
    }

    /**
     * @brief Store all configurations to EEPROM.
     * 
     * @return @ref is_valid()
     */
    bool CalM::write_all()
    {
        for (size_t i = 0; i < eeprom_data_buffer.size(); i++)
        {
            hal::eeprom::write(static_cast<int>(i), eeprom_data_buffer[i]);
        }
        return is_valid();
    }

    /**
     * @brief Initialize configuration with ROM default values and write to EEPROM.
     * 
     * @return @ref is_valid()
     */
    bool CalM::set_defaults()
    {
        // copy default values to eeprom_data_buffer
        for (size_t i = 0; i < eeprom_data_buffer.size(); i++)
        {
            eeprom_data_buffer[i] = ROM_READ_BYTE(&eeprom::default_values::init_values[i]);
        }

        configure_pins();

        // write to EEPROM
        return write_all();
    }

    /**
     * @brief Initialize the calibration manager
     * 
     * Init runable called once at system startup.
     * 
     * Pin configuration according to calibration data is done here.
     * 
     * @note If EEPROM is invalid (never programmed), it is initialized with default values.
     */
    void CalM::init()
    {
        if (!read_all())
        {
            // invalid / never programmed: initialize EEPROM with default values
            set_defaults();
            read_all();
        }
        configure_pins();
    }

    /**
     * @brief Runable 100 ms
     * 
     * Cycle function called every 100 ms.
     * 
     * @note currently empty
     */
    void CalM::cycle100()
    {
    }

} // namespace cal
