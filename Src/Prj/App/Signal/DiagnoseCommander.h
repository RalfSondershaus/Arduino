/**
 * Class DiagnoseCommander.
 */

#ifndef DIAGNOSECOMMANDER_H__
#define DIAGNOSECOMMANDER_H__

#include <IfcDiagnose.h>

// -----------------------------------------------------------------------------------
/// Control diagnostic commands
// -----------------------------------------------------------------------------------
class DiagnoseCommander
{
public:
  /// Command type
  typedef IfcDiagnose::eIfcDiagnosticCommand eIfcDiagnosticCommand;
  /// Type for signal commands
  typedef IfcSignalCommand::eIfcSignalCommand eIfcSignalCommand;

  /// My IfcDiagnose port
  class IfcPortDiagnose : public IfcDiagnose
  {
  protected:
    /// Reference to parent class
    const DiagnoseCommander& mParent;

  public:
    /// Constructor
    IfcPortDiagnose(const DiagnoseCommander& d) : mParent(d) {}
    /// Return diagnostic command.
    virtual eIfcDiagnosticCommand getCommand() const { return mParent.mCmd; }
    /// Return additional (input) data for current command (buffer provided by caller, content to be filled by the caller).
    virtual tDiagCmdData getCmdData() const { return (tDiagCmdData) &mParent.mDiagCmdData; }
    /// Return output data for current command (buffer provided by caller, content to be filled by the callee).
    virtual tDiagCurData getCurData() const { return (tDiagCmdData) &mParent.mDiagCurData; }
  };
  /// The (provided) port for diagnostic commands
  IfcPortDiagnose mIfcPortDiagnose;

  /// My IfcSignalCommand port
  class IfcPortSignalCommand : public IfcSignalCommand
  {
  public:
    /// Parent
    const DiagnoseCommander& mParent;
    /// The signal command for the port
    eIfcSignalCommand mSignalCommand;
    /// Constructor
    IfcPortSignalCommand(const DiagnoseCommander& dc) : mParent(dc), mSignalCommand(INVALID)
    {}
    /// Return index of the last classified value.
    virtual eIfcSignalCommand getCommand() const
    {
      return mSignalCommand;
    }
  };
  /// The (provided) IfcSignalCommand port
  const IfcPortSignalCommand mIfcPortSignalCommand;

protected:
  void parseCommand(int nCmd);
  void cmdC();
  /// The last valid command (can be NONE)
  eIfcDiagnosticCommand mCmd;
  /// The last valid diagnose command data (can be NONE)
  unsigned char mDiagCmdData;
  /// The buffer for current diagnose data
  unsigned char mDiagCurData;

public:
  /// Constructor
  DiagnoseCommander();
  /// Destructor
  ~DiagnoseCommander()
  {}
  /// Set output values
  void update();
  /// initialize output ports
  void init();
};

#endif // DIAGNOSECOMMANDER_H__
