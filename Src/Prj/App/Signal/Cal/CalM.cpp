/**
 * @file Signal/Cal/CalM.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief Calibration manager implementation
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
#include <Rte/Rte.h>
#include <Util/Classifier_cfg.h>

namespace cal
{
    /** 
     * Built-in signal outputs 
     * 
     * @note Adjust unit test Ut_Signal/Test.cpp if this definition changes.
     */
    #define CAL_BUILT_IN_SIGNAL_OUTPUTS     \
        {   /* Ausfahrsignal */             \
            /* red red green yellow white */\
            5,                              \
            0b00011000, 0b00000000,         \
            0b00000100, 0b00000000,         \
            0b00000110, 0b00000000,         \
            0b00011001, 0b00000000,         \
            0b00011111, 0b00000000,         \
            0b00011111, 0b00000000,         \
            0b00011111, 0b00000000,         \
            0b00011111, 0b00000000,         \
            10, 10,                         \
            /* Blocksignal */               \
            /* red green */                 \
            2,                              \
            0b00000010, 0b00000000,         \
            0b00000001, 0b00000000,         \
            0b00000011, 0b00000000,         \
            0b00000011, 0b00000000,         \
            0b00000011, 0b00000000,         \
            0b00000011, 0b00000000,         \
            0b00000011, 0b00000000,         \
            0b00000011, 0b00000000,         \
            10, 10,                         \
            /* Einfahrsignal */             \
            /* red red green yellow */      \
            4,                              \
            0b00001100, 0b00000000,         \
            0b00000010, 0b00000000,         \
            0b00000011, 0b00000000,         \
            0b00001111, 0b00000000,         \
            0b00001111, 0b00000000,         \
            0b00001111, 0b00000000,         \
            0b00001111, 0b00000000,         \
            0b00001111, 0b00000000,         \
            10, 10                          \
        }

    /**
     * @brief GPIO configuration
     */
    hal::GpioConfig CalM::gpio_cfg;

    /**
     * @brief Built-in signal outputs
     */
    const uint8 ROM_CONST_VAR CalM::built_in_signal_outputs[cal::cv::kSignalLength * cfg::kNrBuiltInSignals] = CAL_BUILT_IN_SIGNAL_OUTPUTS;

    /** 
     * @brief Default values for calibration data 
     * 
     * @note Defined in CalM_config.h
     * @note Do not use namespace eeprom here to avoid confusion with cal::eeprom since some constants 
     *       have the same name.
     */
    namespace default_values
    {
        static const uint8 ROM_CONST_VAR init_values[] = EEPROM_INIT;
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
        // set all pins to an invalid state
        util::fill(gpio_cfg.pin_modes.begin(), gpio_cfg.pin_modes.end(), 0xFF);

        for (uint8_least sig_idx = 0U; sig_idx < cfg::kNrSignals; sig_idx++)
        {
            // input pin
            const struct signal::input_cal input = get_input(sig_idx);
            if ((input.type == signal::input_cal::kAdc) && 
                 util::classifier_cal::is_pin_valid(input.pin))
            {
                gpio_cfg.pin_modes[input.pin] = INPUT;
            }

            // output pins
            struct signal::target output = get_first_output(sig_idx);
            const sint8 pin_inc = is_output_pin_order_inverse(sig_idx) ? 
                                 -get_output_pin_step_size(sig_idx) : 
                                  get_output_pin_step_size(sig_idx);
            if (output.type == signal::target::kOnboard)
            {
                const uint8 num_outputs = get_number_of_outputs(get_signal_id(sig_idx));
                for (uint8_least pin_idx = 0; pin_idx < num_outputs; pin_idx++)
                {
                    gpio_cfg.pin_modes[output.pin] = OUTPUT;
                    output.pin = static_cast<uint8>(static_cast<sint8>(output.pin) + pin_inc);
                }
            }
        }
        hal::configure_pins(gpio_cfg);
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
     * @note call @ref configure_pins() afterwards to setup pins according to calibration data
     * 
     * @return @ref is_valid()
     */
    bool CalM::set_defaults()
    {
        // copy default values to eeprom_data_buffer
        for (size_t i = 0; i < eeprom_data_buffer.size(); i++)
        {
            eeprom_data_buffer[i] = ROM_READ_BYTE(&default_values::init_values[i]);
        }

        configure_pins();

        // write to EEPROM
        return write_all();
    }

    /**
     * @brief Save a CV to EEPROM if a value differs from the value already stored in the EEPROM.
     *
     * @return true Validation successful
     * @return false Validation failed
     */
    void CalM::update(uint16 cv_id)
    {
        hal::eeprom::update(static_cast<int>(cv_id), eeprom_data_buffer[cv_id]);
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

    void CalM::get_signal_aspect(uint8 signal_id, uint8 cmd, ::signal::signal_aspect& aspect)
    {
        if (is_user_defined(signal_id))
        {
            uint16 index = zero_based_user_defined(signal_id);
            index = cal::cv::kUserDefinedSignalBase + index * cal::cv::kSignalLength;
            aspect.num_targets = util::bits::masked_shift(
                get_cv(index++),
                cal::constants::bitmask::kNumberOfOutputs,
                cal::constants::bitshift::kNumberOfOutputs);
            aspect.aspect = get_cv(index + 2U*cmd);
            aspect.blink = get_cv(index + 2U*cmd + 1U); index += 2U*cfg::kNrSignalAspects;
            aspect.change_over_time_10ms = get_cv(index++);
            aspect.change_over_time_blink_10ms = get_cv(index++);
        }
        else if (is_built_in(signal_id))
        {
            uint16 index = zero_based_built_in(signal_id) * cal::cv::kSignalLength;
            aspect.num_targets = util::bits::masked_shift(
                ROM_READ_BYTE(&built_in_signal_outputs[index++]),
                cal::constants::bitmask::kNumberOfOutputs,
                cal::constants::bitshift::kNumberOfOutputs);
            aspect.aspect = ROM_READ_BYTE(&built_in_signal_outputs[index + 2U*cmd]);
            aspect.blink = ROM_READ_BYTE(&built_in_signal_outputs[index + 2U*cmd + 1U]); index += 2U*cfg::kNrSignalAspects;
            aspect.change_over_time_10ms = ROM_READ_BYTE(&built_in_signal_outputs[index++]);
            aspect.change_over_time_blink_10ms = ROM_READ_BYTE(&built_in_signal_outputs[index++]);
        }
        else
        {
            aspect.num_targets = 0U;
            aspect.aspect = 0U;
            aspect.blink = 0U;
            aspect.change_over_time_10ms = 0U;
            aspect.change_over_time_blink_10ms = 0U;
        }
    }

} // namespace cal
