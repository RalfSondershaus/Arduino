/**
  * @file RingBuffer.h
  *
  * @author Ralf Sondershaus
  *
  * A ring buffer
  */

#ifndef UTIL_RINGBUFFER_H
#define UTIL_RINGBUFFER_H

#include <Util/Array.h>

namespace Util
{
  // ---------------------------------------------------
  /// A ring buffer
  /// - T: type of elements to be stored
  /// - N: size
  // ---------------------------------------------------
  template<class T, int N>
  class RingBuffer
  {
  public:
    /// MemberClass
    typedef T value_type;
    /// This class
    typedef RingBuffer<T, N> This;
    /// The array
    typedef Array<T, N> RingArray;
    /// iterators
    typedef T *       iterator;
    typedef const T * const_iterator;
    /// size type
    typedef size_t size_type;
    /// pointer type
    typedef T * pointer;
    typedef const T * const_pointer;
    /// reference type
    typedef T& reference;
    typedef const T& const_reference;
  public:
    /// Constructor
    RingBuffer() : unIdxRead(0u), unIdxWrite(0u), bBufferFull(false) 
    {}
    /// Destructor
    ~RingBuffer() = default;
    /// Returns true if a value is available (written into parameter val), return false otherwise (and keep parameter val unchanged).
    /// Not thread safe.
    bool get(reference val)
    {
      bool bRet;

      if (isValueAvailable())
      {
        val = buffer[unIdxRead];
        unIdxRead++;
        if (unIdxRead >= N)
        {
          unIdxRead = 0;
        }
        bRet = true;
      }
      else
      {
        bRet = false;
      }

      return bRet;
    }
    /// add a val to the ring buffer if possible.
    /// Not thread safe.
    void add(const_reference val)
    {
      buffer[unIdxWrite] = val;
      unIdxWrite++;
      if (unIdxWrite >= N)
      {
        unIdxWrite = 0;
      }
      if (unIdxWrite == unIdxRead)
      {
        clear();
        bBufferFull = true;
      }
    }
    /// Return true is buffer had an overflow, clear internal state flag for buffer overflow.
    /// Not thread safe.
    bool isBufferFull()
    {
      bool bRet;

      bRet = bBufferFull;
      bBufferFull = false;

      return bRet;
    }
  protected:
    /// Return true if the next value can be read (has been written before)
    bool isValueAvailable() const
    {
      return (unIdxRead != unIdxWrite) && (!bBufferFull);
    }
    /// Restart
    void clear()
    {
      unIdxRead = 0u;
      unIdxWrite = 0u;
    }
    /// The buffer contains time differences
    RingArray buffer;
    /// Current read index
    size_type unIdxRead;
    /// Current write index
    size_type unIdxWrite;
    /// Flag to signal that an overflow happened
    bool bBufferFull;
  };

} // namespace Util

#endif // UTIL_ARRAY_H