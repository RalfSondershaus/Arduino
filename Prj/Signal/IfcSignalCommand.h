/**
 * Interface for signal commands.
 */

#ifndef IFC_SIGNAL_COMMAND_H__
#define IFC_SIGNAL_COMMAND_H__

#include <IfcBase.h>

// -----------------------------------------------------------------------------------
/// Interface for signal commands
// -----------------------------------------------------------------------------------
class IfcSignalCommand : public IfcBase
{
public:
  /// signal commands
  typedef enum
  {
    RED = 0,
    WHITE = 1,
    YELLOW = 2,
    GREEN = 3,
    ALL = 254,
    INVALID = 255
  } eIfcSignalCommand;

public:
  /// Default constructor, initializes to values that are not plausible
  IfcSignalCommand()
  {}
  /// Destructor: nothing to do
  ~IfcSignalCommand()
  {}
  /// Return last signal command.
  virtual eIfcSignalCommand getCommand() const = 0;
};

#endif // IFC_SIGNAL_COMMAND_H__
