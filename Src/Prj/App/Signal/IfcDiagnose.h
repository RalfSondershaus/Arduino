/**
 * Interface for diagnostic commands.
 */

#ifndef IFC_DIAGNOSE_H__
#define IFC_DIAGNOSE_H__

#include <IfcBase.h>

// -----------------------------------------------------------------------------------
/// Interface for signal commands
// -----------------------------------------------------------------------------------
class IfcDiagnose : public IfcBase
{
public:
  /// diagnostic commands
  typedef enum
  {
    NONE = 0,
    TAKEOVER = 1
  } eIfcDiagnosticCommand;
  /// additional (input) data for current command (buffer provided by caller, content to be filled by the caller)
  typedef void * tDiagCmdData;
  /// output data for current command (buffer provided by caller, content to be filled by the callee)
  typedef void * tDiagCurData;

public:
  /// Default constructor, initializes to values that are not plausible
  IfcDiagnose()
  {}
  /// Destructor: nothing to do
  ~IfcDiagnose()
  {}
  /// Return diagnostic command.
  virtual eIfcDiagnosticCommand getCommand() const = 0;
  /// Return additional (input) data for current command (buffer provided by caller, content to be filled by the caller).
  virtual tDiagCmdData getCmdData() const = 0;
  /// Return output data for current command (buffer provided by caller, content to be filled by the callee).
  virtual tDiagCurData getCurData() const = 0;
};

#endif // IFC_DIAGNOSE_H__
