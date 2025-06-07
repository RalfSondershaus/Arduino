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
#include <Util/bitset.h> // for fix_deque_bool

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
  /**
   * @brief Base struct for fix_deque iterators.
   *
   * Provides common functionality for iterator types used by util::fix_deque,
   * including position tracking, increment/decrement operations, and comparison.
   *
   * @tparam FixDeque The fix_deque type for which this iterator is defined.
   */
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
    /// @brief Returns pos + n, wrapped around MaxIdx.
    static size_type add_wrap(size_type pos, size_type n) { return (pos + n) % MaxIdx; }
    /// @brief Returns pos - n, wrapped around MaxIdx.
    static size_type sub_wrap(size_type pos, size_type n) { return (pos - n) % MaxIdx; }
    /// @brief Current position (index) in the container.
    size_type myPos;

  public:
    /// @brief Constructs an iterator at the given position.
    fix_deque_iterator_base(size_type pos = 0U) : myPos(pos) {}

    /// @name Increment / decrement
    /// @{
    /// @brief Prefix increment.
    This& operator++() noexcept { myPos = add_wrap(myPos, 1U); return *this; }
    /// @brief Prefix decrement.
    This& operator--() noexcept { myPos = sub_wrap(myPos, 1U); return *this; }
    /// @brief Postfix increment.
    This operator++(int) noexcept { This old = *this; operator++(); return old; }
    /// @brief Postfix decrement.
    This operator--(int) noexcept { This old = *this; operator--(); return old; }
    /// @}

    /// @name Compare
    /// @{
    /// @brief Checks if two iterators are not equal.
    friend bool operator!=(const This& it1, const This& it2) noexcept { return !(it1 == it2); }
    /// @brief Checks if two iterators are equal.
    friend bool operator==(const This& it1, const This& it2) noexcept { return it1.myPos == it2.myPos; }
    /// @}
  };

  // ---------------------------------------------------
  /**
   * @brief Non-const iterator for util::fix_deque.
   *
   * Allows modification of elements in the fix_deque.
   *
   * @tparam FixDeque The fix_deque type for which this iterator is defined.
   */
  template<class FixDeque>
  struct fix_deque_iterator : public fix_deque_iterator_base<FixDeque>
  {
  public:
    typedef fix_deque_iterator<FixDeque> This;
    typedef fix_deque_iterator_base<FixDeque> Base;

    using reference = typename FixDeque::reference;
    using size_type = typename Base::size_type;

  protected:
    /// @brief Reference to the underlying fix_deque.
    FixDeque& myDeque;

  public:
    /// @brief Constructs an iterator for the given deque at the specified position.
    fix_deque_iterator(FixDeque& deque, size_type pos = 0U) : Base(pos), myDeque(deque) {}

    /// @name Element access
    /// @{
    /// @brief Dereferences the iterator to access the element.
    reference operator*() noexcept { return myDeque.at(Base::myPos); }
    /// @}

    /// @name Increment / decrement
    /// @{
    /// @brief Postfix increment.
    This operator++(int) noexcept { This old = *this; Base::operator++(); return old; }
    /// @brief Postfix decrement.
    This operator--(int) noexcept { This old = *this; Base::operator--(); return old; }
    /// @}
  };

  /**
   * @brief Const iterator for util::fix_deque.
   *
   * Provides read-only access to elements in the fix_deque.
   *
   * @tparam FixDeque The fix_deque type for which this iterator is defined.
   */
  template<class FixDeque>
  struct fix_deque_iterator_const : public fix_deque_iterator_base<FixDeque>
  {
  public:
    typedef fix_deque_iterator_const<FixDeque> This;
    typedef fix_deque_iterator_base<FixDeque> Base;

    using const_reference = typename FixDeque::const_reference;
    using size_type = typename Base::size_type;

  protected:
    /// @brief Reference to the underlying fix_deque (const).
    FixDeque& myDeque; // "FixDeque" is of type "const fix_deque<T,N>"

  public:
    /// @brief Constructs a const iterator for the given deque at the specified position.
    fix_deque_iterator_const(FixDeque& deque, size_type pos = 0U) : Base(pos), myDeque(deque) {}

    /// @name Element access
    /// @{
    /// @brief Dereferences the iterator to access the element (read-only).
    const_reference operator*() const noexcept { return myDeque.at(Base::myPos); }
    /// @}

    /// @name Increment / decrement
    /// @{
    /// @brief Postfix increment.
    This operator++(int) noexcept { This old = *this; Base::operator++(); return old; }
    /// @brief Postfix decrement.
    This operator--(int) noexcept { This old = *this; Base::operator--(); return old; }
    /// @}
  };

  // ---------------------------------------------------
  /**
   * @brief Fixed-size double-ended queue (deque) container.
   *
   * An indexed sequence container that allows fast insertion and deletion at both its beginning and its end.
   * Insertion or deletion never invalidates pointers or references to the remaining elements.
   * Unlike std::deque, util::fix_deque has a fixed storage size that is not automatically expanded or contracted.
   *
   * @tparam T Type of the elements to be stored.
   * @tparam N The size of the storage (number of elements).
   */
  template<class T, size_t N>
  class fix_deque
  {
  public:
    /// @brief Type of the elements to be stored.
    using value_type = T;
    /// @brief This class type.
    using This = fix_deque<T, N>;
    /// @brief Type used for sizes and indices.
    using size_type = size_t;
    /// @brief Pointer type.
    using pointer = T *;
    using const_pointer = const T *;
    /// @brief Reference type.
    using reference = T&;
    using const_reference = const T&;

    /// @brief Iterator type.
    using iterator = fix_deque_iterator<This>;
    /// @brief Const iterator type.
    using const_iterator = fix_deque_iterator_const<const This>;

  public:
    static constexpr size_type MaxSize = N;
    static constexpr size_type MaxIdx = 2U * MaxSize;
    static constexpr size_type One = 1U;
    static constexpr size_type Zero = 0U;

    /// @brief Constructs an empty deque.
    fix_deque() : unFirst(MaxIdx), unSize(Zero) {}
    /// @brief Destructor.
    ~fix_deque() = default;

    /// @name Element access
    /// @{
    /// @brief Returns a reference to the element at specified location without range check for pos.
    /// @param pos Index position (0-based).
    /// @return Reference to the element at the given position.
    /// @note If pos is out of range, the result is defined by modulo operation.
    reference at(size_type pos)             { return aElements[map_idx_N_1(unFirst + pos)]; }
    const_reference at(size_type pos) const { return aElements[map_idx_N_1(unFirst + pos)]; }

    /// @brief Returns a reference to the element at specified location without range check for pos.
    reference operator[](size_type pos)             { return aElements[map_idx_N_1(unFirst + pos)]; }
    const_reference operator[](size_type pos) const { return aElements[map_idx_N_1(unFirst + pos)]; }

    /// @brief Returns a reference to the first element in the container.
    /// @warning Calling front on an empty container causes undefined behavior.
    reference front()             { return aElements[unFirst]; }
    const_reference front() const { return aElements[unFirst]; }

    /// @brief Returns a reference to the last element in the container.
    /// @warning Calling back on an empty container causes undefined behavior.
    reference back()              { return aElements[map_idx_N_1(last_idx())]; }
    const_reference back() const  { return aElements[map_idx_N_1(last_idx())]; }
    /// @}

    /// @name Iterators
    /// @{
    /// @brief Returns an iterator to the beginning.
    iterator begin() noexcept { return iterator(*this, Zero); }
    const_iterator begin() const noexcept { return const_iterator(*this, Zero); }
    const_iterator cbegin() const noexcept { return const_iterator(*this, Zero); }

    /// @brief Returns an iterator to the end.
    iterator end() noexcept { return iterator(*this, size()); }
    const_iterator end() const noexcept { return const_iterator(*this, size()); }
    const_iterator cend() const noexcept { return const_iterator(*this, size()); }

    /// @brief Returns a reverse iterator to the beginning (not implemented).
    iterator rbegin() noexcept = delete;
    const_iterator rbegin() const noexcept = delete;
    const_iterator crbegin() const noexcept = delete;

    /// @brief Returns a reverse iterator to the end (not implemented).
    iterator rend() noexcept = delete;
    const_iterator rend() const noexcept = delete;
    const_iterator crend() const noexcept = delete;
    /// @}

    /// @name Capacity
    /// @{
    /// @brief Checks whether the container is empty.
    /// @return true if the container is empty, false otherwise.
    bool empty() const noexcept { return unSize == Zero; }
    /// @brief Returns the number of elements currently stored.
    /// @return The number of elements in the container.
    size_type size() const noexcept { return unSize; }
    /// @brief Returns the maximum possible number of elements.
    /// @return The maximum capacity of the container.
    /// @note Unlike std::deque, this is constexpr because storage is fixed.
    constexpr size_type max_size() noexcept { return MaxSize; }
    /// @}

    /// @name Modifiers
    /// @{
    /// @brief Clears the contents of the container.
    void clear() { unFirst = MaxIdx; unSize = Zero; }
    /// @brief Inserts elements before pos (not implemented).
    iterator insert(const_iterator pos, const T& value) {}
    iterator insert(const_iterator pos, T&& value) {}
    iterator insert(const_iterator pos, size_type count, const T& value) {}

    /// @brief Erases elements (not fully implemented).
    iterator erase(const_iterator pos);
    iterator erase(const_iterator first, const_iterator last);

    /// @brief Adds an element to the end if space is available.
    /// @param value The value to add.
    /// @note If the container is full, the operation does nothing.
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

    /// @brief Removes the last element.
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
    /// @brief Inserts an element to the beginning.
    /// @param value The value to insert.
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

    /// @brief Removes the first element.
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
    /// @brief The array of elements.
    value_type aElements[N];
    /// @brief Index of the first element (0 ... N-1; 2N after construct).
    size_type unFirst;
    /// @brief Number of elements (0 ... N-1).
    size_type unSize;
    /// @brief Index of the last element (0 ... 2N-1).
    size_type last_idx() const noexcept { return static_cast<size_type>((unFirst + unSize) - One); }
    /// @brief Index to the element after last (0 ... 2N).
    size_type end_idx() const noexcept { return static_cast<size_type>(last_idx() + One); }
    /// @brief Fold an index back into range [0 ... N-1].
    size_type map_idx_N_1(size_type pos) const noexcept { return pos % MaxSize; }
  };

  // ---------------------------------------------------
  /**
   * @brief Fixed-size double-ended queue (deque) specialization for bool values.
   *
   * This class is a specialization of util::fix_deque for efficiently storing and manipulating
   * a sequence of boolean values as bits. It provides fast insertion and deletion at both the
   * beginning and the end of the container, similar to std::deque, but with a fixed storage size.
   *
   * - The storage is not automatically expanded or contracted.
   * - Insertion or deletion never invalidates pointers or references to the remaining elements.
   * - The container uses a bitset for compact storage of boolean values.
   *
   * @tparam N The number of bits (elements) to be stored in the container.
   */
  template<size_t N>
  class fix_deque_bool
  {
  public:
    /// @brief Type of the elements to be stored (always bool).
    typedef bool value_type;
    /// @brief Type alias for this class.
    typedef fix_deque_bool<N> This;
    /// @brief Type used for sizes and indices.
    typedef size_t size_type;

    /// @brief Underlying word type for the bitset.
    using bitset_base_type = uint32;
    /// @brief Bitset type used for storage.
    using bitset_type = bitset<uint32, N>;

  public:
    /// @brief Maximum number of elements the container can hold.
    static constexpr size_type kMaxSize = N;
    /// @brief Maximum index value (equal to kMaxSize).
    static constexpr size_type MaxIdx = kMaxSize;

    /// @brief Constructs an empty deque.
    fix_deque_bool() : front_idx(0), back_idx(-1), nr_elements(0) {}

    /// @brief Destructor.
    ~fix_deque_bool() = default;

    /// @name Element access
    /// @{
    /// @brief Returns the value at the specified position (no range check).
    /// @param pos Index position (0-based).
    /// @return Value at the given position.
    /// @note If pos is out of range, the result is defined by modulo operation.
    value_type at(size_type pos) const { return bits[(front_idx + pos) % kMaxSize]; }

    /// @brief Returns the value at the specified position (no range check).
    /// @param pos Index position (0-based).
    /// @return Value at the given position.
    value_type operator[](size_type pos) const { return at(pos); }

    /// @brief Returns the value of the first element in the container.
    /// @return Value of the first element.
    /// @warning Calling on an empty container is undefined behavior.
    value_type front() const { return bits[front_idx]; }

    /// @brief Returns the value of the last element in the container.
    /// @return Value of the last element.
    /// @warning Calling on an empty container is undefined behavior.
    value_type back() const  { return bits[back_idx]; }
    /// @}

    /// @name Capacity
    /// @{
    /// @brief Checks whether the container is empty.
    /// @return true if the container is empty, false otherwise.
    bool empty() const noexcept { return nr_elements == 0; }

    /// @brief Returns the number of elements currently stored.
    /// @return The number of elements in the container.
    size_type size() const noexcept { return nr_elements; }

    /// @brief Returns the maximum number of elements the container can hold.
    /// @return The maximum capacity of the container.
    /// @note Unlike std::deque, this is constexpr because storage is fixed.
    constexpr size_type max_size() noexcept { return kMaxSize; }
    /// @}

    /// @name Modifiers
    /// @{
    /// @brief Removes all elements from the container.
    void clear() { front_idx = 0; back_idx = -1; nr_elements = 0; }

    /// @brief Adds an element to the end if space is available.
    /// @param value The value to add.
    /// @note If the container is full, the operation does nothing.
    void push_back(const bool value)
    {
      if (size() < max_size())
      {
        back_idx = (back_idx + 1) % kMaxSize;
        nr_elements++;
        bits.set(back_idx, value);
      }
    }

    // not implemented yet: void push_back(T&& value);

    // removes the last element
    // not implemented yet
    // void pop_back()

    // inserts an element to the beginning
    // not implemented yet
    // void push_front(const T& value)

    // not implemented yet: void push_front(T&& value);

    /// @brief Removes the first element from the container.
    /// @note If the container is empty, the operation does nothing.
    void pop_front()
    {
      if (!empty())
      {
        front_idx = (front_idx + 1) % kMaxSize;
        nr_elements--;
      }
    }
    /// @}

  protected:
    /// @brief Bitset storage for the boolean values.
    bitset_type bits;
    /// @brief Index of the first element (returned and removed by pop()).
    size_type front_idx;
    /// @brief Index of the last element (added by push()).
    size_type back_idx;
    /// @brief Number of elements currently in the container.
    size_type nr_elements;
  };
} // namespace util

#endif // UTIL_FIX_DEQUE_H