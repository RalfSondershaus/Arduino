/**
 * @file Rte_Type_Ifc.h
 * @author Ralf Sondershaus
 *
 * @descr generic classes for RTE interfaces
 *        - ifc_base
 *        - ifc_sr      (Sender Receiver)
 *        - ifc_sr_cal  (Sender Receiver for calibration data)
 *        - ifc_cs      (Client Server)
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

namespace rte
{
  // ----------------------------------------------------------
  /// Base class for RTE interfaces
  // ----------------------------------------------------------
  class ifc_base
  {
  public:
    /// Return type of RTE access functions
    typedef enum
    {
      OK = 0,
      NOK = 1
    } ret_type;
  public:
    ifc_base() {}
    virtual ~ifc_base() {}
  };

  // ----------------------------------------------------------
  /// Sender Receiver interface.
  /// Owns a copy of data of type T.
  // ----------------------------------------------------------
  template<typename T>
  class ifc_sr : public ifc_base
  {
  public:
    typedef ifc_base Base;
    typedef T data_type;
    typedef ifc_sr<data_type> This;
    typedef typename Base::ret_type ret_type;

    data_type mData;

  public:
    ifc_sr() {}
    virtual ~ifc_sr() {}
    /// Read and write data. Default implementation uses operator=. 
    virtual ret_type read (      data_type& t) const { t = mData; return Base::OK; }
    virtual ret_type write(const data_type& t)       { mData = t; return Base::OK; }
    /// get reference to stored data
    const data_type& ref() const noexcept { return mData; }
    data_type& ref() noexcept { return mData; }
  };

  // ----------------------------------------------------------
  /// Sender Receiver interface for calibration values
  // ----------------------------------------------------------
  template<typename T>
  class ifc_sr_cal : public ifc_sr<T>
  {
  public:
    typedef ifc_sr_cal<T> This;
    typedef ifc_sr<T> Base;
  protected:
    bool mIsValid;
  public:
    ifc_sr_cal() : mIsValid(false) {}
    virtual ~ifc_sr_cal() {}
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
  class ifc_cs : public ifc_base
  {
  public:
    typedef ifc_base Base;
    typedef typename Base::ret_type ret_type;

    ifc_cs() {}
    virtual ~ifc_cs() {}
    virtual ret_type call() const = 0;
  };

} // namespace rte

#endif // RTE_TYPE_IFC_H__
