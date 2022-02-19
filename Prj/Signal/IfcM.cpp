/**
 * Interface manager.
 */

#include <IfcM.h>

/// Get the one and only instance
static IfcM& IfcM::refSingleton()
{
  static IfcM;
  return IfcM;
}


// EOF