/**
  * @file Tracer.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines a class to trace events
  *
  * @copyright Copyright 2025 Ralf Sondershaus
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
  * See <https://www.gnu.org/licenses/>.
  */

#ifndef UTIL_TRACER_H_
#define UTIL_TRACER_H_

#include <Std_Types.h>
#include <Util/String.h>
#include <Hal/Timer.h>
#include <Hal/Serial.h>
#include <Util/Fix_Queue.h>

namespace util
{
  // ------------------------------------------------------------------------------
  /// N = Buffer size
  /// InfoLen = Length of additional information
  ///
  /// RAM required for buffer is a bit more than N * InfoLen
  ///
  /// util::tracer trc;
  /// ...
  /// uint8 val = 2;
  /// ...
  /// trc.trace("TracePoint 1");
  /// trc << val << " something";
  /// ...
  /// trc.trace("TracePoint 2");
  /// ...
  /// trc.flush();
  /// [10000000] (TracePoint 1) 2 something
  /// [10005000] (TracePoint 2)
  // ------------------------------------------------------------------------------
  template<size_t N, size_t InfoLen>
  class tracer
  {
  public:

  protected:
    static constexpr size_t kBufferSize = N;

    using time_type = util::MicroTimer::time_type;
    using string_type = util::basic_string<InfoLen, char>;

    typedef struct
    {
      time_type timeStamp;
      const char * szNote;
      string_type strInfo;
      data_type(time_type ts, const char * note) { timeStamp = ts; szNote = szNote; }
    } data_type;
    
    using queue_type = util::fix_queue<data_type, kBufferSize>;

    queue_type queue;

  public:
    tracer() = default;

    void print(const data_type& data) 
    { 
      hal::serial::print("[");
      hal::serial::print(data.timeStamp);
      hal::serial::print("] ");
      if (data.szNote)
      {
        hal::serial::print("(");
        hal::serial::print(data.szNote);
        hal::serial::print(") ");
      }
      hal::serial::print(data.strInfo.c_str());
      hal::serial::println();
    }

    void flush()
    {
      while (!queue.empty())
      {
        print(queue.front());
        queue.pop();
      }
    }

    void trace(const char * szNote)
    {
      queue.push(data_type{ hal::micros(), szNote });
    }

    tracer& operator<<(uint8 val)         { queue.back().strInfo.append(val); return *this; }
    tracer& operator<<(uint16 val)        { queue.back().strInfo.append(val); return *this; }
    tracer& operator<<(uint32 val)        { queue.back().strInfo.append(val); return *this; }
    tracer& operator<<(const char * val)  { queue.back().strInfo.append(val); return *this; }
    tracer& operator<<(int val)           { queue.back().strInfo.append(val); return *this; }
    template<int Size, class CharT>
    tracer& operator<<(basic_string<Size, CharT>& str) { queue.back().strInfo.append(str); return *this; }
    
    // for tracer::endl
    tracer& operator<<(tracer& (*pfunc)(tracer&)) { return pfunc(*this); }

    tracer& endl(tracer& log) { log << "\n"; return *this; }
  };
} // namespace util

#endif // UTIL_TRACER_H_
