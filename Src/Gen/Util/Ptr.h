/**
  * @file Ptr.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Wraps a pointer.
  *
  * @copyright Copyright 2023 Ralf Sondershaus
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

} // namespace util

#endif // UTIL_PTR_H