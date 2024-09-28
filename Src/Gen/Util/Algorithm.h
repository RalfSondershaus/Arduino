/**
  * @file Algorithm.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Generic algorithms
  *
  * @copyright Copyright 2022 Ralf Sondershaus
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

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <Std_Types.h>
#include <Platform_Limits.h>

namespace util
{
  // ------------------------------------------------------------------------------
  /// Returns true if lowerlimit <= v < upperlimit.
  // ------------------------------------------------------------------------------
  template<typename T>
  bool is_in_interval(const T v, const T lowerlimit, const T upperlimit)
  {
    return ((v >= lowerlimit) && (v < upperlimit));
  }

  // ------------------------------------------------------------------------------
  /// Returns an interator to the element that equals v.
  /// Returns last if the element is not found.
  /// Performs a linear search.
  // ------------------------------------------------------------------------------
  template<class InputIt, class T>
  InputIt find(InputIt first, InputIt last, const T& v)
  {
    while (first != last)
    {
      if (*first == v)
      {
        break;
      }
      first++;
    }
    return first;
  }

  // ------------------------------------------------------------------------------
  /// Copies the elements in the range, defined by [first, last), to another range 
  /// beginning at d_first (copy destination range).
  // ------------------------------------------------------------------------------
  template< class InputIt, class OutputIt >
  OutputIt copy(InputIt s_first, InputIt last, OutputIt d_first)
  {
    while (s_first != last)
    {
      *d_first++ = *s_first++;
    }

    return d_first;
  }

  // ------------------------------------------------------------------------------
  /// Copies exactly count values from the range beginning at first to the range 
  /// beginning at d_first.
  /// 
  /// Returns iterator in the destination range, pointing past the last element 
  /// copied if count > 0 or d_first otherwise.
  // ------------------------------------------------------------------------------
  template< class InputIt, class Size, class OutputIt >
  OutputIt copy_n(InputIt s_first, Size count, OutputIt d_first)
  {
    while (count > 0U)
    {
      *d_first++ = *s_first++;
      count--;
    }

    return d_first;
  }

  // ------------------------------------------------------------------------------
  /// Assigns the given value to the elements in the range [first, last)
  // ------------------------------------------------------------------------------
  template< class ForwardIt, class T >
  void fill(ForwardIt d_first, ForwardIt d_last, const T& value)
  {
    while (d_first != d_last)
    {
      *d_first++ = value;
    }
  }

  // ------------------------------------------------------------------------------
  /// Assigns the given value to the first count elements in the range beginning at 
  /// first if count > 0. Does nothing otherwise.
  // ------------------------------------------------------------------------------
  template< class OutputIt, class Size, class T >
  OutputIt fill_n(OutputIt d_first, Size count, const T& value)
  {
    while (count > 0U)
    {
      *d_first++ = value;
      count--;
    }

    return d_first;
  }

  // ------------------------------------------------------------------------------
  /// Copies count bytes from the object pointed to by src to the object pointed to 
  /// by dest. Both objects are reinterpreted as arrays of unsigned char.
  /// Returns dest.
  // ------------------------------------------------------------------------------
  void* memcpy(void* dest, const void* src, size_t count);

  template<typename T, typename... U>
  T make_number(U...);

  template<uint16, uint8, uint8>
  uint16 make_number(uint8 hi, uint8 lo)
  {
    return static_cast<uint16>((static_cast<uint16>(hi) << 8U) | static_cast<uint16>(lo));
  }
  
  // ------------------------------------------------------------------------------
  /// Returns the smaller value of the given values.
  // ------------------------------------------------------------------------------
  template<class T>
  const T& min(const T& a, const T& b) 
  { 
    return (a < b) ? a : b; 
  }

  // ------------------------------------------------------------------------------
  /// Returns the greater value of the given values.
  // ------------------------------------------------------------------------------
  template<class T>
  const T& max(const T& a, const T& b) 
  { 
    return (a > b) ? a : b; 
  }

  // ------------------------------------------------------------------------------
  /// Returns s1 + s2 and limits the sum to max value of T
  // ------------------------------------------------------------------------------
  template<typename T>
  T add_no_overflow(T s1, T s2)
  {
    T ret = s1 + s2;

    if (ret < s1)
    {
      // overflow
      ret = platform::numeric_limits<T>::max_();
    }

    return ret;
  }
} // namespace util

#endif // ALGORITHM_H_
