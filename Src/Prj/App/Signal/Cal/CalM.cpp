/**
 * @file Signal/Cal/CalM.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief  Access to calibration parameters
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
#include <Rte/Rte.h>
#include <Hal/EEPROM.h>

#include <Cal/CalM_config.h>

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
            static const uint8 init_values[] = EEPROM_INIT;

            static const util::array<output_type, cfg::kNrBuiltInSignals> built_in_signal_outputs = CAL_BUILT_IN_SIGNAL_OUTPUTS;
        }
    }

    // -----------------------------------------------
    /// Returns CRC8 from EEPROM addresses unStartNvmId ... unStartNvmId + unLen - 1.
    /// ucCrc is initial CRC value.
    // -----------------------------------------------

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
     * @brief Read manufacturer ID and compare against initial value (default EEPORM value if never
     * written before).
     */
    bool CalM::is_valid()
    {
        return hal::eeprom::read(eeprom::eManufacturerID) != hal::eeprom::kInitial;
    }

    /**
     * @brief calculate checksum for @ref signals and @ref classifier_array.
     */
    uint8 CalM::calc_checksum()
    {
        uint8 ucCrc = 0; // start value

        // signals
        for (auto it = signals.begin(); it < signals.end(); it++)
        {
            ucCrc = calc_sum(it, sizeof(signal_type), ucCrc);
        }

        // classifiers
        for (auto it = classifier_array.classifiers.begin(); it < classifier_array.classifiers.end(); it++)
        {
            ucCrc = calc_sum(it, sizeof(classifier_array_element_type), ucCrc);
        }

        return ucCrc;
    }

#if 0
    /**
     * @brief Initialize @ref base_cv and corresponding EEPROM values with default values from ROM.
     */
    void CalM::init_base_CV()
    {
        base_cv.CV1_address_LSB = eeprom::default_values::init_values[eeprom::eDecoderAddressLSB];
        base_cv.CV7_manufacturer_version_ID = eeprom::default_values::init_values[eeprom::eManufacturerVersionID];
        base_cv.CV8_manufacturer_ID = eeprom::default_values::init_values[eeprom::eManufacturerID];
        base_cv.CV9_address_MSB = eeprom::default_values::init_values[eeprom::eDecoderAddressMSB];
        base_cv.CV29_configuration = eeprom::default_values::init_values[eeprom::eConfiguration];

        (void)update_base_CV();
    }
