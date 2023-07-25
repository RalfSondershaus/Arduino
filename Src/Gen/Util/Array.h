/**
  * @file Array.h
  *
  * @author Ralf Sondershaus
  *
  * @brief An array class.
  *
  * @copyright Copyright 2018 - 2022 Ralf Sondershaus
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
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef UTIL_ARRAY_H
#define UTIL_ARRAY_H

#include <Std_Types.h>

namespace util
{
  // ---------------------------------------------------
  /// A simple container class which supports
  /// - begin
  /// - end
  /// - size
  /// and other functions as known from STL containers
  // ---------------------------------------------------
  template<class T, int N>
  struct array
  {
    /// MemberClass
    typedef T value_type;
    /// This class
    typedef array<T, N> This;
    /// iterators
    typedef T *       iterator;
    typedef const T * const_iterator;
    /// size type
    using size_type = size_t;
    /// pointer type
    typedef T * pointer;
    typedef const T * const_pointer;
    /// reference type
    typedef T& reference;
    typedef const T& const_reference;

    /// Integral template parameters
    static constexpr int kNrElements = N;

    /// array of elements
    T elements[N];
    
    /// Constructor
    /// returns an iterator to the beginning
    iterator begin() { return &elements[0]; }
    const_iterator begin() const { return &elements[0]; }
    /// returns an iterator to the end
    iterator end() { return &elements[N]; }
    const_iterator end() const { return &elements[N]; }
    /// Returns a reference to the element at specified location pos. No bounds checking is performed (contrary to STL)
    reference at(size_type pos) { return elements[pos]; }
    constexpr const_reference at(size_type pos) const { return elements[pos]; }
    /// Returns a reference to the element at specified location pos. No bounds checking is performed.
    reference operator[](size_type pos) { return elements[pos]; }
    constexpr const_reference operator[](size_type pos) const { return elements[pos]; }
    /// Returns a reference to the first element in the container. Calling front on an empty container is undefined.
    reference front() { return elements[0]; }
    constexpr const_reference front() const { return elements[0]; }
    /// Returns a reference to the last element in the container. Calling back on an empty container causes undefined behavior.
    reference back() { return elements[N-1]; }
    constexpr const_reference back() const { return elements[N-1]; }
    /// Returns pointer to the underlying array serving as element storage.
    pointer data() noexcept { return &elements[0]; }
    constexpr const_pointer data() const noexcept { return &elements[0]; }
    /// Returns the maximum possible number of elements
    constexpr size_type max_size() const noexcept { return N; }
    /// Returns the number of elements
    constexpr size_type size() const noexcept { return N; }
    /// Return true if the index is valid (within boundaries)
    constexpr bool check_boundary(size_type pos) { return (pos >= static_cast<size_type>(0)) && (pos < size()); }
    /// Assigns the given value value to all elements in the container.
    void fill(const T& value) 
    {
      for (auto it = begin(); it != end(); it++)
      {
        *it = value;
      }
    }
  };

} // namespace util

#endif // UTIL_ARRAY_H