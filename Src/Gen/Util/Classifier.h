
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
    struct classifier_limits
    {
      uint8 ucDebounce;                    ///< [10 ms] Debounce time until a class is classified: 0 sec ... 2.55 sec
      util::array<uint8, NrClasses> aucLo; ///< Lower limits for classes [0, 255]. Needs to be upscaled to match the AD value resolution.
      util::array<uint8, NrClasses> aucHi; ///< Upper limits for classes [0, 255]. Needs to be upscaled to match the AD value resolution.
    };

    template<int NrClasses>
    struct input_classifier_single
    {
      using classifier_limits_type = classifier_limits<NrClasses>;
      uint8 ucPin; ///< Pin of AD channel (such as A0)
      util::ptr<const classifier_limits_type> pLimits;    ///< Pointer to debounce and limits.
                                                                ///< Pointer is used because this can be shared between aClassifiers.
    };

    /// Example byte layout with NrClassifiers = 6, NrClasses = 5:
    /// 
    /// 12 byte per classifier:
    ///  0     ucPin
    ///  1     ucDebounce
    ///  2- 6  aucLo 0 ... 4
    ///  7-11  aucHi 0 ... 4
    template<int NrClassifiers, int NrClasses>
    struct input_classifier_cal
    {
      using input_classifier_single_type = input_classifier_single<NrClasses>;
      util::array<input_classifier_single_type, NrClassifiers> aClassifiers; ///< pin, debounce and limits
    };
  } // namespace cal

  // -----------------------------------------------------------------------------------
  /// Classify AD input value into classes. Supports NrClasses classes.
  /// Class n is active if all measured values are within the associated interval for the specified time.
  /// - Class 0: anLow[0] <= M <= anHigh[0] for time tmrDebounce_ms
  /// - Class 1: anLow[1] <= M <= anHigh[1] for time tmrDebounce_ms
  /// - ...
  /// - Class N: anLow[N] <= M <= anHigh[N] for time tmrDebounce_ms (with N = NrClasses - 1)
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
    using classifier_limits_cal_type = cal::classifier_limits<NrClasses>;
    using cal_const_pointer = util::ptr<const classifier_limits_cal_type>;

    /// The invalid index
    static constexpr class_type kInvalidIndex = platform::numeric_limits<class_type>::max_();

  protected:
    cal_const_pointer pCfg;      ///< Pointer to the configuration
    class_type currentClass;     ///< Index for anLow and anHigh for last AD sample value, 0 ... NrClasses-1; nCurClass = INVALID_INDEX if no valid class from last sample
    MilliTimer tmrDebounce_ms;   ///< Timer until button is considered to be pressed

  public:
    /// Default constructor
    classifier() : currentClass{ kInvalidIndex }
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
    time_type cod_get_debounce_time_ms() { return static_cast<time_type>(10U * static_cast<uint32>(pCfg->ucDebounce)); }

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
        tmrDebounce_ms.start(cod_get_debounce_time_ms());
      }
      return get_class_index();
    }

    /// Reset to a new measurement
    void reset()
    {
      currentClass = kInvalidIndex;
      tmrDebounce_ms.start(cod_get_debounce_time_ms());
    }
    
    /// Returns class index that was deteced or kInvalidIndex.
    class_type get_class_index() const
    {
      class_type idx = kInvalidIndex;

      if (tmrDebounce_ms.timeout())
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
  /// Requires the function analogRead(uint8 ucPic) to get the latest AD value.
  ///
  /// @tparam NrClassifiers Number of AD channels that shall be classified
  /// @tparam NrClasses Number of classes that the values shall be classified into
  // -----------------------------------------------------------------------------------
  template<int NrClassifiers, int NrClasses>
  class input_classifier
  {
  public:
    /// type of this class
    using This = input_classifier<NrClassifiers, NrClasses>;

    /// A single classifier
    using classifier_type = classifier<NrClasses>;
    /// The single classifiers are arranged in an util::array
    using classifier_array_type = util::array<classifier_type, NrClassifiers>;

    /// Calibration data type for this class including 'NrClassifiers' classifiers
    using classifier_limits_cal_type = typename classifier_type::classifier_limits_cal_type;
    using input_classifier_cal_type = cal::input_classifier_cal<NrClassifiers, NrClasses>;
    using cal_const_pointer = util::ptr<const input_classifier_cal_type>;

    /// Take over classifier_type's types. These types define bit resolution of input signals and limits
    using input_type = typename classifier_type::input_type;
    using limit_type = typename classifier_type::limit_type;
    using class_type = typename classifier_type::class_type;

    /// The classified values are arranged in an util::array
    using classified_values_array_type = util::array<uint8, NrClassifiers>;
    using adc_values_array_type = util::array<uint16, NrClassifiers>;

    /// The invalid index, type is uint8
    static constexpr uint8 kInvalidIndex = classifier_type::kInvalidIndex;

    /// Integral template parameters
    static constexpr int kNrClassifiers = NrClassifiers;
    static constexpr int kNrClasses = NrClasses;

  protected:
    /// Array of input aClassifiers
    classifier_array_type aClassifiers;
    /// Array of classified values (output)
    classified_values_array_type aClassifiedValues;
    /// Array of last AD values (for debugging)
    adc_values_array_type aunAdcValues;
    /// The configuration
    cal_const_pointer pCfg;
    
  public:
    /// Constructor
    input_classifier()
    {}

    /// Constructor
    explicit input_classifier(cal_const_pointer p)
    {
      set_config(p);
    }

    /// Set new configuration values
    void set_config(cal_const_pointer p) noexcept
    {
      pCfg = p;

      if (valid_cfg())
      {
        auto cit = p->aClassifiers.begin();
        for (auto it = aClassifiers.begin(); it != aClassifiers.end(); it++)
        {
          it->set_config(cit->pLimits);
          cit++;
        }
      }
    }

    /// Returns true if configuration parameters are available
    bool valid_cfg() const noexcept { return (bool) pCfg; }

    /// Get / calculate AD value for the given pin
    int get_ADC(uint8 ucPin) const
    {
      return hal::analogRead(static_cast<int>(ucPin));
    }

    /// Returns classified values. No boundary check on i.
    const class_type classified_value(int i) const { return aClassifiedValues[i]; }
    const classified_values_array_type& classified_values() const noexcept { return aClassifiedValues; }
    /// Returns ADC values.
    const adc_values_array_type& adc_values() const noexcept { return aunAdcValues; }

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
        auto it_cls = aClassifiedValues.begin();
        auto it_adc = aunAdcValues.begin();
        auto it_cfg = pCfg->aClassifiers.begin();
        for (auto it = aClassifiers.begin(); it != aClassifiers.end(); it++)
        {
          nAdc = get_ADC((it_cfg++)->ucPin);
          nClass = it->classify_debounce(static_cast<input_type>(nAdc));
          (*it_cls++) = static_cast<uint8>(nClass);
          (*it_adc++) = static_cast<uint16>(nAdc);
        }
      }
    }
  };

} // namespace util

#endif // UTIL_CLASSIFIER_H__
