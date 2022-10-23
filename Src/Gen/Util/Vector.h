/**
  * @file Vector.h
  *
  * @author Ralf Sondershaus
  *
  * A Vector class
  */

#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

namespace Util
{
  // ---------------------------------------------------
  /// Provides a fixed number of elements
  // ---------------------------------------------------
  template<class T, int N>
  class StaticAllocator
  {
  protected:
    T mArray[N];
  public:
    typedef T value_type;
    typedef size_t size_type;

    T * allocate(size_type n)
    {
      (void) n;
      return &mArray[0];
    }
    void deallocate(T * p, size_type n)
    {}
    /// Returns the maximum possible number of elements
    size_type max_size() const noexcept { return size_type{ N }; }
  };

  // ---------------------------------------------------
  /// A simple container class which supports
  /// - begin
  /// - end
  /// - size
  /// - push_back (max number of elements is limited, no dynamic memory allocation)
  /// and other functions as known from STL containers
  // ---------------------------------------------------
  template<class T, class Allocat = StaticAllocator<T, 16> >
  class Vector
  {
  public:
    /// MemberClass
    typedef T value_type;
    /// This class
    typedef Vector<T, Allocat> This;
    /// iterators
    typedef T *       iterator;
    typedef const T * const_iterator;
    /// size type
    typedef typename Allocat::size_type size_type;

  protected:
    typedef T * pointer;
    typedef const T * const_pointer;

    pointer myFirst;
    pointer myLast;
    pointer myEnd;
    Allocat myal;

    pointer& getMyFirst() noexcept { return myFirst; }
    pointer& getMyLast() noexcept { return myLast; }
    pointer& getMyEnd() noexcept { return myEnd; }
    const const_pointer& getMyFirst() const noexcept { return myFirst; }
    const const_pointer& getMyLast() const noexcept { return myLast; }
    const const_pointer& getMyEnd() const noexcept { return myEnd; }

    /// Returns size of allocated memory
    size_type capacity() const noexcept { return static_cast<size_type>(getMyEnd() - getMyFirst()); }

    /// Returns new capacity which is greater than (or equal to) newsize and smaller than max_size()
    size_type calculateNewCapacity(const size_type newsize) const
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

    /// Copies elements fromFirst, ..., fromLast to toFirst
    void move(const_pointer fromFirst, const_pointer fromLast, pointer toFirst)
    {
      while (fromFirst != fromLast)
      {
        *toFirst++ = *fromFirst++;
      }
    }

    /// Discard old memory, use new memory
    void changeMem(pointer newfirst, const size_type newsize, const size_type newcapacity)
    {
      myal.deallocate(getMyFirst(), size());
      getMyFirst() = newfirst;
      getMyLast() = newfirst + newsize;
      getMyEnd() = newfirst + newcapacity;
    }
    /// Returns true if allocated memory is available to store new data
    bool hasUnusedCapacity() const noexcept
    {
      return getMyLast() != getMyEnd();
    }

    /// Stores v as last element; assumes that sufficient allocated memory is available
    void emplace_back(const T& v)
    {
      *getMyLast() = v;
      getMyLast()++;
    }
    /// Reallocates memory and stores v as last element; iterators are invalid afterwards if new memory was allocated successfully
    void emplace_back_realloc(const T& v)
    {
      const size_type cursize = size();
      if (cursize < max_size())
      {
        const size_type newcapacity = calculateNewCapacity(cursize + 1);
        pointer newfirst = myal.allocate(newcapacity);
        if (newfirst != nullptr)
        {
          move(getMyFirst(), getMyLast(), newfirst);
          changeMem(newfirst, cursize, newcapacity);
          emplace_back(v);
        }
      }
    }

  public:
    /// Constructor
    Vector()
      : myFirst(nullptr)
      , myLast(nullptr)
      , myEnd(nullptr)
    {}
    /// Destructor
    ~Vector()
    {
      myal.deallocate(getMyFirst(), capacity());
    }
    /// begin
    iterator begin() { return getMyFirst(); }
    const_iterator begin() const { return getMyFirst(); }
    /// end
    iterator end() { return getMyEnd(); }
    const_iterator end() const { getMyEnd(); }
    /// Returns the maximum possible number of elements
    size_type max_size() const noexcept { return myal.max_size(); }
    /// push back
    void push_back(const T& v)
    {
      if (hasUnusedCapacity())
      {
        emplace_back(v);
      }
      else
      {
        emplace_back_realloc(v);
      }
    }
    /// Returns a reference to the last element in the container.
    T& back() { return *getMyLast(); }
    const T& back() const { return *getMyLast(); }
    /// Returns the number of elements
    size_type size() const { return static_cast<size_type>(getMyLast() - getMyFirst()); }
    /// find
    iterator find(const T& p)
    {
      iterator it;
      for (it = begin(); it != end(); it++)
      {
        if (*it == p)
        {
          break;
        }
      }
      return it;
    }
  };

} // namespace Util

#endif // UTIL_VECTOR_H