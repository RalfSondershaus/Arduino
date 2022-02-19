/**
 * Classify input signals from ADC.
 */

#ifndef INPUT_CLASSIFIER_H__
#define INPUT_CLASSIFIER_H__

#include <IfcClassifier.h>

 /// minimal AD raw value
#define INPUTCLASSIFIER_MIN_AD_RAW                (0)
/// maximal AD raw value
#define INPUTCLASSIFIER_MAX_AD_RAW                (1023)
/// [ms] default debounce time until a button is considered to be pressed
#define INPUTCLASSIFIER_DEFAULT_BUTTON_DEBOUNCE   (20)
/// invalid index
#define INPUTCLASSIFIER_INVALID_INDEX             (-1)

// -----------------------------------------------------------------------------------
/// Classify input signals from ADC.
// -----------------------------------------------------------------------------------
template<int NrClasses>
class InputClassifier
{
public:
  /// timer type for debouncing
  typedef MilliTimer::tTimer tTimer;
  /// type of this class
  typedef InputClassifier<NrClasses> This;

  /// type of my IfcClassifier port
  class IfcMyClassifier : public IfcClassifier<int>
  {
  public:
    /// Parent
    const This& inputClassifier;
    /// Constructor
    IfcMyClassifier(const This& classifier) : inputClassifier(classifier)
    {}
    /// Return index of the last classified value.
    virtual int getClassIndex() const
    {
      return inputClassifier.getClassIndex();
    }
  };
  /// The (provided) port for input classification
  const IfcMyClassifier mIfcMyClassifier;

protected:
  int nPin;                    ///< AD channel, e.g. A0
  int anLow[NrClasses];        ///< Lower limit for pressed [0, 1023]
  int anHigh[NrClasses];       ///< Upper limit for pressed [0, 1023]
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
    : nPin(A0)
    , nCurClass(INPUTCLASSIFIER_INVALID_INDEX)
    , tmrDebounce(INPUTCLASSIFIER_DEFAULT_BUTTON_DEBOUNCE)
    , mIfcMyClassifier(*this)
  {
    int i;
    for (i = 0; i < NrClasses; i++)
    {
      anLow[i] = INPUTCLASSIFIER_MIN_AD_RAW;
      anHigh[i] = INPUTCLASSIFIER_MAX_AD_RAW;
    }
  }
  /// Constructor
  /// nPin: Input ADC pin, e.g. A0
  /// nLow: Lower Limit for pressed [0, 1023]
  /// nHigh: Upper Limit for pressed [0, 1023]
  InputClassifier(int pin, int anL[NrClasses], int anH[NrClasses]) 
    : nPin(pin)
    , nCurClass(INPUTCLASSIFIER_INVALID_INDEX)
    , tmrDebounce(INPUTCLASSIFIER_DEFAULT_BUTTON_DEBOUNCE)
    , mIfcMyClassifier(*this)
  {
    init(pin, anL, anH);
  }
  /// Destructor: nothing to do
  ~InputClassifier()
  {}
  /// Set input channel for ADC
  void setAdcPin(int pin) { nPin = pin; }
  /// Set lower limit
  void setAdcLow(int idx, int low) { anLow[idx] = low; }
  /// Set upper limit
  void setAdcHigh(int idx, int high) { anHigh[idx] = high; }
  /// Set maximal number of samples until AD value is considered to be correct
  void setDebounce(tTimer t) { tmrDebounce = t; }
  /// Set all given parameters
  void init(int pin, int anL[NrClasses], int anH[NrClasses])
  {
    int i;
    nPin = pin;
    for (i = 0; i < NrClasses; i++)
    {
      anLow[i] = anL[i];
      anHigh[i] = anH[i];
    }
  }
  /// Measure one value
  void measure()
  {
    int nAdc;
    int nIdx;

    nAdc = analogRead(nPin);
    nIdx = findMatch(nAdc);
    if (nIdx != nCurClass)
    {
      // start timer even if nIdx is equal to INVALID_INDEX (don't care)
      nCurClass = nIdx;
      mtDebounce.start(tmrDebounce);
    }
  }
  /// Reset to a new measurement
  void reset()
  {
    nCurClass = INPUTCLASSIFIER_INVALID_INDEX;
    mtDebounce.start(tmrDebounce);
  }
  /// Return index that was deteced or INPUTCLASSIFIER_INVALID_INDEX.
  int getClassIndex() const
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
