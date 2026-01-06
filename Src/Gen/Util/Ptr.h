/**
  * @file Ptr.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Wraps a pointer.
  *
  * @copyright Copyright 2023 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_PTR_H
#define UTIL_PTR_H

namespace util
{
  // ---------------------------------------------------
  /// A simple pointer wrapper class which supports
  /// - access
  /// - check for null pointer
  // ---------------------------------------------------
  template<class T>
  struct ptr
  {
    /// MemberClass
    typedef T element_type;
    /// This class
    typedef ptr<T> This;
    /// pointer type
    typedef T* pointer;
    /// reference type
    typedef T& reference;

    /// the pointer
    pointer pntr;

    /// construct
    ptr() : pntr{ nullptr } {}
    ptr(pointer p) : pntr{ p } {}
    ptr(const This& p) : pntr{ p.pntr } {}
    ptr(const This&& p) : pntr{p.pntr} {}

    /// destruct
    ~ptr() = default;

    /// assignment
    This& operator=(const This& p) noexcept { pntr = p.pntr; return *this; }
    This& operator=(This&& p) noexcept { pntr = p.pntr; return *this; }

    /// Returns the stored pointer
    pointer get() const noexcept { return pntr; }
    pointer operator->() const noexcept { return pntr; }

    /// Returns the stored element
    reference operator*() const noexcept { return *pntr; }

    /// checks if the stored pointer is not null
    explicit operator bool() const noexcept { return get() != nullptr; }
  };

    template<class T>
    inline bool operator==(const ptr<T>& lhs, const ptr<T>& rhs) { return lhs.get() == rhs.get(); }
    template<class T>
    inline bool operator!=(const ptr<T>& lhs, const ptr<T>& rhs) { return !(lhs == rhs); }

} // namespace util

#endif // UTIL_PTR_H