/**
 * Interface for classifier ports.
 */

#ifndef IFC_CLASSIFIER_H__
#define IFC_CLASSIFIER_H__

#include <IfcBase.h>

// -----------------------------------------------------------------------------------
/// Interface for classifier ports
// -----------------------------------------------------------------------------------
template<typename T>
class IfcClassifier : public IfcBase
{
public:

public:
  /// Default constructor, initializes to values that are not plausible
  IfcClassifier()
  {}
  /// Destructor: nothing to do
  ~IfcClassifier()
  {}
  /// Return index of the last classified value.
  virtual T getClassIndex() const = 0;
};

#endif // IFC_CLASSIFIER_H__
