 /**
  * @file InputClassifier.h
  *
  * @descr Classify AD input signals.
  *
  * @author Ralf Sondershaus
  */

#ifndef INPUT_CLASSIFIER_H__
#define INPUT_CLASSIFIER_H__

#include <Arduino.h>
#include <Gen/Timer/Timer.h>

//$ TODO Move these #defines into a template parameter class

/// minimal AD raw value
#define INPUTCLASSIFIER_MIN_AD_RAW                (0)
/// maximal AD raw value
#define INPUTCLASSIFIER_MAX_AD_RAW                (1023)
/// [ms] default debounce time until a button is considered to be pressed
#define INPUTCLASSIFIER_DEFAULT_BUTTON_DEBOUNCE   (20)
/// invalid index
#define INPUTCLASSIFIER_INVALID_INDEX             (-1)

// -----------------------------------------------------------------------------------
/// Classify input signals from ADC. Supports NrClasses classes.  
/// Class n is active if each measured analogue value M for a debounce time is within the interval of class n.
/// - Class 0: anLow[0] <= M <= anHigh[0] for time tmrDebounce
/// - Class 1: anLow[1] <= M <= anHigh[1] for time tmrDebounce
/// - ...
/// - Class N: anLow[N] <= M <= anHigh[N] for time tmrDebounce (with N = NrClasses - 1)
/// If any of the above conditions is true, the class is invalid.
/// Method measure() must be called cyclicly.
// -----------------------------------------------------------------------------------
template<int NrClasses>
class InputClassifier
{
public:
  /// timer type for debouncing
  typedef MilliTimer::tTimer tTimer;
  /// type of this class
  typedef InputClassifier<NrClasses> This;
  /// Configuration type
  typedef struct
  {
    int     anLow[NrClasses];   ///< Lower limit for class x [0, 1023]
    int     anHigh[NrClasses];  ///< Upper limit for class x [0, 1023]
    uint8_t u8Pin;              ///< AD channel, e.g. A0
  } tConfig;
protected:
  uint8_t u8Pin;               ///< AD channel, e.g. A0
  int anLow[NrClasses];        ///< Lower limit for class x [0, 1023]
  int anHigh[NrClasses];       ///< Upper limit for class x [0, 1023]
  int nCurClass;               ///< Index for anLow and anHigh for last AD sample value, 0 ... NrClasses-1; nCurClass = INVALID_INDEX if no valid class from last sample
  tTimer tmrDebounce;          ///< [ms] Debounce time until button is considered to be pressed
  MilliTimer mtDebounce;       ///< Timer until button is considered to be pressed

  /// Return the index to anLow / anHigh with anLow[.] <= nAdc <= anHigh[.]. Return INVALID_INDEX if nAdc is never between anLow and anHigh.
  int findMatch(int nAdc) const
  {
    int i;
    int nIdx = INPUTCLASSIFIER_INVALID_INDEX;

    for (i = 0; i < NrClasses; i++)
    {
      if ((nAdc >= anLow[i]) && (nAdc <= anHigh[i]))
      {
        nIdx = i;
        break;
      }
    }
    return nIdx;
  }

public:
  /// Default constructor, initializes to values that are not plausible
  InputClassifier()
    : u8Pin(A0)
    , nCurClass(INPUTCLASSIFIER_INVALID_INDEX)
    , tmrDebounce(INPUTCLASSIFIER_DEFAULT_BUTTON_DEBOUNCE)
  {
    int i;
    for (i = 0; i < NrClasses; i++)
    {
      anLow[i] = INPUTCLASSIFIER_MIN_AD_RAW;
      anHigh[i] = INPUTCLASSIFIER_MAX_AD_RAW;
    }
  }
  /// Constructor
  /// port: reference to RTE output port
  /// nPin: Input ADC pin, e.g. A0
  /// nLow: Lower Limit for pressed [0, 1023]
  /// nHigh: Upper Limit for pressed [0, 1023]
  InputClassifier(uint8_t pin, const int anL[NrClasses], const int anH[NrClasses])
    : u8Pin(pin)
    , nCurClass(INPUTCLASSIFIER_INVALID_INDEX)
    , tmrDebounce(INPUTCLASSIFIER_DEFAULT_BUTTON_DEBOUNCE)
  {
    init(pin, anL, anH);
  }
  /// Copy constructor
  InputClassifier(const This& toCopy)
  {
    init(toCopy.u8Pin, toCopy.anLow, toCopy.anHigh);
  }
  /// Destructor: nothing to do
  ~InputClassifier()
  {}
  /// Set input channel for ADC
  constexpr void setAdcPin(uint8_t pin) { u8Pin = pin; }
  /// Set lower limit
  constexpr void setAdcLow(int idx, int low) { anLow[idx] = low; }
  /// Set upper limit
  constexpr void setAdcHigh(int idx, int high) { anHigh[idx] = high; }
  /// Set maximal number of samples until AD value is considered to be correct
  constexpr void setDebounce(tTimer t) { tmrDebounce = t; }
  /// Set pin and intervals
  void setConfig(const tConfig& pConfig)
  {
    init(pConfig.u8Pin, pConfig.anLow, pConfig.anHigh);
  }
  /// Set pin and intervals
  void init(uint8_t pin, const int anL[NrClasses], const int anH[NrClasses])
  {
    int i;
    u8Pin = pin;
    for (i = 0; i < NrClasses; i++)
    {
      anLow[i] = anL[i];
      anHigh[i] = anH[i];
    }
    nCurClass = INPUTCLASSIFIER_INVALID_INDEX;
  }
  /// Measure one value via analogRead(nPin) and update RTE interface if a new class was measured and verified (debounced).
  /// Return last measured AD value.
  int measure()
  {
    int nAdc;
    int nIdx;

    nAdc = analogRead(u8Pin);
    nIdx = findMatch(nAdc);
    if (nIdx != nCurClass)
    {
      // start timer even if nIdx is equal to INVALID_INDEX (don't care)
      nCurClass = nIdx;
      mtDebounce.start(tmrDebounce);
    }
    return nAdc;
  }
  /// Reset to a new measurement
  constexpr void reset()
  {
    nCurClass = INPUTCLASSIFIER_INVALID_INDEX;
    mtDebounce.start(tmrDebounce);
  }
  /// Return index that was deteced or INPUTCLASSIFIER_INVALID_INDEX.
  constexpr int getClassIndex() const
  {
    int nIdx = INPUTCLASSIFIER_INVALID_INDEX;

    if (mtDebounce.timeout())
    {
      nIdx = nCurClass;
    }

    return nIdx;
  }
};

#endif // INPUT_CLASSIFIER_H__
