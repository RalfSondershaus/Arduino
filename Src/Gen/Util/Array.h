/**
  * @file Array.h
  *
  * @author Ralf Sondershaus
  *
  * An Array class
  */

#ifndef UTIL_ARRAY_H
#define UTIL_ARRAY_H

#include <Std_Types.h>

namespace Util
{
  // ---------------------------------------------------
  /// A simple container class which supports
  /// - begin
  /// - end
  /// - size
  /// and other functions as known from STL containers
  // ---------------------------------------------------
  template<class T, int N>
  class Array
  {
  public:
    /// MemberClass
    typedef T value_type;
    /// This class
    typedef Array<T, N> This;
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

  protected:

    /// array of elements
    T elements[N];
    
  public:
    /// Constructor
    Array() = default;
    /// Destructor
    ~Array() = default;
    /// returns an iterator to the beginning
    iterator begin() { return &elements[0]; }
    const_iterator begin() const { return &elements[0]; }
    /// returns an iterator to the end
    iterator end() { return &elements[N]; }
    const_iterator end() const { &elements[N]; }
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
    /// Assigns the given value value to all elements in the container.
    void fill(const T& value) 
    {
      for (auto it = begin(); it != end(); it++)
      {
        *it = value;
      }
    }
  };

} // namespace Util

#endif // UTIL_ARRAY_H