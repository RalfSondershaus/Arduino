/**
 * Interface for classifier ports.
 */

#ifndef IFC_CLASSIFIER_H__
#define IFC_CLASSIFIER_H__

#include <IfcBase.h>

/// Base class for RTE interfaces
template<typename T>
class IfcRteBase
{
public:
  typedef IfcRte<T> This;
  typedef T ThisType;
  typedef enum
  {
    OK = 0,
    NOK = 1
  } IfcRteRetType;
public:
  IfcRteBase() {}
  virtual ~IfcRteBase() {}
};

/// Sender Receiver interface
template<typename T>
class IfcRteSR : public IfcRteBase<T>
{
public:
  typedef IfcRteSR<T> This;
  T mData;
public:
  IfcRteSR() {}
  virtual ~IfcRteSR() {}
  /// Default implementation uses operator=
  virtual IfcRteRetType write(const T& t) { mData = t; }
  /// Default implementation uses operator=
  virtual IfcRteRetType read(T& t) const { t = mData; }
};
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
