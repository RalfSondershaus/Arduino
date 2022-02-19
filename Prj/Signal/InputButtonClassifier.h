/**
 * Classify input signals from AD values
 */

#ifndef INPUT_BUTTON_CLASSIFIER_H__
#define INPUT_BUTTON_CLASSIFIER_H__

#include <InputClassifier.h>
#include <IfcSignalCommand.h>

// -----------------------------------------------------------------------------------
/// Classify input signals from ADC.
// -----------------------------------------------------------------------------------
template<int NrClasses>
class InputButtonClassifier : public InputClassifier<NrClasses>
{
public:
  /// type of this class
  typedef InputButtonClassifier<NrClasses> This;
  /// type of parent class
  typedef InputClassifier<NrClasses> Parent;
  /// type of signal commands
  typedef IfcSignalCommand::eIfcSignalCommand eIfcSignalCommand;

  /// type of my IfcSignalCommand port
  class IfcPortSignalCommand : public IfcSignalCommand
  {
  public:
    /// Parent
    const This& inputBtnClassifier;
    /// Constructor
    IfcPortSignalCommand(const This& classifier) : inputBtnClassifier(classifier)
    {}
    /// Return index of the last classified value.
    virtual eIfcSignalCommand getCommand() const
    {
      return inputBtnClassifier.getCommand();
    }
  };
  /// The (provided) port
  const IfcPortSignalCommand mIfcPortSignalCommand;

protected:
  unsigned char aucEnum[NrClasses];        ///< enumeration values to be returned per pair nLow[.] / nHigh[.]

public:
  /// Constructor
  /// nPin: Input ADC pin, e.g. A0
  /// aucEnum: enumeration value to be returned per pair nLow[.] / nHigh[.]
  /// nLow: Lower Limit for pressed [0, 1023]
  /// nHigh: Upper Limit for pressed [0, 1023]
  InputButtonClassifier(int pin, unsigned char aucE[NrClasses], int anL[NrClasses], int anH[NrClasses]) : Parent(pin, anL, anH), mIfcPortSignalCommand(*this)
  {
    int i;

    for (i = 0; i < NrClasses; i++)
    {
      aucEnum[i] = aucE[i];
    }
  }
  /// Destructor: nothing to do
  ~InputButtonClassifier()
  {}
  /// get last command
  eIfcSignalCommand getCommand() const
  {
    eIfcSignalCommand cmd;
    const int nClass = getClassIndex();

    if ((nClass >= 0) && (nClass < NrClasses))
    {
      cmd = (eIfcSignalCommand)  aucEnum[nClass];
    }
    else
    {
      cmd = eIfcSignalCommand::INVALID;
    }
    return cmd;
  }
};

#endif // INPUT_BUTTON_CLASSIFIER_H__
