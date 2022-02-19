/**
 * Interface manager.
 */

#ifndef IFCM_H__
#define IFCM_H__

#include <IfcBase.h>

// -----------------------------------------------------------------------------------
/// Manage interfaces.
// -----------------------------------------------------------------------------------
class IfcM
{
protected:
  IfcM()  {}
  ~IfcM() {}

  IfcBase * maIfcBase[];

public:
  /// Get the one and only instance
  static IfcM& refSingleton();

};

#endif // IFCM_H__
