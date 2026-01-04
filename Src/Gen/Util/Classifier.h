
/**
 * @file Util/Classifier.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Classify values and classify AD input signals.
 * 
 * This header file contains classes for classifying analog input values into discrete classes:
 *
 * - classifier<NrClasses>: Classifies a single analog input into NrClasses discrete classes
 * - classifier_array<NrClassifiers,NrClasses>: Manages multiple classifiers for different analog inputs
 *
 * The classification is done with configurable upper/lower limits and debouncing.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef UTIL_CLASSIFIER_H__
#define UTIL_CLASSIFIER_H__

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Hal/Gpio.h>
#include <Util/Array.h>
#include <Util/Ptr.h>
#include <Util/Timer.h>
#include <Util/Classifier_cfg.h> // can be defined by user to override default configuration

namespace util
{
    /**
     * @brief Template class for classifying analog input values into discrete classes.
     * 
     * @tparam NrClasses Number of discrete classes to classify input values into.
     * 
     * This classifier handles analog input values (e.g., from ADC) and maps them to discrete
     * classes based on configured upper and lower limits for each class. It supports:
     * - Raw input values in 10-bit resolution (0-1023)
     * - Internal conversion to 8-bit resolution for limit comparison
     * - Optional debouncing with configurable timeout
     * - Detection of invalid/out-of-range values
     * 
     * The classifier maintains the current class state and provides both immediate
     * and debounced classification results.
     * 
     * - Class 0: anLow[0] <= M <= anHigh[0] for time debounce_timer_ms
     * - Class 1: anLow[1] <= M <= anHigh[1] for time debounce_timer_ms
     * - ...
     * - Class N: anLow[N] <= M <= anHigh[N] for time debounce_timer_ms (with N = NrClasses - 1)
     * 
     * Example usage:
     * @code
     * classifier<3> button_classifier; // Classifier with 3 classes
     * uint16_t adc_value = read_adc();
     * auto class_index = button_classifier.classify_debounce(adc_value);
     * @endcode
     * 
     * @see classifier_cal For calibration limits configuration
     * @see MilliTimer For debouncing implementation
     */
    template <int NrClasses>
    class classifier
    {
    public:
        using time_type = util::MilliTimer::time_type; /**< Timer type for debouncing */

        using input_type = uint16; /**< measurements are 10-bit unsigned AD values: 0 ... 1023 */
        using limit_type = uint8;  /**< calibration values for limits are stored with 8 bit resolution [10 bit ADC = 0 ... 1023; 1024 / 4] */
        using class_type = uint8;  /**< discrete classes are 0 ... NrClasses-1 */

        using This = classifier<NrClasses>; /**< Type of this class */

        /**
         * @brief Invalid discrete class
         */
        static constexpr class_type kInvalidIndex = platform::numeric_limits<class_type>::max_();

    protected:
        /**
         * @brief Current discrete class value
         * 
         * Index into lo_limits and hi_limits arrays corresponding to the last AD sample 
         * value, ranging from 0 to NrClasses-1.
         * Equals kInvalidIndex if the last sample did not match any valid class.
         */
        class_type current_class;
        MilliTimer debounce_timer_ms; ///< Timer until button is considered to be pressed
        /**
         * @brief The classifier type selects the calibration limits to be used.
         */
        uint8 classifier_type;

    public:
        /**
         * @brief Default constructor
         * 
         * Initializes current_class to kInvalidIndex.
         */
        classifier() : current_class{kInvalidIndex}, classifier_type{0}
        {
        }

        /** @brief Get the classifier type (which selects the calibration limits to be used) */
        uint8 get_classifier_type() const noexcept { return classifier_type; }

        /** @brief Set the classifier type (which selects the calibration limits to be used) */
        void set_classifier_type(uint8 type) noexcept { classifier_type = type; }

        /**
         * @brief Returns input (measured) value v in limit_type units
         * 
         * @param v Input value in input_type units (10-bit AD value: 0 ... 1023)
         * @return constexpr limit_type converted value in limit_type units (0 ... 255)
         */
        static constexpr limit_type convert_input(const input_type v) noexcept 
        { 
            return static_cast<limit_type>(v / static_cast<input_type>(4U)); 
        }

        /**
         * @brief Classify input value without debouncing.
         * 
         * Classifies an input value into one of NrClasses discrete classes based on configured limits.
         * Returns the index of the first class where the input value falls within its boundaries.
         * 
         * @param val Raw input value to be classified (10-bit ADC value: 0...1023)
         * @return class_type Class index if value matches a class, kInvalidIndex if no match
         */
        class_type classify(input_type val)
        {
            class_type idx = kInvalidIndex;

            const limit_type v = convert_input(val);

            for (class_type i = 0; i < static_cast<class_type>(NrClasses); i++)
            {
                if ((v >= classifier_cal::get_lo_limit(get_classifier_type(), i)) 
                 && (v <= classifier_cal::get_hi_limit(get_classifier_type(), i)))
                {
                    idx = i;
                    break;
                }
            }

            return idx;
        }

        /**
         * @brief Get the debounce time [ms]
         * 
         * @return uint16_t Debounce time [ms]
         */
        uint16_t get_debounce_time_ms() const noexcept
        {
            return classifier_cal::get_debounce_time_ms(get_classifier_type());
        }

        /**
         * @brief Classify input value with debouncing.
         * 
         * Classifies an input value into one of NrClasses discrete classes with debouncing.
         * The classification only becomes valid after the input value remains stable in a class
         * for the configured debounce time.
         * 
         * This method needs to be called cyclically to maintain proper debouncing.
         * 
         * @param val Raw input value to be classified (10-bit ADC value: 0...1023)
         * @return class_type The debounced class index if valid, kInvalidIndex if debouncing 
         *                    is in progress or value doesn't match any class
         */
        class_type classify_debounce(input_type val)
        {
            class_type idx = classify(val);
            if (idx != current_class)
            {
                // start timer even if idx is equal to kInvalidIndex (in order to measure time for
                // invalid matches)
                current_class = idx;
                debounce_timer_ms.start(get_debounce_time_ms());
            }
            return get_class_index();
        }

        /**
         * @brief Resets the classifier state.
         * 
         * Resets the current class to kInvalidIndex and restarts the debounce timer.
         */
        void reset()
        {
            current_class = kInvalidIndex;
            debounce_timer_ms.start(get_debounce_time_ms());
        }

        /**
         * @brief Returns class index that was detected or kInvalidIndex.
         * @return class_type Detected class index if debouncing time elapsed, kInvalidIndex otherwise
         */
        class_type get_class_index() const
        {
            class_type idx = kInvalidIndex;

            if (debounce_timer_ms.timeout())
            {
                idx = current_class;
            }

            return idx;
        }
    };

    /**
     * @brief Classify 10-bit unsigned AD values into 8-bit discrete class indices.
     * 
     * This class manages multiple classifiers, each responsible for classifying
     * a specific AD channel.
     * 
     * Each classifier uses the classifier<NrClasses> class to perform the actual
     * classification based on configured limits and debouncing.
     * 
     * Requires the function hal::analogRead(uint8 ucPic) to get the latest AD value.
     * 
     * @tparam NrClassifiers Number of AD channels that shall be classified
     * @tparam NrClasses Number of classes that the values shall be classified into
     */
    template <int NrClassifiers, int NrClasses>
    class classifier_array
    {
    public:
        /**
         * @brief Type of this class
         */
        using This = classifier_array<NrClassifiers, NrClasses>;

        /**
         * @brief Type of a single classifier
         */
        using classifier_type = classifier<NrClasses>;
        /**
         * @brief Type of the array holding all classifiers
         */
        using classifier_array_type = util::array<classifier_type, NrClassifiers>;

        /** 
         * @brief Take over classifier_type's types.
         * 
         * These types define bit resolution of input signals and limits
         */ 
        using input_type = typename classifier_type::input_type;
        using limit_type = typename classifier_type::limit_type;
        using class_type = typename classifier_type::class_type;

        /**
         * @brief Array storing the classified values for each classifier
         * 
         * Each element corresponds to a classifier's output class (0 to NrClasses-1 or kInvalidIndex)
         * The array size matches NrClassifiers, maintaining one classification result per AD channel
         */
        using classified_values_array_type = util::array<uint8, NrClassifiers>;
        /**
         * @brief Array storing the raw ADC values for each classifier
         */
        using adc_values_array_type = util::array<uint16, NrClassifiers>;

        /**
         * @brief Invalid class index constant
         */
        static constexpr uint8 kInvalidIndex = classifier_type::kInvalidIndex;

        /**
         * @brief Number of classifiers managed by this class
         */
        static constexpr int kNrClassifiers = NrClassifiers;
        /**
         * @brief Number of classes that the values shall be classified into
         */
        static constexpr int kNrClasses = NrClasses;

    protected:
        /**
         * @brief Array of classifiers for each AD channel
         */
        classifier_array_type classifiers;
        /**
         * @brief Array storing the classified values for each classifier
         *
         * Each element corresponds to a classifier's output class (0 to NrClasses-1 or kInvalidIndex)
         * The array size matches NrClassifiers, maintaining one classification result per AD channel
         */
        classified_values_array_type classified_values;
        /**
         * @brief Array storing the raw ADC values for each classifier
         */
        adc_values_array_type adc_values;

    public:
        /**
         * @brief Default constructor
         */
        classifier_array()
        {
        }

        /**
         * @brief Get / calculate AD value for the given pin
         * 
         * @param pin Pin number
         * @return int ADC value
         */
        int get_ADC(uint8 pin) const
        {
            return hal::analogRead(static_cast<int>(pin));
        }

        /**
         * @brief Get the classified value for a specific classifier. No boundary check on i.
         *
         * @param i Index of the classifier
         * @return class_type Classified value for classifier i
         */
        const class_type classified_value(int i) const { return classified_values[i]; }
        /**
         * @return const classified_values_array_type& Classified values array
         */
        const classified_values_array_type &get_classified_values() const noexcept { return classified_values; }
        /**
         * @return const adc_values_array_type& ADC values array
         */
        const adc_values_array_type &get_adc_values() const noexcept { return adc_values; }

        /**
         * @brief Initialize the classifier array, e.g., at system startup.
         */
        void init(void) noexcept
        {
        }

        /**
         * @brief Main execution function.
         * 
         * Reads AD values for each classifier and classifies them with debouncing.
         * Updates the classified_values and adc_values arrays accordingly.
         */
        void run(void)
        {
            int adc_value;
            int class_idx;
            auto it_cls = classified_values.begin();
            auto it_adc = adc_values.begin();
            for (auto it = classifiers.begin();
                    it != classifiers.end();
                    it++, it_adc++, it_cls++)
            {
                const int pin = classifier_cal::get_pin(static_cast<uint8>(it - classifiers.begin()));
                if (!classifier_cal::is_pin_valid(pin))
                {
                    adc_value = 0;
                    class_idx = kInvalidIndex;
                }
                else
                {
                    adc_value = get_ADC(pin);
                    // Set the classifier type for this classifier in case calibration data have changed
                    it->set_classifier_type(classifier_cal::get_classifier_type(static_cast<uint8>(it - classifiers.begin())));
                    class_idx = it->classify_debounce(static_cast<input_type>(adc_value));
                }
                *it_cls = static_cast<uint8>(class_idx);
                *it_adc = static_cast<uint16>(adc_value);
            }
        }
    };

} // namespace util

#endif // UTIL_CLASSIFIER_H__
