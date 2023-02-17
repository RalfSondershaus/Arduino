
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

#include <Arduino.h>
#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Timer.h>
#include <Util/Array.h>

namespace util
{
  namespace cal
  {
    template<int NrClasses>
    struct classifier_limits
    {
      uint8 ucDebounce;                    ///< [10 ms] Debounce time until a class is classified: 0 sec ... 2.55 sec
      util::array<uint8, NrClasses> aucLo; ///< Lower limits for classes [0, 255]. Needs to be upscaled to match the AD value resolution.
      util::array<uint8, NrClasses> aucHi; ///< Upper limits for classes [0, 255]. Needs to be upscaled to match the AD value resolution.
    };

    template<int NrClasses>
    struct input_classifier_single
    {
      uint8 ucPin; ///< Pin of AD channel (such as A0)
      classifier_limits<NrClasses> limits; ///< debounce and limits
    };

    template<int NrClassifiers, int NrClasses>
    struct input_classifier_cal
    {
      util::array<input_classifier_single<NrClasses>, NrClassifiers> classifiers; ///< pin, debounce and limits
    };
  } // namespace cal

  // -----------------------------------------------------------------------------------
  /// Classify AD input value into classes. Supports NrClasses classes.
  /// Class n is active if all measured values are within the associated interval for the specified time.
  /// - Class 0: anLow[0] <= M <= anHigh[0] for time tmrDebounce
  /// - Class 1: anLow[1] <= M <= anHigh[1] for time tmrDebounce
  /// - ...
  /// - Class N: anLow[N] <= M <= anHigh[N] for time tmrDebounce (with N = NrClasses - 1)
  /// If any of the above conditions is true, the class is invalid.
  /// Method measure() must be called cyclicly.
  // -----------------------------------------------------------------------------------
  template<int NrClasses>
  class classifier
  {
  public:
    /// timer type for debouncing
    typedef util::MilliTimer::time_type time_type;

    typedef uint16 input_type; ///< measurements
    typedef uint8 limit_type; ///< calibration values for limits are stored with 8 bit resolution
    typedef uint8 class_type; ///< classes

    /// type of this class
    typedef classifier<NrClasses> This;

    /// Configuration data
    typedef cal::classifier_limits<NrClasses> classifier_limits_cal_type;
    typedef const classifier_limits_cal_type * cal_const_pointer;

    /// The invalid index
    static constexpr class_type kInvalidIndex = platform::numeric_limits<class_type>::max_();

  protected:
    class_type currentClass;     ///< Index for anLow and anHigh for last AD sample value, 0 ... NrClasses-1; nCurClass = INVALID_INDEX if no valid class from last sample
    MilliTimer tmrDebounce;      ///< Timer until button is considered to be pressed
    cal_const_pointer pCfg;      ///< Pointer to the configuration

  public:
    /// Default constructor
    classifier() 
      : pCfg{nullptr}
      , currentClass{ kInvalidIndex }
    {}
    /// Constructor with coding data
    classifier(cal_const_pointer p)
      : pCfg{ p }
      , currentClass{ kInvalidIndex }
    {}
    /// Set new configuration values
    void set_config(cal_const_pointer p) noexcept
    {
      pCfg = p;
      currentClass = kInvalidIndex;
    }

    /// Returns input (measured) value v in limit_type units
    static limit_type convertInput(const input_type v) noexcept { return static_cast<limit_type>(v / static_cast<input_type>(4U)); }

    /// Returns the debounce time in [ms]
    time_type cod_get_debounce_time() { return static_cast<time_type>(10U * static_cast<uint32>(pCfg->ucDebounce)); }

