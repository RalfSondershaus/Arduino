/**
 * @file Rte_Type_Ifc.h
 *
 * @descr generic classes for RTE interfaces
 *        - ifc_base
 *        - ifc_sr        (Sender Receiver)
 *        - ifc_sr_array  (Sender Receiver for container types)
 *        - ifc_cs        (Client Server)
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
    using size_type = size_t;
    using ret_type = typename Base::ret_type ;
    using pointer = data_type*;
    using const_pointer = const data_type*;
  
  protected:
    data_type mData;

  public:
    /// Read and write data. Default implementation uses operator=. 
    ret_type read (      data_type& t) const { t = mData; return Base::OK; }
    ret_type write(const data_type& t)       { mData = t; return Base::OK; }
    /// size
    size_type size() const { return 1; }
    /// Returns a pointer to the data element
    pointer data() { return &mData; }
    const_pointer data() const { return &mData; }
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
    using pointer = typename array_type::pointer;
    using const_pointer = typename array_type::const_pointer;

  protected:
    array_type mData;

  public:
    /// Helper class: non const iterator
/*    class iterator
    {
    public:
      This& arr;
      size_type pos;
    public:
      iterator(This& a, size_type p) : arr(a), pos(p) {}
      void operator++() { pos++; }
      value_type& operator*() { return arr.mData.at(pos); }
      bool operator!=(const iterator& it) { return pos != it.pos; }
      bool operator==(const iterator& it) { return pos == it.pos; }
    };*/
    /// Helper class: const iterator
    /*class const_iterator
    {
    public:
      using array_const_iterator = typename array_type::const_iterator;

      const This& arr;
      array_const_iterator cit;

    public:
      const_iterator (const This& a, array_const_iterator it) : arr(a), cit(it) {}
      void operator++() { cit++; }
      const value_type& operator*() const { return *cit; }
      bool operator!=(const const_iterator& it) { return it->cit != cit; }
      bool operator==(const const_iterator& it) { return it->cit == cit; }
    };*/
  public:
    /// array
    //const_iterator begin() const noexcept { return const_iterator(*this, mData.begin()); }
    //const_iterator end() const noexcept { return const_iterator(*this, mData.end()); }
    /// Read and write array. Default implementation uses operator=. 
    ret_type read(array_type& t) const { t = mData; return Base::OK; }
    ret_type write(const array_type& t) { mData = t; return Base::OK; }
    /// Read and write a single element
    ret_type readElement(size_type pos, value_type& v) const { v = mData.at(pos); return Base::OK; }
    ret_type writeElement(size_type pos, const value_type& v) { mData.at(pos) = v; return Base::OK; }
    /// size
    size_type size() const { return mData.size(); }
    /// Returns true if pos is a valid index (is within boundaries)
    bool boundaryCheck(size_type pos) const { return pos < size(); }
    /// Returns a pointer to the underlying array
    pointer data() { return mData.data(); }
    const_pointer data() const { return mData.data(); }
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

  // ----------------------------------------------------------
  /// Make RTE ports (interfaces) available for diagnosis.
  // ----------------------------------------------------------
  typedef struct 
  {
    /// Pointer to the data or first element of the array
    void * pData;
    /// Name of the port (interface)
    const char * szName;
    /// Number of elements in pData (>= 1)
    const size_t size;
    /// size of data element
    const size_t size_of_element;
  } port_data_t;
} // namespace rte

#endif // RTE_TYPE_IFC_H__
