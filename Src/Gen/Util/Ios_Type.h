/**
  * @file Ios_Type.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines basic ios types such as util::streamoff and util::streamsize.
  *        Defines util::basic_fpos.
  *
  * @copyright Copyright 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_IOS_TYPE_H
#define UTIL_IOS_TYPE_H

#include <Std_Types.h>

namespace util
{
  /// size_t is the unsigned integer type of the result of sizeof , _Alignof (since C11) and offsetof, depending on the data model.
  /// The bit width of size_t is not less than 16. (since C99)
  /// size_t can store the maximum size of a theoretically possible object of any type (including array).
  /// size_t is commonly used for array indexing and loop counting. Programs that use other types, such as unsigned int, for array 
  /// indexing may fail on, e.g. 64-bit systems when the index exceeds UINT_MAX or if it relies on 32-bit modular arithmetic.
  using size_t = ::size_t;

  /// The type std::streamoff is an implementation-defined signed integral (since C++11) type 
  /// of sufficient size to represent the maximum possible file size supported by the operating system.
  ///
  /// We use the POSIX type ssize_t here which can represent [-1, SSIZE_MAX] and fits our needs.
  /// In the C++ standard, however, std::streamoff is typically 64 bit wide (long long int).
  ///
  /// Do not use types such as long here in order to enable template functions to be partially specialized
  /// independent of ssize_t being int (on Windows) or long (on Arduino).
  using streamoff = ::ssize_t;

  /// The type std::streamsize is an implementation-defined signed integral type used to represent the 
  /// number of characters transferred in an I/O operation or the size of an I/O buffer.
  /// It is used as a signed counterpart of size_t, similar to the POSIX type ssize_t.
  /// Except in the constructors of std::strstreambuf, negative values of std::streamsize are never used.
  using streamsize = ::ssize_t;

  /// Returns size_t from v.
  /// Assumes that streamsize is used as a signed counterpart of size_t.
  inline size_t conv_streamsize_to_size_t_safe(streamsize v)
  {
    return static_cast<size_t>(v);
  }

  /// basic_fpos is a non-template version of std::fpos that support absolute positions in a stream 
  /// or in a file but does not support multi byte character representations. Support of 
  /// multi byte characters is not required for our needs.
  class basic_fpos
  {
  public:
    /// Construct
    basic_fpos(streamoff off = 0) : my_off(off) {}

    /// get the offset
    operator streamoff() const { return my_off; }

    /// Modifiers
    streamoff operator-(const basic_fpos& right) const { return static_cast<streamoff>(*this) - static_cast<streamoff>(right); }
    basic_fpos& operator+=(streamoff off) { my_off += off; return *this; }
    basic_fpos& operator-=(streamoff off) { my_off -= off; return *this; }
    basic_fpos  operator+ (streamoff off) const { basic_fpos tmp = *this; tmp += off; return tmp; }
    basic_fpos  operator- (streamoff off) const { basic_fpos tmp = *this; tmp -= off; return tmp; }

    /// Compare
    bool operator==(const basic_fpos& right) const { return static_cast<streamoff>(*this) == static_cast<streamoff>(right); }
    bool operator!=(const basic_fpos& right) const { return static_cast<streamoff>(*this) != static_cast<streamoff>(right); }

  private:
    streamoff my_off;
  };

  /// Absolute positions in a stream.
  /// Must be large enough to epresent the maximum possible file size supported by the system.
  using streampos = basic_fpos;

};

#endif // UTIL_IOS_TYPE_H