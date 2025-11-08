
/**
 * @file Util/Classifier_cfg.h
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

#ifndef UTIL_CLASSIFIER_CAL_H__
#define UTIL_CLASSIFIER_CAL_H__

#include <Std_Types.h>

namespace util
{
    namespace classifier_cal
    {
        /**
         * @brief Invalid pin number, indicating no valid pin is assigned
         */
        static constexpr uint8 kInvalidPin = 0xFF;

        /**
         * @brief Get the default debounce time [ms]
         * @param classifier_type Classifier type index which selects the calibration limits to be used
         * @return debounce time [ms]
         */
        static inline uint16 get_debounce_time_ms(uint8 classifier_type) { (void)classifier_type; return 10U; }

        /**
         * @brief Get the classifier type which selects the calibration limits to be used
         * @param idx In an array of classifiers, the index of the classifier
         * @return uint8 Classifier type for classifier at index idx
         */
        static inline uint8 get_classifier_type(uint8 idx) { (void)idx; return 0; }

        /**
         * @brief Get the default lower limit for the given class
         * @param class_idx Class index
         * @param classifier_type Classifier type index which selects the calibration limits to be used
         * @return uint8 Lower limit for the class
         */
        static inline uint8 get_lo_limit(uint8 classifier_type, uint8 class_idx) 
        { 
            (void)classifier_type; 
            (void)class_idx;
            return 255;
        }

        /**
         * @brief Get the default upper limit for the given class
         * @param class_idx Class index
         * @param classifier_type Classifier type index which selects the calibration limits to be used
         * @return uint8 Upper limit for the class
         */
        static inline uint8 get_hi_limit(uint8 classifier_type, uint8 class_idx)
        {
            (void)classifier_type;
            (void)class_idx;
            return 0;
        }

        /**
         * @brief Get the default upper limit for the given class
         * @param idx In an array of classifiers, the index of the classifier
         * @return uint8 Upper limit for the class
         */
        static inline uint8 get_pin(uint8 idx) { (void)idx; return 0; }

    } // namespace classifier_cal

} // namespace util

#endif // UTIL_CLASSIFIER_CAL_H__
