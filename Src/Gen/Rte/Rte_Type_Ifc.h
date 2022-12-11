/**
 * @file Rte_Type_Ifc.h
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
    /// Read and write data. Default implementation uses operator=. 
    ret_type read (      data_type& t) const { t = mData; return Base::OK; }
    ret_type write(const data_type& t)       { mData = t; return Base::OK; }
    /// get reference to stored data
    const data_type& ref() const noexcept { return mData; }
    data_type& ref() noexcept { return mData; }
  };

  // ----------------------------------------------------------
  /// Sender Receiver interface for array types.
  /// Owns a copy of array type T.
  /// @tparam T An array type such as util::array<int>
  // ----------------------------------------------------------
  template<typename T>
  class ifc_sr_array : public ifc_base
  {
  public:
    typedef ifc_base Base;
    typedef T array_type;
    typedef ifc_sr<array_type> This;
    using value_type = typename array_type::value_type;
    using size_type = typename array_type::size_type;
    using ret_type = typename Base::ret_type;

    array_type mData;

  public:
    /// Read and write array. Default implementation uses operator=. 
    ret_type read(array_type& t) const { t = mData; return Base::OK; }
    ret_type write(const array_type& t) { mData = t; return Base::OK; }
    /// Read and write a single element
    ret_type readElement(size_type pos, value_type& v) const { v = mData.at(pos); return Base::OK; }
    ret_type writeElement(size_type pos, const value_type& v) { mData.at(pos) = v; return Base::OK; }
    /// get reference to stored data
    const array_type& ref() const noexcept { return mData; }
    array_type& ref() noexcept { return mData; }
  };

  // ----------------------------------------------------------
  /// Client Server interface.
  /// @tparam Ret Return type of the member function to be called
  /// @tparam Cls class type of the object to be called
  /// @tparam Args Parameter types (arguments) of the member function to be called
  // ----------------------------------------------------------
  template<typename Ret, typename Cls, typename ... Args>
  class ifc_cs : public ifc_base
  {
  public:
    /// Base class
    typedef ifc_base Base;
    /// The server object is of this type
    typedef Cls class_type;
    /// Reference to the server object
    typedef class_type& class_type_reference;
    /// The server function has this return type
    typedef Ret ret_type;
    /// Type of a pointer to a member function of class class_type
    typedef ret_type (class_type::*member_func_type)(Args...);

  protected:
    /// The server object
    class_type_reference obj;
    /// The pointer to a member function
    member_func_type func;

  public:
    /// Constructor
    ifc_cs(class_type_reference o, member_func_type f) : obj(o), func(f) {}
    /// Server function
    ret_type call(Args... args) { return CALL_MEMBER_FUNC(obj, func)(args...); }
  };

  // ----------------------------------------------------------
  /// Client Server interface. Partial specialization for the important use case of a
  /// member function without parameters.
  /// @tparam Ret Return type of the member function to be called
  /// @tparam Cls class type of the object to be called
  // ----------------------------------------------------------
  template<typename Ret, typename Cls>
  class ifc_cs<Ret, Cls, void> : public ifc_base
  {
  public:
    /// Base class
    typedef ifc_base Base;
    /// The server object is of this type
    typedef Cls class_type;
    /// Reference to the server object
    typedef class_type& class_type_reference;
    /// The server function has this return type
    typedef Ret ret_type;
    /// Type of a pointer to a member function of class class_type
    typedef ret_type(class_type::*member_func_type)(void);

  protected:
    /// The server object
    class_type_reference obj;
    /// The pointer to a member function
    member_func_type func;

  public:
    /// Constructor
    ifc_cs(class_type_reference o, member_func_type f) : obj(o), func(f) {}
    /// Server function
    ret_type call() { return CALL_MEMBER_FUNC(obj, func)(); }
  };

} // namespace rte

#endif // RTE_TYPE_IFC_H__
