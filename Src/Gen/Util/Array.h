/**
  * @file Array.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Provides an array class similar to STL std::array.
  *
  * @copyright Copyright 2018 - 2022 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_ARRAY_H
#define UTIL_ARRAY_H

#include <Std_Types.h>
#include <Util/Algorithm.h>

namespace util
{
  // ---------------------------------------------------
  /// Returns a pointer to the beginning of array
  // ---------------------------------------------------
  template <class T, size_t N>
  constexpr T* begin(T(&arr)[N]) noexcept
  {
    return arr;
  }

  // ---------------------------------------------------
  /// Returns a pointer to the end of array
  // ---------------------------------------------------
  template <class T, size_t N>
  constexpr T* end(T(&arr)[N]) noexcept
  {
    return &arr[N];
  }

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
    constexpr bool check_boundary(size_type pos) const { return pos < size(); }
    /// Assigns the given value value to all elements in the container.
    void fill(const T& value) { util::fill(begin(), end(), value); }
  };

} // namespace util

#endif // UTIL_ARRAY_H