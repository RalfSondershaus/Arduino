/**
  * @file Vector.h
  *
  * @author Ralf Sondershaus
  *
  * A Vector class
  */

#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

#include <Platform_Types.h>
#include <Util/Algorithm.h>
#include <Util/Utility.h>

namespace util
{
  // ---------------------------------------------------
  /// Holds N elements of type T
  // ---------------------------------------------------
  template<class T, int N>
  class static_allocator
  {
  protected:
    T arr[N];
  public:
    typedef T           value_type;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;
    typedef T*          pointer;
    typedef const T*    const_pointer;

    static constexpr size_type kMaxSize { N };

    /// @brief Allocates continuous memory for n elements of type T, calls constructors
    /// @param n Number of elements
    /// @return Pointer to the first element, or nullptr if n is greater than the maximal number
    ///         of elements that can be allocated
    T * allocate(size_type n)
    {
      T * p;
      if (n < kMaxSize)
      {
        p = &arr[0];
      }
      else
      {
        p = nullptr;
      }
      return p;
    }

    /// @brief Deallocates the memory at p, calls destructor of each element
    /// @param p Pointer to the memory to be deallocated
    /// @param n Number of elements in the memory
    void deallocate(T * p, size_type n)
    {}

    /// Returns the maximum possible number of elements
    size_type max_size() const noexcept { return kMaxSize; }

    /// @brief Constructs an object of type T in allocated uninitialized memory pointed
    /// to by p, using placement-new.
    /// @tparam U Memory type, typ. T
    /// @tparam Args 
    /// @param p Pointer to uninitialized memory of type U
    /// @param args 
    template<class U, class... Args>
    void construct(U* p, Args&&... args)
    {
      // Should be new((void *)p) U(util::forward<Args>(args)...)
      new((void *)p) U(args...);
    }
  };

  // ---------------------------------------------------
  /// A simple container class which supports
  /// - begin
  /// - end
  /// - size
  /// - push_back (max number of elements is limited, no dynamic memory allocation)
  /// and other functions as known from STL containers
  // ---------------------------------------------------
  template<class T, class Allocat = static_allocator<T, 16> >
  class vector
  {
  public:
    /// MemberClass
    typedef T value_type;
    /// This class
    typedef vector<T, Allocat> This;
    /// The allocator
    typedef Allocat allocator;
    /// iterators
    typedef T *       iterator;
    typedef const T * const_iterator;
    /// references
    typedef T&        reference;
    typedef const T&  const_reference;
    /// types from allocator
    using size_type = typename Allocat::size_type;
    using difference_type = typename Allocat::difference_type;
    using pointer = typename Allocat::pointer;
    using const_pointer = typename Allocat::const_pointer;

  protected:
    /// points to the first element
    pointer first;
    /// points to one element past the last (used) element
    /// points to first if the container is empty
    pointer last;
    /// points to one element after the end of the buffer
    pointer last_storage;
    /// allocates memory
    allocator alloc;

    /// Return pointer to the first element
    pointer get_first()               { return first; }
    const_pointer get_first() const   { return first; }

    /// Return pointer to the last used element
    pointer get_back()              { return (last > first) ? (last-1) : (last); }
    const_pointer get_back() const  { return (last > first) ? (last-1) : (last); }

    /// Return pointer to the last element (one after last used element)
    pointer get_last()              { return last; }
    const_pointer get_last() const  { return last; }

    /// Return pointer to one element after the last element of the buffer
    pointer get_last_storage()              { return last_storage; }
    const_pointer get_last_storage() const  { return last_storage; }

    /// Returns new capacity which is greater than (or equal to) newsize and smaller than max_size()
    size_type calculate_new_capacity(const size_type newsize) const
    {
      size_type newcap;
      const size_type cap = capacity();
      const size_type geo = cap + cap / static_cast<size_type>(2);
      if (geo < newsize)
      {
        newcap = newsize;
      }
      else
      {
        newcap = geo;
      }
      if (newcap > max_size())
      {
        newcap = max_size();
      }
      return newcap;
    }

    /// Update internal data to reflect the new first and the new size
    void change_mem(pointer newfirst, const size_type newsize, const size_type newcapacity)
    {
      if (newfirst != first)
      {
        alloc.deallocate(first, size());
        first = newfirst;
        last = newfirst + newsize;
        last_storage = newfirst + newcapacity;
      }
    }
    /// Returns true if memory is available to store n new elements
    bool has_unused_capacity(difference_type n) const noexcept
    {
      return last + n <= last_storage;
    }