#endif
    /**
     * @brief Update @ref sig_type and @ref classifier_array_element from given raw values
     * 
     * @todo Add digital inputs
     */
    void CalM::set_input_and_classifier(
        uint8 signal_pos,
        classifier_array_element_type& classifier_array_element,
        signal_type& sig_type,
        uint8 signal_input_raw_val, 
        uint8 classifier_type_raw_val)
    {
        uint8 adc_pin{ 0 };
        uint8 classifier_type_val;
        util::ptr<const cal::classifier_type> limits_ptr;

        bool update_classifier_array = false;

        const uint8 input_type_val = util::bits::masked_shift(
                                    signal_input_raw_val,
                                    cal::signal::bitmask::kInputType,
                                    cal::signal::bitshift::kInputType);
        // A classifier is required for ADC values only
        if (input_type_val == cal::signal::values::kInputType_ADC)
        {
            adc_pin = util::bits::masked_shift(
                                 signal_input_raw_val,
                                 cal::signal::bitmask::kAdcPin, 
                                 cal::signal::bitshift::kAdcPin);
            classifier_type_val = util::bits::masked_shift(
                                    classifier_type_raw_val,
                                    cal::signal::bitmask::kClassifierType,
                                    cal::signal::bitshift::kClassifierType);
            if (classifier_type_val >= cfg::kNrUserDefinedClassifierTypes)
            {
                // fallback in case of out-of-bounds: use first classifier
                classifier_type_val = 0;
            }
            limits_ptr = &user_defined_classifier_types[classifier_type_val];
            sig_type.input.type = cal::input_type::eAdc;
            sig_type.input.idx = signal_pos;
            update_classifier_array = true;
        }
        else if (input_type_val == cal::signal::values::kInputType_DCC)
        {
            // DCC address is DCC output address plus signal index
            adc_pin = 0;
            // limits_ptr is already nullptr
            sig_type.input.type = cal::input_type::eDcc;
            sig_type.input.idx = signal_pos;
            update_classifier_array = true;
        }
        else
        {
            // Digital input
        }

        if (update_classifier_array)
        {
            if ((classifier_array_element.pin != adc_pin) ||
                (classifier_array_element.limits_ptr != limits_ptr))
            {
                classifier_array_element.pin = adc_pin;
                classifier_array_element.limits_ptr = limits_ptr;
                    //&user_defined_classifier_types[classifier_type_val];
            }
            // Classifiers do not read the configuration cyclicly, so we need to inform
            // them if anything changed. This causes the classifier to reset and use
            // the new configuration.
            rte::ifc_rte_update_config_for_classifier::call(signal_pos);
        }
    }

    /**
     * @brief Returns true if signal is a built in signal
     * 
     * @param id Signal ID from CV values
     * @return true if signal is built in
     * @return false otherwise
     */
    inline bool is_built_in(CalM::SignalId id) noexcept
    {
        return (id.val >= kFirstBuiltInSignalID) &&
               (id.val < eeprom::default_values::built_in_signal_outputs.max_size());
    }

    /**
     * @brief Returns the zero based index of signal id to be used for array indexing
     * 
     * @param id signal id
     * @return uint8 zero based index of the signal id
     * 
     * @note Use for built in signal ids only.
     */
    uint8 zero_based_built_in(CalM::SignalId id)  { return id.val - kFirstBuiltInSignalID; }

    /**
     * @brief Returns the zero based index of signal id to be used for array indexing
     * 
     * @param id signal id
     * @return uint8 uint8 zero based index of the signal id
     * 
     * @note Use for built in signal ids only.
     */
    uint8 zero_based_user_defined(CalM::SignalId id) { return id.val - kFirstUserDefinedSignalID; }

    /**
     * @brief Use bit operations to update @ref first_target with @ref raw_val.
     */
    void CalM::set_first_target(cal::signal_type& sig_type, uint8 raw_val)
    {
        sig_type.first_target.type = util::bits::masked_shift(
                                    raw_val,
                                    cal::signal::bitmask::kFirstOutputType,
                                    cal::signal::bitshift::kFirstOutputType);
        sig_type.first_target.idx = util::bits::masked_shift(
                                    raw_val,
                                    cal::signal::bitmask::kFirstOutputPin,
                                    cal::signal::bitshift::kFirstOutputPin);
    }

    /**
     * @brief Update output_ptr of @ref sig_type.
     * 
     * @param sig_type The data structure to be updated
     * @param signal_id Which signal type it is
     */
    void CalM::set_output(signal_type& sig_type, CalM::SignalId signal_id)
    {
        if (is_built_in(signal_id))
        {
            sig_type.output_ptr = eeprom::default_values::built_in_signal_outputs.data()
                                + zero_based_built_in(signal_id);
            sig_type.signal_id = signal_id.val;
        }
        else if (is_user_defined(signal_id))
        {
            sig_type.output_ptr = user_defined_signal_outputs.data() 
                                + zero_based_user_defined(signal_id);
            sig_type.signal_id = signal_id.val;
        }
        else
        {
            // invalid signal
            sig_type.output_ptr = util::ptr<const cal::output_type>();
            sig_type.signal_id = signal_id.val;
        }
    }

#if 0
    /**
     * @brief Initialize @ref signals, @ref classifier_array, and @ref user_defined_signal_outputs
     *        with default values from ROM.
     */
    void CalM::init_signals()
    {
        uint16 eep_idx;
        uint16 eep_idx_input;
        uint16 eep_idx_classifier_type;
        uint16 eep_idx_signal_id;
        uint16 eep_idx_first_output;
        uint8 pos;

        // user defined signals
        eep_idx = eeprom::eUserDefinedSignalBase;
        for (auto it = user_defined_signal_outputs.begin(); 
                  it < user_defined_signal_outputs.end(); 
                  it++)
        {
            it->num_targets = util::bits::masked_shift(
                eeprom::default_values::init_values[eep_idx++],
                user_defined_signal::bitmask::kNumberOfOutputs,
                user_defined_signal::bitshift::kNumberOfOutputs);
            for (auto asp_it = it->aspects.begin(); asp_it < it->aspects.end(); asp_it++)
            {
                asp_it->aspect = eeprom::default_values::init_values[eep_idx++];
                asp_it->blink = eeprom::default_values::init_values[eep_idx++];
            }
            it->change_over_time = eeprom::default_values::init_values[eep_idx++];
            it->change_over_time_blink = eeprom::default_values::init_values[eep_idx++];
        }

        // first target and output
        eep_idx_signal_id = eeprom::eSignalIDBase;
        eep_idx_first_output = cal::eeprom::eSignalFirstOutputBase;

        for (pos = 0; pos < cfg::kNrSignals; pos++)
        {
            SignalId signal_id { eeprom::default_values::init_values[eep_idx_signal_id++] };

            set_first_target(
                signals.at(pos), 
                eeprom::default_values::init_values[eep_idx_first_output++]);
            set_output(signals.at(pos), signal_id);
        }

        // inputs
        eep_idx_input = cal::eeprom::eSignalInputBase;
        eep_idx_classifier_type = cal::eeprom::eSignalInputClassifierTypeBase;

        for (pos = 0; pos < cfg::kNrSignals; pos++)
        {
            set_input_and_classifier(
                pos,
                classifier_array.classifiers.at(pos),
                signals.at(pos),
                eeprom::default_values::init_values[eep_idx_input++],
                eeprom::default_values::init_values[eep_idx_classifier_type++]);

        }

        update_signals();
    }
