/**
 * Basic interface declaration.
 */

#ifndef IFCBASE_H__
#define IFCBASE_H__

// -----------------------------------------------------------------------------------
/// The base class for all interfaces.
// -----------------------------------------------------------------------------------
class IfcBase
{
public:
  /// Constructor, does nothing
  IfcBase()     {}
  /// Destructor, does nothing
  ~IfcBase()   {}
  /// Switch to another interface, needed for multiplexer interfaces, if a sub class implements this interface function, true is returned.
  virtual bool switchInterface(int nIdx) { return false };
};

#endif // IFCBASE_H__
