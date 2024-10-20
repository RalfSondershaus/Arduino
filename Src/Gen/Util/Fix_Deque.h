/**
  * @file Fix_Deque.h
  *
  * @author Ralf Sondershaus
  *
  * @brief util::fix_deque is an indexed sequence container that is inspired by std::deque
  *
  * util::fix_deque (double-ended queue) is an indexed sequence container that
  * allows fast insertion and deletion at both its beginning and its end.
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

#ifndef UTIL_FIX_DEQUE_H
#define UTIL_FIX_DEQUE_H

#include <Std_Types.h>

// General rules: 
//    (1) first <= last (first shall alway be less than or equal to last)
//    (2) end := last + 1
// push_front: 
//     - inserts an element to the beginning. 
//     - Means, first--. 
//     - If first has underflow, first = N-1. And because of general rule (1), last += N.
// 
// Sample with N = 8
// 
//            | ------- real -----------------|----------- virtual ----------------------|
//            | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |(8)|(9)|(10)|(11)|(12)|(13)|(14)|(15)|(16)|
// 
// construct
//            | 0 | 1 | 2 | 3 | 4 | 5 | 6 |N-1|(N)|(9)|(10)|(11)|(12)|(13)|(14)|2N-1|(2N)|
//              .   .   .   .   .   .   .   .
//                                                                                |   |
//                                                                                |   first
//                                                                                |   end
//                                                                                last
// 
// push_front(.)
//            | 0 | 1 | 2 | 3 | 4 | 5 | 6 |N-1|(N)|(9)|(10)|(11)|(12)|(13)|(14)|2N-1|(2N)
//              x   .   .   .   .   .   .   .
//              |   |
//              |   end
//              first
//              last
// 
// push_front(.)
//            | 0 | 1 | 2 | 3 | 4 | 5 | 6 |N-1|(N)|(9)|(10)|(11)|(12)|(13)|(14)|2N-1|(2N)
//              x   .   .   .   .   .   .   x
//                                          |   |   |
//                                          |   |   end
//                                          |   last
//                                          first

namespace util
{
  // ---------------------------------------------------
  /// base struct for iterators
  // ---------------------------------------------------
  template<class FixDeque>
  struct fix_deque_iterator_base
  {
  public:
    using value_type = typename FixDeque::value_type;
    using size_type = typename FixDeque::size_type;
    using pointer = typename FixDeque::pointer;

    static constexpr size_type MaxIdx = FixDeque::MaxIdx;

    typedef fix_deque_iterator_base<FixDeque> This;

  protected:
    /// Returns pos + n, considers overflows
    static size_type add_wrap(size_type pos, size_type n) { return (pos + n) % MaxIdx; }
    /// Returns pos - n, considers underflows
    static size_type sub_wrap(size_type pos, size_type n) { return (pos - n) % MaxIdx; }
    /// Current position (index) in the container
    size_type myPos;

  public:
    /// Constructor
    fix_deque_iterator_base(size_type pos = 0U) : myPos(pos) {}

    /// @name Increment / decrement
    /// @{
    /// prefix increment / decrement
    This& operator++() noexcept { myPos = add_wrap(myPos, 1U); return *this; }
    This& operator--() noexcept { myPos = sub_wrap(myPos, 1U); return *this; }
    /// postfix increment / decrement
    This operator++(int) noexcept { This old = *this; operator++(); return old; }
    This operator--(int) noexcept { This old = *this; operator--(); return old; }
    /// @}

    /// @name Compare
    /// @{
    /// compare
    friend bool operator!=(const This& it1, const This& it2) noexcept { return !(it1 == it2); }
    friend bool operator==(const This& it1, const This& it2) noexcept { return it1.myPos == it2.myPos; }
    /// @}
  };

  // ---------------------------------------------------
  /// iterators (non const)
  // ---------------------------------------------------
  template<class FixDeque>
  struct fix_deque_iterator : public fix_deque_iterator_base<FixDeque>
  {
  public:
    typedef fix_deque_iterator<FixDeque> This;
    typedef fix_deque_iterator_base<FixDeque> Base;

    using reference = typename FixDeque::reference;
    using size_type = typename Base::size_type;

  protected:
    FixDeque& myDeque;

  public:
    /// Constructor
    fix_deque_iterator(FixDeque& deque, size_type pos = 0U) : Base(pos), myDeque(deque) {}

    /// @name Element access
    /// @{
    reference operator*() noexcept { return myDeque.at(Base::myPos); }
    /// @}

    /// @name Increment / decrement
    /// @{
    /// postfix increment / decrement
    This operator++(int) noexcept { This old = *this; Base::operator++(); return old; }
    This operator--(int) noexcept { This old = *this; Base::operator--(); return old; }
    /// @}
  };

  // ---------------------------------------------------
  /// const iterators
  // ---------------------------------------------------
  template<class FixDeque>
  struct fix_deque_iterator_const : public fix_deque_iterator_base<FixDeque>
  {
  public:
    typedef fix_deque_iterator_const<FixDeque> This;
    typedef fix_deque_iterator_base<FixDeque> Base;

    using const_reference = typename FixDeque::const_reference;
    using size_type = typename Base::size_type;

  protected:
    FixDeque& myDeque; // "FixDeque" is of type "const fix_deque<T,N>"

  public:
    /// Constructor
    fix_deque_iterator_const(FixDeque& deque, size_type pos = 0U) : Base(pos), myDeque(deque) {}

    /// @name Element access
    /// @{
    const_reference operator*() const noexcept { return myDeque.at(Base::myPos); }
    /// @}

    /// @name Increment / decrement
    /// @{
    /// postfix increment / decrement
    This operator++(int) noexcept { This old = *this; Base::operator++(); return old; }
    This operator--(int) noexcept { This old = *this; Base::operator--(); return old; }
    /// @}
  };

  // ---------------------------------------------------
  /// An indexed sequence container that allows fast insertion and
  /// deletion at its beginning and its end.
  /// 
  /// Insertion or deletion never invalidates pointers or references
  /// to the rest of the elements.
  /// 
  /// In contrast to the std::deque specification, a util::fix_deque has a
  /// fixed storage size. The storage is not automatically expanded or
  /// contracted.
  /// 
  /// @typeparam T: type of the elements to be stored
  /// @typeparam N: the size of the storage (number of elements)
  // ---------------------------------------------------
  template<class T, size_t N>
  class fix_deque
  {
  public:
    /// Type of the elements to be stored
    typedef T value_type;
    /// This class
    typedef fix_deque<T, N> This;
    /// size type
    typedef size_t size_type;
    /// pointer type
    typedef T * pointer;
    typedef const T * const_pointer;
    /// reference type
    typedef T& reference;
    typedef const T& const_reference;

    /// iterator type
    using iterator = fix_deque_iterator<This>;
    using const_iterator = fix_deque_iterator_const<const This>;

  public:
    static constexpr size_type MaxSize = N;
    static constexpr size_type MaxIdx = 2U * MaxSize;
    static constexpr size_type One = 1U;
    static constexpr size_type Zero = 0U;

    /// Constructor
    fix_deque() : unFirst(MaxIdx), unSize(Zero) {}
    /// Destructor
    ~fix_deque() = default;

    /// @name Element access
    /// @{
    /// Returns a reference to the element at specified location without range check for pos.
    /// Note: std::deque performs a range check here. If the range check fails, an exception
    /// is thrown. We avoid exceptions and perform a modulo operation instead. So the return
    /// value is defined by the modulo operation if pos is out of range.
    /// Check range of pos before calling this function if demanded.
    reference at(size_type pos)             { return aElements[map_idx_N_1(unFirst + pos)]; }
    const_reference at(size_type pos) const { return aElements[map_idx_N_1(unFirst + pos)]; }

    /// Returns a reference to the element at specified location without range check for pos.
    reference operator[](size_type pos)             { return aElements[map_idx_N_1(unFirst + pos)]; }
    const_reference operator[](size_type pos) const { return aElements[map_idx_N_1(unFirst + pos)]; }

    /// Access the first element.
    /// Returns a reference to the first element in the container.
    /// Calling front on an empty container causes undefined behavior.
    reference front()             { return aElements[unFirst]; }
    const_reference front() const { return aElements[unFirst]; }

    /// Access the last element.
    /// Returns a reference to the last element in the container.
    /// Calling back on an empty container causes undefined behavior.
    reference back()              { return aElements[map_idx_N_1(last_idx())]; }
    const_reference back() const  { return aElements[map_idx_N_1(last_idx())]; }
    /// @}

    /// @name Iterators
    /// @{
    /// returns an iterator to the beginning
    iterator begin() noexcept { return iterator(*this, Zero); }
    const_iterator begin() const noexcept { return const_iterator(*this, Zero); }
    const_iterator cbegin() const noexcept { return const_iterator(*this, Zero); }

    /// returns an iterator to the end
    iterator end() noexcept { return iterator(*this, size()); }
    const_iterator end() const noexcept { return const_iterator(*this, size()); }
    const_iterator cend() const noexcept { return const_iterator(*this, size()); }

    /// returns an reverse iterator to the beginning
    iterator rbegin() noexcept {}
    const_iterator rbegin() const noexcept {}
    const_iterator crbegin() const noexcept {}

    /// returns an reverse iterator to the end
    iterator rend() noexcept {}
    const_iterator rend() const noexcept {}
    const_iterator crend() const noexcept {}
    /// @}

    /// @name Capacity
    /// @{
    /// checks whether the container is empty
    bool empty() const noexcept { return unSize == Zero; }
    ///  returns the number of elements
    size_type size() const noexcept { return unSize; }
    /// returns the maximum possible number of elements
    /// Note: std::deque::max_size is not constexpr but we can make max_size constexpr
    /// because it just returns a template parameter (no dynamic allocation)
    constexpr size_type max_size() noexcept { return MaxSize; }
    /// @}

    /// @name Modifiers
    /// @{
    /// clears the contents
    void clear() { unFirst = MaxIdx; unSize = Zero; }
    /// inserts elements before pos
    iterator insert(const_iterator pos, const T& value) {}
    iterator insert(const_iterator pos, T&& value) {}
    iterator insert(const_iterator pos, size_type count, const T& value) {}

    /// erases elements
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    /// Adds an element to the end if space is available.
    /// Does nothing if space is not available anymore
    /// (can be checked with size() < max_size() before).
    void push_back(const T& value)
    {
      if (size() < max_size())
      {
        if (empty())
        {
          unFirst = Zero;
        }
        unSize++;
        back() = value;
      }
    }

    // not implemented yet: void push_back(T&& value);

    /// removes the last element
    void pop_back()
    {
      if (unSize > Zero)
      {
        unSize--;

        if (unSize == Zero)
        {
          // last element removed
          unFirst = MaxIdx;
        }
      }
    }
    /// inserts an element to the beginning
    void push_front(const T& value)
    {
      if (size() < max_size())
      {
        if (empty())
        {
          unFirst = Zero;
        }
        else
        {
          if (unFirst == Zero)
          {
            unFirst = MaxSize - One;
          }
          else
          {
            unFirst--;
          }
        }
        unSize++;
        front() = value;
      }
    }

    // not implemented yet: void push_front(T&& value);

    /// removes the first element
    void pop_front()
    {
      if (size() > Zero)
      {
        unSize--;

        if (size() == Zero)
        {
          // last element removed
          unFirst = MaxIdx;
        }
        else
        {
          unFirst = map_idx_N_1(unFirst + One);
        }
      }
    }
    /// @}

  protected:
    /// The array of elements
    value_type aElements[N];
    /// Index of the first element (0 ... N-1; 2N after construct)
    size_type unFirst;
    /// Number of elements (0 ... N-1)
    size_type unSize;
    /// Index of the last element (0 ... 2N-1)
    size_type last_idx() const noexcept { return static_cast<size_type>((unFirst + unSize) - One); }
    /// Index to the element after last (0 ... 2N)
    size_type end_idx() const noexcept { return static_cast<size_type>(last_idx() + One); }
    /// Fold an index back into range [0 ... N-1]
    size_type map_idx_N_1(size_type pos) const noexcept { return pos % MaxSize; }
  };

} // namespace Util

#endif // UTIL_FIX_DEQUE_H