    /// Classify without debouncing.
    /// Returns the index to cfg.aunLo / cfg.aunHi with cfg.aucLo[.] <= convertInput(val) <= cfg.aucHi[.]. 
    /// Returns kInvalidIndex if convertInput(val) is never between cfg.aucLo[.] and cfg.aucHi[.].
    class_type classify(input_type val)
    {
      class_type idx = kInvalidIndex;
      
      const limit_type v = convertInput(val);

      for (class_type i = 0; i < static_cast<class_type>(NrClasses); i++)
      {
        if ((v >= pCfg->aucLo[i]) && (v <= pCfg->aucHi[i]))
        {
          idx = i;
          break;
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
      if (idx != currentClass)
      {
        // start timer even if nIdx is equal to val_not_found (in order to measure time for invalid matches)
        currentClass = idx;
        tmrDebounce.start(cod_get_debounce_time());
      }
      return get_class_index();
    }

    /// Reset to a new measurement
    void reset()
    {
      currentClass = kInvalidIndex;
      tmrDebounce.start(cod_get_debounce_time());
    }
    
    /// Returns class index that was deteced or kInvalidIndex.
    class_type get_class_index() const
    {
      class_type idx = kInvalidIndex;

      if (tmrDebounce.timeout())
      {
        idx = currentClass;
      }

      return idx;
    }
  };

  // -----------------------------------------------------------------------------------
  /// Classify 10-bit unsigned AD values into 8-bit unsigned class indices.
  ///
  /// Is based on configuration parameters that need to be provided.
  ///
  /// @tparam NrClassifiers Number of AD channels that shall be classified
  /// @tparam NrClasses Number of classes that the values shall be classified into
  // -----------------------------------------------------------------------------------
  template<int NrClassifiers, int NrClasses>
  class input_classifier
  {
  public:
    /// type of this class
    typedef input_classifier<NrClassifiers, NrClasses> This;

    /// Calibration data type for this class including N classifiers with N = NrClassifiers
    typedef cal::input_classifier_cal<NrClassifiers, NrClasses> input_classifier_cal_type;
    typedef const input_classifier_cal_type * cal_const_pointer;

    /// A classifier
    typedef classifier<NrClasses> classifier_type;
    /// The classifiers are arranged in an util::array
    typedef util::array<classifier_type, NrClassifiers> classifier_array_type;

    /// Take over classifier_type's types. These types define bit resolution of input signals and limits
    using input_type = typename classifier_type::input_type;
    using limit_type = typename classifier_type::limit_type;
    using class_type = typename classifier_type::class_type;

    /// The classified values are arranged in an util::array
    typedef util::array<uint8, NrClassifiers> classified_values_array_type;

    /// The invalid index, type is uint8
    static constexpr uint8 kInvalidIndex = classifier_type::kInvalidIndex;

    /// Integral template parameters
    static constexpr int kNrClassifiers = NrClassifiers;
    static constexpr int kNrClasses = NrClasses;

  protected:
    /// Array of input classifiers
    classifier_array_type classifiers;
    /// Array of classified values (output)
    classified_values_array_type classifiedValues;
    /// The configuration
    cal_const_pointer pCfg;
  public:
    /// Constructor
    input_classifier() : pCfg{ nullptr }
    {}

    /// Constructor
    explicit input_classifier(cal_const_pointer p) : pCfg{ nullptr }
    {
      setConfig(p);
    }

    /// Set new configuration values
    void set_config(cal_const_pointer p) noexcept
    {
      pCfg = p;

      if (valid_cfg())
      {
        auto cit = p->classifiers.begin();
        for (auto it = classifiers.begin(); it != classifiers.end(); it++)
        {
          it->set_config(&(cit->limits));
          cit++;
        }
      }
    }

    /// Returns true if configuration parameters are available
    bool valid_cfg() const noexcept { return pCfg != nullptr; }

    /// Get / calculate AD value for the given pin
    int get_ADC(uint8 ucPin) const
    {
      return analogRead(static_cast<int>(ucPin));
    }

    /// Returns classified values. No range check on i.
    const class_type classified_value(int i) const { return classifiedValues[i]; }
    const classified_values_array_type& classified_values() const noexcept { return classifiedValues; }

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
        auto it_cls = classifiedValues.begin();
        auto it_cfg = pCfg->classifiers.begin();
        for (auto it = classifiers.begin(); it != classifiers.end(); it++)
        {
          nAdc = get_ADC((it_cfg++)->ucPin);
          nClass = it->classify_debounce(static_cast<input_type>(nAdc));
          (*it_cls++) = static_cast<uint8>(nClass);
        }
      }
    }
  };

} // namespace util

#endif // UTIL_CLASSIFIER_H__
