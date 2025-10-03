
/**
  * @file Util/Classifier.h
  *
  * @author Ralf Sondershaus
  *
  * @descr Classify values and classify AD input signals.
  *
  * This file doesn't have include guards because it is included several times in a row.
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

#ifndef UTIL_CLASSIFIER_H__
#define UTIL_CLASSIFIER_H__

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Hal/Gpio.h>
#include <Util/Array.h>
#include <Util/Ptr.h>
#include <Util/Timer.h>

namespace util
{
  namespace cal
  {
    template<int NrClasses>
    struct classifier
    {
      uint8 debounce_time;              ///< [10 ms] Debounce time until a class is classified: 0 sec ... 2.55 sec
      util::array<uint8, NrClasses> lo_limits; ///< Lower limits for classes [0, 255]. Needs to be upscaled to match the AD value resolution.
      util::array<uint8, NrClasses> hi_limits; ///< Upper limits for classes [0, 255]. Needs to be upscaled to match the AD value resolution.
    };

    template<int NrClasses>
    struct classifier_array_element
    {
      using classifier_type = classifier<NrClasses>;

      uint8 pin;                                    ///< Pin of AD channel (such as A0)
      util::ptr<const classifier_type> limits_ptr;  ///< Pointer to debounce and limits.
                                ///< A pointer is used because it can be shared between classifiers.
    };

    /// Coding data for an array of classifiers
    template<int NrClassifiers, int NrClasses>
    struct classifier_array
    {
      using classifier_array_element_type = classifier_array_element<NrClasses>;

      util::array<classifier_array_element_type, NrClassifiers> classifiers; ///< pin, debounce and limits
    };
  } // namespace cal

  // -----------------------------------------------------------------------------------
  /// Classify AD input value into classes. Supports NrClasses classes.
  /// Class n is active if all measured values are within the associated interval for the specified time.
  /// - Class 0: anLow[0] <= M <= anHigh[0] for time debounce_timer_ms
  /// - Class 1: anLow[1] <= M <= anHigh[1] for time debounce_timer_ms
  /// - ...
  /// - Class N: anLow[N] <= M <= anHigh[N] for time debounce_timer_ms (with N = NrClasses - 1)
  /// If any of the above conditions is true, the class is invalid.
  /// Method measure() must be called cyclicly.
  // -----------------------------------------------------------------------------------
  template<int NrClasses>
  class classifier
  {
  public:
    /// timer type for debouncing
    using time_type = util::MilliTimer::time_type;

    using input_type = uint16;  ///< measurements
    using limit_type = uint8;   ///< calibration values for limits are stored with 8 bit resolution [10 bit ADC = 0 ... 1023; 1024 / 4]
    using class_type = uint8;   ///< classes

    /// type of this class
    using This = classifier<NrClasses>;

    /// Configuration data
    using classifier_cal_type = cal::classifier<NrClasses>;
    using cal_const_pointer = util::ptr<const classifier_cal_type>;

    /// The invalid index
    static constexpr class_type kInvalidIndex = platform::numeric_limits<class_type>::max_();

  protected:
    cal_const_pointer cfg_ptr;      ///< Pointer to the configuration
    class_type current_class;     ///< Index for anLow and anHigh for last AD sample value, 0 ... NrClasses-1; nCurClass = INVALID_INDEX if no valid class from last sample
    MilliTimer debounce_timer_ms;   ///< Timer until button is considered to be pressed

  public:
    /// Default constructor
    classifier() : current_class{ kInvalidIndex }
    {}
    /// Constructor with coding data
    classifier(cal_const_pointer p)
      : cfg_ptr{ p }
      , current_class{ kInvalidIndex }
    {}
    /// Set new configuration values
    void set_config(cal_const_pointer p) noexcept
    {
      cfg_ptr = p;
      current_class = kInvalidIndex;
    }

    /// Returns input (measured) value v in limit_type units
    static constexpr limit_type convert_input(const input_type v) noexcept { return static_cast<limit_type>(v / static_cast<input_type>(4U)); }

    /// Returns the debounce time in [ms]
    time_type cod_get_debounce_time_ms() { return static_cast<time_type>(10U * static_cast<uint32>(cfg_ptr->debounce_time)); }

    /// Classify without debouncing.
    /// Returns the index to cfg.aunLo / cfg.aunHi with cfg.lo_limits[.] <= convertInput(val) <= cfg.hi_limits[.]. 
    /// Returns kInvalidIndex if convertInput(val) is never between cfg.lo_limits[.] and cfg.hi_limits[.].
    class_type classify(input_type val)
    {
      class_type idx = kInvalidIndex;
      
      if (cfg_ptr)
      {
        const limit_type v = convert_input(val);

        for (class_type i = 0; i < static_cast<class_type>(NrClasses); i++)
        {
            if ((v >= cfg_ptr->lo_limits[i]) && (v <= cfg_ptr->hi_limits[i]))
            {
                idx = i;
                break;
            }
        }
      }

      return idx;
    }

    /// Classify with debouncing.
    /// Needs to be triggered cyclicly. Starts debouncing if required.
    ///
    /// @param val Value to be classified (with debouncing)
    ///
    /// @return class index
    class_type classify_debounce(input_type val)
    {
      class_type idx = classify(val);
      if (idx != current_class)
      {
        // start timer even if idx is equal to kInvalidIndex (in order to measure time for 
        // invalid matches)
        current_class = idx;
        debounce_timer_ms.start(cod_get_debounce_time_ms());
      }
      return get_class_index();
    }

    /// Reset to a new measurement
    void reset()
    {
      current_class = kInvalidIndex;
      debounce_timer_ms.start(cod_get_debounce_time_ms());
    }
    
    /// Returns class index that was deteced or kInvalidIndex.
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

  // -----------------------------------------------------------------------------------
  /// Classify 10-bit unsigned AD values into 8-bit unsigned class indices.
  ///
  /// Is based on configuration parameters that need to be provided.
  ///
  /// Requires the function analogRead(uint8 ucPic) to get the latest AD value.
  ///
  /// @tparam NrClassifiers Number of AD channels that shall be classified
  /// @tparam NrClasses Number of classes that the values shall be classified into
  // -----------------------------------------------------------------------------------
  template<int NrClassifiers, int NrClasses>
  class classifier_array
  {
  public:
    /// type of this class
    using This = classifier_array<NrClassifiers, NrClasses>;

    /// A single classifier
    using classifier_type = classifier<NrClasses>;
    /// The single classifiers are arranged in an util::array
    using classifier_array_type = util::array<classifier_type, NrClassifiers>;

    /// Calibration data type for this class including 'NrClassifiers' classifiers
    using classifier_cal_type = typename classifier_type::classifier_cal_type;
    using classifier_array_cal_type = cal::classifier_array<NrClassifiers, NrClasses>;
    using cal_const_pointer = util::ptr<const classifier_array_cal_type>;

    /// Take over classifier_type's types. These types define bit resolution of input signals and limits
    using input_type = typename classifier_type::input_type;
    using limit_type = typename classifier_type::limit_type;
    using class_type = typename classifier_type::class_type;

    /// @brief Stores classified values in a fixed-size util::array, one entry per classifier
    using classified_values_array_type = util::array<uint8, NrClassifiers>;
    /// @brief Holds raw ADC values corresponding to each classifier (for debugging)
    using adc_values_array_type = util::array<uint16, NrClassifiers>;

    /// The invalid index, type is uint8
    static constexpr uint8 kInvalidIndex = classifier_type::kInvalidIndex;

    /// Integral template parameters
    static constexpr int kNrClassifiers = NrClassifiers;
    static constexpr int kNrClasses = NrClasses;

  protected:
    /// Array of classifiers
    classifier_array_type classifiers;
    /// Array of classified values (output)
    classified_values_array_type classified_values;
    /// Array of last AD values (for debugging)
    adc_values_array_type adc_values;
    /// The configuration
    cal_const_pointer cfg_ptr;
    
  public:
    /// Constructor
    classifier_array()
    {}

    /// Constructor
    explicit classifier_array(cal_const_pointer p)
    {
      set_config(p);
    }

    /**
     * @brief Update the configuration and update the limits for the classifier at classifier_pos
     * 
     * @param p Pointer to the configuration
     * @param classifier_pos Position of the classifier [0 ... @ref NrClassifiers).
     */
    void set_config(cal_const_pointer p, uint8 classifier_pos) noexcept
    {
        cfg_ptr = p;

        if (valid_cfg() && classifiers.check_boundary(classifier_pos))
        {
            classifiers.at(classifier_pos).set_config(p->classifiers.at(classifier_pos).limits_ptr);
        }
    }

    /// Set new configuration values
    void set_config(cal_const_pointer p) noexcept
    {
      cfg_ptr = p;

      if (valid_cfg())
      {
        auto cit = p->classifiers.begin();
        for (auto it = classifiers.begin(); it != classifiers.end(); it++)
        {
          it->set_config(cit->limits_ptr);
          cit++;
        }
      }
    }

    /// Returns true if configuration parameters are available
    bool valid_cfg() const noexcept { return (bool) cfg_ptr; }

    /// Get / calculate AD value for the given pin
    int get_ADC(uint8 pin) const
    {
      return hal::analogRead(static_cast<int>(pin));
    }

    /// Returns classified values. No boundary check on i.
    const class_type classified_value(int i) const { return classified_values[i]; }
    const classified_values_array_type& get_classified_values() const noexcept { return classified_values; }
    /// Returns ADC values.
    const adc_values_array_type& get_adc_values() const noexcept { return adc_values; }

    /// Initialization at system start
    void init(void) noexcept
    {}

    /// Main execution function.
    /// Reads AD value (get_ADC) and classifies it with debouncing.
    void run(void)
    {
      if (valid_cfg())
      {
        int nAdc;
        int nClass;
        auto it_cls = classified_values.begin();
        auto it_adc = adc_values.begin();
        auto it_cfg = cfg_ptr->classifiers.begin();
        for (auto it = classifiers.begin(); 
                  it != classifiers.end(); 
                  it++, it_adc++, it_cfg++, it_cls++)
        {
            nAdc = get_ADC(it_cfg->pin);
            nClass = it->classify_debounce(static_cast<input_type>(nAdc));
            *it_cls = static_cast<uint8>(nClass);
            *it_adc = static_cast<uint16>(nAdc);
        }
      }
    }
  };

} // namespace util

#endif // UTIL_CLASSIFIER_H__
