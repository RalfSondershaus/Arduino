/**
 * Classify AD input signals and map to button IDs.
 */

#ifndef INPUT_BUTTON_CLASSIFIER_H__
#define INPUT_BUTTON_CLASSIFIER_H__

#include <InputClassifier.h>
#include <IfcSignalCommand.h>

// -----------------------------------------------------------------------------------
/// Map an integral classifier into a signal command.
// -----------------------------------------------------------------------------------
class SignalCmdClassifier
{
public:
  /// type of this class
  typedef InputButtonClassifier<NrClasses> This;
  /// type of signal commands
  typedef IfcSignalCommand::eIfcSignalCommand eIfcSignalCommand;

protected:
  unsigned char aucEnum[NrClasses];        ///< enumeration values to be returned per pair nLow[.] / nHigh[.]
  /// Use the interface to receive last measured value
  Parent::IfcRteSRInputClassifier mMeasuresValue;

public:
  /// Constructor
  /// nPin: Input ADC pin, e.g. A0
  /// aucEnum: enumeration value to be returned per pair nLow[.] / nHigh[.]
  /// nLow: Lower Limit for pressed [0, 1023]
  /// nHigh: Upper Limit for pressed [0, 1023]
  InputButtonClassifier(int pin, unsigned char aucE[NrClasses], int anL[NrClasses], int anH[NrClasses]) : Parent(pin, anL, anH)
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
  void cycle()
  {
    Parent::cycle();
  }
};

#endif // INPUT_BUTTON_CLASSIFIER_H__
