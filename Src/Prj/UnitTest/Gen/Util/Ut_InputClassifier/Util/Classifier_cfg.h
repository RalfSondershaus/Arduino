
/**
 * @file Ut_InputClassifier/Util/Classifier_cfg.h
 *
 * @brief Configuration header file for the input classifier utility
 *
 * This file contains configuration parameters and calibration functions for the input classifier.
 * It defines default values for debounce times, class limits and pin mappings that can be
 * customized for specific classification needs.
 *
 * The classifier_cal namespace provides calibration functions for:
 * - Debounce timing configuration
 * - Upper and lower classification limits
 * - Pin mapping configuration
 *
 * Constants:
 * - kInvalidPin: Defines invalid pin value (0xFF)
 *
 * @note All functions in this file are defined as inline to allow compile-time optimization
 * 
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UTIL_CLASSIFIER_CAL_H__
#define UTIL_CLASSIFIER_CAL_H__

#include <Std_Types.h>
#include <Util/Array.h>

namespace util
{
    namespace classifier_cal
    {
        util::array<uint8, 5> cfg_limits_lo[3]; // Lower limit for class x [0, 255]
        util::array<uint8, 5> cfg_limits_hi[3]; // Upper limit for class x [0, 255]   
        util::array<uint8, 3> pins; // Pin of AD channel (such as A0)
        util::array<uint8, 3> debounces; // [10 ms] Debounce time until a class is classified: 0 sec ... 2.55 sec

        /**
         * @brief Invalid pin number, indicating no valid pin is assigned
         */
        static constexpr uint8 kInvalidPin = 0xFF;

        /**
         * @brief Get the default debounce time [ms]
         * @param classifier_type Classifier type index which selects the calibration limits to be used
         * @return debounce time [ms]
         */
        inline uint16 get_debounce_time_ms(uint8 classifier_type) { return 10U * debounces[classifier_type]; }

        /**
         * @brief Get the classifier type which selects the calibration limits to be used
         * @param idx In an array of classifiers, the index of the classifier
         * @return uint8 Classifier type for classifier at index idx
         */
        inline uint8 get_classifier_type(uint8 idx) { return idx; }

        /**
         * @brief Get the default lower limit for the given class
         * @param class_idx Class index
         * @param classifier_type Classifier type index which selects the calibration limits to be used
         * @return uint8 Lower limit for the class
         */
        uint8 get_lo_limit(uint8 classifier_type, uint8 class_idx) 
        { 
            return cfg_limits_lo[classifier_type][class_idx];
        }

        /**
         * @brief Get the default upper limit for the given class
         * @param class_idx Class index
         * @param classifier_type Classifier type index which selects the calibration limits to be used
         * @return uint8 Upper limit for the class
         */
        static uint8 get_hi_limit(uint8 classifier_type, uint8 class_idx)
        {
            return cfg_limits_hi[classifier_type][class_idx];
        }

        /**
         * @brief Get the pin number for the classifier at the given index
         * @param idx In an array of classifiers, the index of the classifier
         * @return uint8 Pin number for the classifier at index idx
         */
        inline uint8 get_pin(uint8 idx) { return pins[idx]; }

        /**
         * @brief Check if the given pin number is valid
         * 
         * @param pin Pin number to check
         * @return true If the pin number is valid
         * @return false If the pin number is invalid
         */
        static inline bool is_pin_valid(uint8 pin) { return (pin != kInvalidPin); }
    } // namespace classifier_cal

} // namespace util

#endif // UTIL_CLASSIFIER_CAL_H__
