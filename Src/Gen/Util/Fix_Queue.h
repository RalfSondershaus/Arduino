/**
  * @file Fix_Queue.h
  *
  * @author Ralf Sondershaus
  *
  * @brief The util::fix_queue class is a container adaptor similar to std::queue.
  *
  * The util::fix_queue class is a container adaptor that provides the functionality 
  * of a FIFO (first-in, first-out) data structure.
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

#ifndef UTIL_FIX_QUEUE_H
#define UTIL_FIX_QUEUE_H

#include <Std_Types.h>
#include <Util/Fix_Deque.h>      // the default Container type

namespace util
{
  // ---------------------------------------------------
  /// A container adaptor for embedded systems that provides a 
  /// FIFO data structure with
  /// 
  /// - front
  /// - back
  /// - push
  /// - pop
  /// - empty
  /// 
  /// and other functions as known from std::queue.
  /// 
  /// @typeparam T The type of the stored elements
  /// @typeparam N size of the queue
  /// @typeparam Container the type of the underlying container
  // ---------------------------------------------------
  template<class T, size_t N, class Container = util::fix_deque<T, N> >
  struct fix_queue
  {
    /// The Container type
    typedef Container container_type;
    /// value type
    using value_type = typename Container::value_type;
    /// reference type
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    /// size type
    using size_type = size_t;
    /// This class
    typedef fix_queue<value_type, N, container_type> this_type;

    /// Maximal number of elements that can be stored
    static constexpr size_type MaxSize = Container::MaxSize;

    /// stored elements
    container_type container;

    /// @name element access
    /// @{
    ///  Returns reference to the first element in the queue. 
    reference front() { return container.front(); }
    const_reference front() const { return container.front(); }
    ///  Returns reference to the last element in the queue. 
    reference back() { return container.back(); }
    const_reference back() const { return container.back(); }
    /// @}

    /// @name capacity
    /// @{
    /// Checks if the underlying container has no elements
    bool empty() const { return container.empty(); }
    /// Returns the number of elements in the underlying container
    size_type size() const { return container.size(); }
    /// Returns the maximum possible number of elements (capacity).
    constexpr size_type max_size() noexcept { return container.max_size(); }
    /// @}

    /// @name modifiers
    /// @{
    /// Pushes the given element value to the end of the queue
    void push(const value_type& val) { container.push_back(val); }
    //void push(value_type&& val) {}
    ///  Pushes a new element to the end of the queue. The element is constructed in-place
    //template<class... Args> void emplace(Args&&... args);
    /// Removes an element from the front of the queue
    void pop() { container.pop_front(); }
    ///  Exchanges the contents of the container adaptor with those of other
    //void swap(queue& other) noexcept {}
    /// @}
  };

  // ---------------------------------------------------
  /// A sort-of specialization for type bool.
  ///
  /// A container adaptor for embedded systems that provides a 
  /// FIFO data structure with
  /// 
  /// - front
  /// - back
  /// - push
  /// - pop
  /// - empty
  /// 
  /// and other functions as known from std::queue.
  /// 
  /// @typeparam T The type of the stored elements
  /// @typeparam N size of the queue
  /// @typeparam Container the type of the underlying container
  // ---------------------------------------------------
  template<size_t N, class Container = util::fix_deque_bool<N> >
  struct fix_queue_bool
  {
    /// The Container type
    using container_type = Container;
    /// value type
    using value_type = typename Container::value_type;
    /// reference type
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    /// size type
    using size_type = size_t;
    /// This class
    using this_type = typename fix_queue_bool<N, container_type>;

    /// Maximal number of elements that can be stored
    static constexpr size_type kMaxSize = Container::kMaxSize;

    /// the elements
    container_type container;

    /// @name element access
    /// @{
    ///  Returns reference to the first element in the queue. 
    reference front() { return container.front(); }
    const_reference front() const { return container.front(); }
    ///  Returns reference to the last element in the queue. 
    reference back() { return container.back(); }
    const_reference back() const { return container.back(); }
    /// @}

    /// @name capacity
    /// @{
    /// Checks if the underlying container has no elements
    bool empty() const { return container.empty(); }
    /// Returns the number of elements in the underlying container
    size_type size() const { return container.size(); }
    /// Returns the maximum possible number of elements (capacity).
    constexpr size_type max_size() noexcept { return container.max_size(); }
    /// @}

    /// @name modifiers
    /// @{
    /// Pushes the given element value to the end of the queue
    void push(const value_type& val) { container.push_back(val); }
    //void push(value_type&& val) {}
    ///  Pushes a new element to the end of the queue. The element is constructed in-place
    //template<class... Args> void emplace(Args&&... args);
    /// Removes an element from the front of the queue
    void pop() { container.pop_front(); }
    ///  Exchanges the contents of the container adaptor with those of other
    //void swap(queue& other) noexcept {}
    /// @}
  };
} // namespace util

#endif // UTIL_FIX_QUEUE_H