    /// Tries to reallocate memory.
    /// Iterators are invalid afterwards if new memory was allocated successfully.
    void realloc()
    {
      const size_type cursize = size();
      const size_type newcap = calculate_new_capacity(cursize + 1);
      pointer newfirst = alloc.allocate(newcap);
      if (newfirst != nullptr)
      {
        util::copy(get_first(), get_last(), newfirst);
        change_mem(newfirst, cursize, newcap);
      }
    }

  public:
    /// Constructor
    vector()
    {
      const size_type initsize = 2;
      first = alloc.allocate(initsize);
      last = first;
      last_storage = first + initsize;
    }

    /// Constructor for n elements
    explicit vector(size_type initsize)
    {
      first = alloc.allocate(initsize);
      last = first;
      last_storage = first + initsize;
    }

    /// Destructor
    ~vector()
    {
      alloc.deallocate(first, capacity());
    }

    // Iterators

    /// begin
    iterator begin() { return get_first(); }
    const_iterator begin() const { return get_first(); }
    /// end
    iterator end() { return get_last(); }
    const_iterator end() const { get_last(); }

    // Element access

    /// Returns a reference to the element at the specified position, without bounds checking
    /// (C++ standard library performs bounds checking here)
    reference at(size_type pos) { return begin() + pos; }
    const_reference at(size_type pos) const { return begin() + pos; }
    /// Returns a reference to the element at the specified position, without bounds checking
    reference operator[](size_type pos) { return begin() + pos; }
    const_reference operator[](size_type pos) const { return begin() + pos; }
    /// Returns a reference to the first element in the container.
    reference front() { return *get_first(); }
    const_reference front() const { return *get_first(); }
    /// Returns a reference to the last element in the container.
    reference back() { return *get_back(); }
    const_reference back() const { return *get_back(); }
    /// Returns the underlying contiguous storage
    pointer data() { return get_first(); }
    const_pointer data() const { return get_first(); }

    // Capacity

    /// Returns the number of elements
    size_type size() const { return static_cast<size_type>(get_last() - get_first()); }
    /// Returns the maximum possible number of elements
    size_type max_size() const noexcept { return alloc.max_size(); }
    /// Returns size of allocated memory
    size_type capacity() const noexcept { return static_cast<size_type>(get_last_storage() - get_first()); }
    /// Returns true if the container is empty
    bool empty() const { return get_last() == get_first(); }

    // Modifiers

    /// @brief Inserts a new element into the container directly before pos
    /// @param pos The position where the element is inserted
    /// @param ...args Arguments passed to the constructor of the new element
    /// @return Iterator pointing to the emplaced element
    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
      if (!has_unused_capacity(1))
      {
        realloc();
      }
      if (has_unused_capacity(1))
      {
        util::copy(pos, end(), pos + 1);
        alloc.construct(pos, args...);
      }
      return first + (pos - first);
    }
    /// @brief Appends a new element to the end of this vector.
    /// The element is constructed through placement-new.
    /// @param args Parameters that are passed on to the constructor
    template<class... Args>
    void emplace_back(Args&&... args)
    {
      if (!has_unused_capacity(1))
      {
        realloc();
      }
      if (has_unused_capacity(1))
      {
        alloc.construct(last, args...);
        last++;
      }
      // else: no memory available
    }
    /// push back
    void push_back(const T& v)
    {
      if (!has_unused_capacity(1))
      {
        realloc();
      }
      if (has_unused_capacity(1))
      {
        *last++ = v;
      }
      // else: no memory available
    }
    /// @brief Appends a new element to the end of the vector.
    /// @param v Value that is moved to the new element
    void push_back(T&& v)
    {
      if (!has_unused_capacity(1))
      {
        realloc();
      }
      if (has_unused_capacity(1))
      {
        *last++ = util::forward<T>(v);
      }
      // else: no memory available
    }
    /// Erase all elements from the vector, size() returns 0 afterwards. Iterators are invalid.
    /// capacity() is not changed.
    void clear()
    {
      last = first;
    }
    /// Inserts v at the specified position
    iterator insert(const_iterator pos, const T& v)
    {
      if (has_unused_capacity())
      {
        move(pos, get_last(), get_first() + 1);
      }
    }
  };

} // namespace Util

#endif // UTIL_VECTOR_H