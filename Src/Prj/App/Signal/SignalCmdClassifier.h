/**
 * @fle SignalCmdClassifier.h
 *
 * Map classified integral input values to signal commands (IfcRteSignalCmd::eIfcRteSignalCmd, RED, GREEN, etc).
 */

#ifndef SIGNAL_CMD_CLASSIFIER_H__
#define SIGNAL_CMD_CLASSIFIER_H__

// -----------------------------------------------------------------------------------
/// Map an integral classifier to a signal command.
// -----------------------------------------------------------------------------------
class SignalCmdClassifier
{
protected:
  unsigned char aucEnum[NrClasses];        ///< enumeration values to be returned per pair nLow[.] / nHigh[.]

public:
  /// Constructor
  SignalCmdClassifier(const IfcRteSRInputClassifier& inputPort, IfcRteSignalCmd& signalPort) 
    : rteInputClassifier(inputPort)
    , rteSignalCmd(signalPort)
  {}
  /// Destructor: nothing to do
  ~SignalCmdClassifier()
  {}
  /// Cyclic runable: read from interface, map, write to output interface
  void cycle() const
  {
    int nClass;
    IfcRteSignalCmd::eIfcRteSignalCmd cmd;

    rteInputClassifier.read(nClass);

    switch (nClass)
    {
    case 0: { cmd = IfcRteSignalCmd::RED; } break;
    case 1: { cmd = IfcRteSignalCmd::WHITE; } break;
    case 2: { cmd = IfcRteSignalCmd::YELLOW; } break;
    case 3: { cmd = IfcRteSignalCmd::GREEN; } break;
    default: { cmd = IfcRteSignalCmd::INVALID; } break;
    }

    rteSignalCmd.write(cmd);
  }
};

#endif // SIGNAL_CMD_CLASSIFIER_H__
