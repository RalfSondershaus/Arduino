/**
 * @file Rte_Type_Ifc.h
 * @author Ralf Sondershaus
 *
 * @descr generic classes for RTE interfaces
 *        - IfcBase
 *        - IfcSR     (Sender Receiver)
 *        - IfcSRCal  (Sender Receiver for calibration data)
 *        - IfcCS     (Client Server)
 *
 * @copyright Copyright 2020 - 2022 Ralf Sondershaus
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * See <https://www.gnu.org/licenses/>.
 */

#ifndef RTE_TYPE_IFC_H__
#define RTE_TYPE_IFC_H__

namespace Rte
{
  // ----------------------------------------------------------
  /// Base class for RTE interfaces
  // ----------------------------------------------------------
  template<typename T>
  class IfcBase
  {
  public:
    typedef T ifc_type;
    typedef IfcBase<ifc_type> This;
    /// Return type of RTE access functions
    typedef enum
    {
      OK = 0,
      NOK = 1
    } ret_type;
  public:
    IfcBase() {}
    virtual ~IfcBase() {}
  };

  // ----------------------------------------------------------
  /// Sender Receiver interface.
  /// Owns a copy of data of type T.
  // ----------------------------------------------------------
  template<typename T>
  class IfcSR : public IfcBase<T>
  {
  public:
    typedef IfcBase<T> Base;
    typedef IfcSR<T> This;
    typedef typename Base::ifc_type ifc_type;
    typedef typename Base::ret_type ret_type;

    ifc_type mData;

  public:
    IfcSR() {}
    virtual ~IfcSR() {}
    /// Read and write data. Default implementation uses operator=. 
    virtual ret_type read (      ifc_type& t) const { t = mData; return Base::OK; }
    virtual ret_type write(const ifc_type& t)       { mData = t; return Base::OK; }
    /// get reference to stored data
    const ifc_type& ref() const noexcept { return mData; }
    ifc_type& ref() noexcept { return mData; }
  };

  // ----------------------------------------------------------
  /// Sender Receiver interface for calibration values
  // ----------------------------------------------------------
  template<typename T>
  class IfcSRCal : public IfcSR<T>
  {
  public:
    typedef IfcSRCal<T> This;
    typedef IfcSR<T> Base;
    typedef typename Base::ifc_type ifc_type;
    typedef typename Base::ret_type ret_type;
  protected:
    bool mIsValid;
  public:
    IfcSRCal() : mIsValid(false) {}
    virtual ~IfcSRCal() {}
    /// Return true if calibation data are valid
    constexpr bool isValid() const noexcept { return mIsValid; }
    /// Validate calibration data
    constexpr void SetValid() noexcept { mIsValid = true; }
    /// Invalidate calibration data
    constexpr void SetInvalid() noexcept { mIsValid = false; }
  };

  // ----------------------------------------------------------
  /// Client Server interface
  // ----------------------------------------------------------
  template<typename T>
  class IfcCS : public IfcBase<T>
  {
  public:
    typedef IfcBase<T> Base;
    typedef IfcCS<T> This;
    typedef typename Base::ifc_type ifc_type;
    typedef typename Base::ret_type ret_type;

    IfcCS() {}
    virtual ~IfcCS() {}
    virtual ifc_type call() const = 0;
  };

} // namespace Rte

#endif // RTE_TYPE_IFC_H__
