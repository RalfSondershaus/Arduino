/**
 * @file IfcRte.h
 *
 * @descr generic classes for RTE interfaces
 *        - IfcRteBase
 *        - IfcRteSR (Sender Receiver)
 *
 * @author Ralf Sondershaus
 */

#ifndef IFCRTE_H__
#define IFCRTE_H__

// ----------------------------------------------------------
/// Base class for RTE interfaces
// ----------------------------------------------------------
template<typename T>
class IfcRteBase
{
public:
  typedef IfcRteBase<T> This;
  typedef T ThisType;
  /// Return type of RTE access functions
  typedef enum
  {
    OK = 0,
    NOK = 1
  } IfcRteRetType;
public:
  IfcRteBase() {}
  virtual ~IfcRteBase() {}
};

// ----------------------------------------------------------
/// Sender Receiver interface
// ----------------------------------------------------------
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
  virtual IfcRteRetType write(const T& t) { mData = t; return OK; }
  /// Default implementation uses operator=
  virtual IfcRteRetType read(T& t) const { t = mData; return OK; }
  /// get read-only reference to stored data
  const T& ref() const noexcept { return mData; }
  /// get write access to stored data
  T& ref() noexcept { return mData; }
};

// ----------------------------------------------------------
/// Sender Receiver interface for calibration values
// ----------------------------------------------------------
template<typename T>
class IfcRteSRCal : public IfcRteSR<T>
{
public:
  typedef IfcRteSRCal<T> This;
  typedef IfcRteSR<T> Parent;
protected:
  bool mIsValid;
public:
  IfcRteSRCal() : mIsValid(false) {}
  virtual ~IfcRteSRCal() {}
  /// Return true if calibation data are valid
  constexpr bool isValid() const noexcept { return mIsValid; }
  /// Validate calibration data
  constexpr void SetValid() noexcept { mIsValid = true; }
  /// Invalidate calibration data
  constexpr void SetInvalid() noexcept { mIsValid = false; }
};


template<typename T>
class IfcRteCS : public IfcRteBase<T>
{
public:
  IfcRteCS() {}
  virtual ~IfcRteCS() {}
  virtual T call() const = 0;
};

#endif // IFCRTE_H__
