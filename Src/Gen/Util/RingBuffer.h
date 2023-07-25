/**
  * @file RingBuffer.h
  *
  * @author Ralf Sondershaus
  *
  * @brief util::ring_buffer is an indexed sequence container that operates as a
  *        ring buffer
  *
  * util::ring_buffer is an indexed sequence container that allows fast insertion 
  * and deletion at both its beginning and its end.
  *
  * @copyright Copyright 2020 - 2023 Ralf Sondershaus
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

#ifndef UTIL_RINGBUFFER_H
#define UTIL_RINGBUFFER_H

#include <Util/Array.h>

namespace util
{
  // ---------------------------------------------------
  /// A ring buffer
  /// - T: type of elements to be stored
  /// - N: size
  // ---------------------------------------------------
  template<class T, int N>
  class ring_buffer
  {
  public:
    /// MemberClass
    typedef T value_type;
    /// This class
    typedef ring_buffer<T, N> This;
    /// The array
    typedef array<T, N> array_type;
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
    ring_buffer() : unIdxRead(0u), unIdxWrite(0u), bBufferFull(false) 
    {}
    /// Destructor
    ~ring_buffer() = default;
    /// Returns true if a value is available (and writes the value into parameter val).
    /// Returns false otherwise (and keeps parameter val unchanged).
    /// Not thread safe.
    bool get(reference val)
    {
      bool bRet;

      if (is_value_available())
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
    /// Returns true is buffer had an overflow, clear internal state flag for buffer overflow.
    /// Not thread safe.
    bool is_full()
    {
      bool bRet;

      bRet = bBufferFull;
      bBufferFull = false;

      return bRet;
    }
  protected:
    /// Return true if the next value can be read (has been written before)
    bool is_value_available() const
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
    array_type buffer;
    /// Current read index
    size_type unIdxRead;
    /// Current write index
    size_type unIdxWrite;
    /// Flag to signal that an overflow happened
    bool bBufferFull;
  };

} // namespace Util

#endif // UTIL_RINGBUFFER_H