#endif

#if 0
   /**
     * @brief Initialize @ref user_defined_classifier_types and corresponding EEPROM values
     *        with default values from ROM
     */
    void CalM::init_user_defined_classifiers()
    {
        // Initialize user-defined classifiers
        util::memcpy(user_defined_classifier_types.begin(), 
                     &eeprom::default_values::init_values[eeprom::eClassifierBase], 
                     cfg::kNrUserDefinedClassifierTypes * sizeof(classifier_type));

        update_user_defined_classifiers();
    }
#endif
    /**
     * @brief Read values for @ref base_cv from EEPROM
     */
    void CalM::read_base_CV()
    {
        base_cv.CV1_address_LSB = hal::eeprom::read(eeprom::eDecoderAddressLSB);
        base_cv.CV9_address_MSB = hal::eeprom::read(eeprom::eDecoderAddressMSB);
        base_cv.CV8_manufacturer_ID = hal::eeprom::read(eeprom::eManufacturerID);
        base_cv.CV7_manufacturer_version_ID = hal::eeprom::read(eeprom::eManufacturerVersionID);
        base_cv.CV29_configuration = hal::eeprom::read(eeprom::eConfiguration);
    }

    /**
     * @brief Read values for @ref signals and @ref classifier_array from EEPROM
     */
    void CalM::read_signals()
    {
        uint16 eep_idx = eeprom::eUserDefinedSignalBase;
        uint16 eep_idx_first_output;
        uint16 eep_idx_classifier_type;
        uint8 pos;

        // user defined signals
        for (auto it = user_defined_signal_outputs.begin(); 
                  it < user_defined_signal_outputs.end(); 
                  it++)
        {
            it->num_targets = util::bits::masked_shift(
                hal::eeprom::read(eep_idx),
                user_defined_signal::bitmask::kNumberOfOutputs,
                user_defined_signal::bitshift::kNumberOfOutputs);
            eep_idx++;
            for (auto asp_it = it->aspects.begin(); asp_it < it->aspects.end(); asp_it++)
            {
                asp_it->aspect = hal::eeprom::read(eep_idx++);
                asp_it->blink = hal::eeprom::read(eep_idx++);
            }
            it->change_over_time = hal::eeprom::read(eep_idx++);
            it->change_over_time_blink = hal::eeprom::read(eep_idx++);
        }

        // first target and output
        eep_idx = eeprom::eSignalIDBase;
        eep_idx_first_output = eeprom::eSignalFirstOutputBase;
        for (auto it = signals.begin(); it < signals.end(); it++)
        {
            const SignalId signal_id { hal::eeprom::read(eep_idx++) };

            set_first_target(*it, hal::eeprom::read(eep_idx_first_output++));
            set_output(*it, signal_id);
        }

        // inputs
        eep_idx = eeprom::eSignalInputBase;
        eep_idx_classifier_type = eeprom::eSignalInputClassifierTypeBase;
        for (pos = 0; pos < cfg::kNrSignals; pos++)
        {
            set_input_and_classifier(
                pos,
                classifier_array.classifiers.at(pos),
                signals.at(pos),
                hal::eeprom::read(eep_idx++),
                hal::eeprom::read(eep_idx_classifier_type++));
        }
    }

    /**
     * @brief Read values for @ref user_defined_classifier_types from EEPROM.
     */
    void CalM::read_classifiers()
    {
        uint16 unEepIdx = eeprom::eClassifierBase; // local index, starts with first classifier

        for (auto it = user_defined_classifier_types.begin(); 
                  it < user_defined_classifier_types.end(); 
                  it++)
        {
            it->debounce_time = hal::eeprom::read(unEepIdx++);
            for (auto limit_it = it->lo_limits.begin(); limit_it < it->lo_limits.end(); limit_it++)
            {
                *limit_it = hal::eeprom::read(unEepIdx++);
            }
            for (auto limit_it = it->hi_limits.begin(); limit_it < it->hi_limits.end(); limit_it++)
            {
                *limit_it = hal::eeprom::read(unEepIdx++);
            }
        }
    }

    /**
     * @brief Store @ref base_cv in EEPROM if a value differs from the value already stored 
     *        in the EEPROM.
     */
    void CalM::update_base_CV()
    {
        hal::eeprom::update(eeprom::eDecoderAddressLSB, base_cv.CV1_address_LSB);
        hal::eeprom::update(eeprom::eDecoderAddressMSB, base_cv.CV9_address_MSB);
        hal::eeprom::update(eeprom::eManufacturerID, base_cv.CV8_manufacturer_ID);
        hal::eeprom::update(eeprom::eManufacturerVersionID, base_cv.CV7_manufacturer_version_ID);
        hal::eeprom::update(eeprom::eConfiguration, base_cv.CV29_configuration);
    }

    /**
     * @brief Returns the position of limits_ptr in @ref user_defined_classifier_types
     * 
     * @param limits_ptr Pointer to limits that shall be found
     * @return uint8 Position of limits_ptr in @ref user_defined_classifier_types.
     *         255 if limits_ptr is not found.
     */
    uint8 CalM::find_classifier_type(util::ptr<const classifier_type> limits_ptr)
    {
        uint8 pos = 0;
        for (auto it = user_defined_classifier_types.begin();
                  it != user_defined_classifier_types.end();
                  it++)
        {
            if (it == limits_ptr.get())
            {
                break;
            }
            pos++;
        }

        if (pos >= user_defined_classifier_types.size())
        {
            pos = 255;
        }

        return pos;
    }

    /**
     * @brief Store @ref signals and @ref classifier_array to EEPROM. 
     *        Save data to EEPROM only if the new value differs from the one currently stored.
     */
    void CalM::update_signals()
    {
        uint16 eep_idx;
        uint16 eep_idx_signal_id;
        uint16 eep_idx_first_output;
        uint16 eep_idx_classifier_type;
        uint8 tmp;

        eep_idx_signal_id = eeprom::eSignalIDBase;
        eep_idx_first_output = cal::eeprom::eSignalFirstOutputBase;

        // signal ids and first outputs
        for (auto it = signals.begin(); it < signals.end(); it++)
        {
            tmp = util::bits::lshift(it->first_target.type, signal::bitshift::kFirstOutputType)
                | util::bits::lshift(it->first_target.idx, signal::bitshift::kFirstOutputPin);
            hal::eeprom::update(eep_idx_signal_id++, it->signal_id);
            hal::eeprom::update(eep_idx_first_output++, tmp);
        }

        // user defined signals
        eep_idx = eeprom::eUserDefinedSignalBase;
        for (auto it = user_defined_signal_outputs.begin(); 
                  it < user_defined_signal_outputs.end(); 
                  it++)
        {
            hal::eeprom::update(eep_idx++, it->num_targets);
            for (auto asp_it = it->aspects.begin(); asp_it < it->aspects.end(); asp_it++)
            {
                hal::eeprom::update(eep_idx++, asp_it->aspect);
                hal::eeprom::update(eep_idx++, asp_it->blink);
            }
            hal::eeprom::update(eep_idx++, it->change_over_time);
            hal::eeprom::update(eep_idx++, it->change_over_time_blink);
        }

        // signal inputs
        eep_idx = cal::eeprom::eSignalInputBase;
        eep_idx_classifier_type = cal::eeprom::eSignalInputClassifierTypeBase;
        for (auto it = signals.begin(); it < signals.end(); it++)
        {
            tmp = util::bits::lshift(it->input.type, signal::bitshift::kInputType)
                | util::bits::lshift(classifier_array.classifiers[it->input.idx].pin, signal::bitshift::kAdcPin);
            hal::eeprom::update(eep_idx++, tmp);
            if (it->input.type == cal::input_type::eAdc)
            {
                tmp = util::bits::lshift(
                    find_classifier_type(classifier_array.classifiers[it->input.idx].limits_ptr),
                    signal::bitshift::kClassifierType
                );
            }
            else
            {
                tmp = 0;
            }
            hal::eeprom::update(eep_idx_classifier_type++, tmp);
        }
    }

    /**
     * @brief Store @ref user_defined_classifier_types in EEPROM.
     */
    void CalM::update_user_defined_classifiers()
    {
        uint16 unEepIdx = eeprom::eClassifierBase; // local index, starts with first classifier

        for (auto it = user_defined_classifier_types.begin(); 
                  it < user_defined_classifier_types.end(); 
                  it++)
        {
            hal::eeprom::update(unEepIdx++, it->debounce_time);
            for (auto limit_it = it->lo_limits.begin(); limit_it < it->lo_limits.end(); limit_it++)
            {
                hal::eeprom::update(unEepIdx++, *limit_it);
            }
            for (auto limit_it = it->hi_limits.begin(); limit_it < it->hi_limits.end(); limit_it++)
            {
                hal::eeprom::update(unEepIdx++, *limit_it);
            }
        }
    }

    /**
     * @brief Read all configurations from EEPROM and compare CV 9 (manufacturer ID) against initial
     * value (default EEPORM value if never written before).
     * 
     * @return true CV 9 has been written
     * @return false CV 9 has not been written
     */
    bool CalM::read_all()
    {
        read_base_CV();
        read_signals();
        read_classifiers();
        return is_valid();
    }

    /**
     * @brief Store all configurations to EEPROM.
     * 
     * @return true CV 9 has been written successfully
     * @return false CV 9 has not been written
     */
    bool CalM::update()
    {
        update_base_CV();
        update_signals();
        update_user_defined_classifiers();

        // verify
        return read_all();
    }

    /**
     * @brief Server function. Update @ref signal at @ref signal_pos.
     * 
     * @todo Re-calculate LEDs.
     */
    rte::ret_type CalM::set_signal_first_target(
        uint8 signal_pos, 
        const target_type& first_target, 
        bool do_update)
    {
        rte::ret_type ret = rte::ret_type::NOK;

        if (signals.check_boundary(signal_pos))
        {
            signals.at(signal_pos).first_target = first_target;
            // 
            if (do_update)
            {
                update_signals();
            }
            ret = rte::ret_type::OK;
        }

        return ret;
    }

    /**
     * @brief Server function. Update @ref signal at @ref signal_pos.
     */
    rte::ret_type CalM::set_signal_id(uint8 signal_pos, const uint8 signal_id, bool do_update)
    {
        rte::ret_type ret = rte::ret_type::NOK;

        if (signals.check_boundary(signal_pos))
        {
            SignalId sig_id { signal_id };
            set_output(signals.at(signal_pos), sig_id);
            if (signals.at(signal_pos).output_ptr)
            {
                if (do_update)
                {
                    update_signals();
                }
                ret = rte::ret_type::OK;
            }
        }
        return ret;
    }

    /**
     * @brief Server function. Update @ref signals and @ref classifier_array at @ref signal_pos.
     */
    rte::ret_type CalM::set_signal_input_classifier(
        uint8 signal_pos, 
        const uint8 adc_pin, 
        const uint8 classifier_type_val,
        bool do_update)
    {
        rte::ret_type ret = rte::ret_type::NOK;

        if (user_defined_classifier_types.check_boundary(classifier_type_val) &&
            classifier_array.classifiers.check_boundary(signal_pos) &&
            signals.check_boundary(signal_pos))
        {
            signals.at(signal_pos).input.type = input_type::eAdc;
            signals.at(signal_pos).input.idx = signal_pos;
            classifier_array.classifiers.at(signal_pos).pin = adc_pin;
            classifier_array.classifiers.at(signal_pos).limits_ptr = 
                user_defined_classifier_types.data() + classifier_type_val;
            if (do_update)
            {
                update_signals();
            }
            ret = rte::ret_type::OK;
        }

        return ret;
    }

    /**
     * @brief Server function. Update @ref signals at @ref signal_pos
     */
    rte::ret_type CalM::set_signal_input_dcc(uint8 signal_pos, 
                                       bool do_update)
    {
        rte::ret_type ret = rte::ret_type::NOK;

        if (signals.check_boundary(signal_pos))
        {
            signals.at(signal_pos).input.type = input_type::eDcc;
            signals.at(signal_pos).input.idx = signal_pos;
            if (do_update)
            {
                update_signals();
            }
            ret = rte::ret_type::OK;
        }

        return ret;
    }

    /**
     * @brief Server function. Update base_cv.
     */
    rte::ret_type CalM::set_base_cv(const base_cv_cal_type &new_base_cv, bool do_update)
    {
        base_cv = new_base_cv;

        if (do_update)
        {
            update_base_CV();
        }

        return rte::ret_type::OK;
    }

    /**
     * @brief Get a CV
     */
    rte::ret_type CalM::get_cv(uint16 cv_id, uint8 *val)
    {
        rte::ret_type success = rte::ret_type::NOK;

        if (cv_id < static_cast<uint16>(cv::eLastCV))
        {
            *val = hal::eeprom::read(cv_id);
            hal::serial::print("CalM::get_cv cv_id=");
            hal::serial::print(cv_id);
            hal::serial::print(" val=");
            hal::serial::println(*val);
            success = rte::ret_type::OK;
        }

        return success;
    }

    /**
     * @brief Set a CV
     */
    rte::ret_type CalM::set_cv(uint16 cv_id, uint8 val)
    {
        typedef void (CalM::*member_func_type)(void);
        struct CvFuncTuple 
        {
            uint16 cv_min;
            uint16 cv_max;
            member_func_type func;
        };

        static const util::array<CvFuncTuple, 5> lut = {{ 
            {                          0, cv::eConfiguration        , &CalM::read_base_CV },
            { cv::eSignalIDBase         , cv::eClassifierBase       , &CalM::read_signals },
            { cv::eClassifierBase       , cv::eUserDefinedSignalBase, &CalM::read_classifiers },
            { cv::eUserDefinedSignalBase, cv::eLastCV               , &CalM::read_signals },
        }};

        rte::ret_type success = rte::ret_type::NOK;

        if (cv_id < static_cast<uint16>(cv::eLastCV))
        {
            hal::eeprom::update(cv_id, val);
            for (const auto& element : lut)
            {
                if ((cv_id >= element.cv_min) && (cv_id < element.cv_max))
                {
                    (this->*element.func)();
                }
            }
            success = rte::ret_type::OK;
        }

        return success;
    }

    #if 0
    /**
     * @brief Server function. Initialize EEPROM with ROM default values
     * 
     * @return rte::ret_type 
     */
    rte::ret_type CalM::init_all()
    {
        init_base_CV();
        init_signals();
        init_user_defined_classifiers();
        return rte::ret_type::OK;
    }
    #endif
    /**
     * @brief Server function. Initialize EEPROM with ROM default values
     * 
     * @return rte::ret_type OK Data validation ok (written manufacturer ID valid)
     * @return rte::ret_type NOK Data validation ok (written manufacturer ID in valid)
     */
    rte::ret_type CalM::set_defaults()
    {
        uint16 pos;

        for (pos = 0; pos < eeprom::eSizeOfData; pos++)
        {
            hal::eeprom::update(pos, eeprom::default_values::init_values[pos]);
        }

        return read_all() ? rte::ret_type::OK : rte::ret_type::NOK;
    }

    // -----------------------------------------------
    /// Calculate bit field: for each port, set a bit to 1 if the port is used by a LED
    /// or clear the bit to 0 if the port is not used by a LED.
    // -----------------------------------------------
    void CalM::calc_leds()
    {
        const signal_cal_type *signals_ptr = get_signal();
        if (signals_ptr != nullptr)
        {
            // set all bits to zero
            leds.reset();
            for (auto it = signals_ptr->begin(); it != signals_ptr->end(); it++)
            {
                if (it->first_target.type == target_type::eOnboard)
                {
                    uint8 pos;
                    const uint8 num_targets = (it->output_ptr) ? (it->output_ptr->num_targets) : (0);
                    for (pos = it->first_target.idx; 
                         pos < it->first_target.idx + num_targets; 
                         pos++)
                    {
                        leds.set(pos);
                    }
                }
            }
        }
    }

    // -----------------------------------------------
    /// Init runable
    // -----------------------------------------------
    void CalM::init()
    {
        if (!read_all())
        {
            // invalid / never programmed: initialize EEPROM with default values
            set_defaults();
            read_all();
        }

        calc_leds();
    }

    // -----------------------------------------------
    /// Cyclic runable
    // -----------------------------------------------
    void CalM::cycle100()
    {
    }

} // namespace cal
