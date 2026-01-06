/**
  * @file Logger.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines a class to print data to serial
  *
  * @copyright Copyright 2025 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_LOGGER_H_
#define UTIL_LOGGER_H_

#include <Std_Types.h>
#include <Util/String.h>
#include <Util/Timer.h>
#include <Hal/Timer.h>
#include <Hal/Serial.h>

namespace util
{
  // ------------------------------------------------------------------------------
  // ------------------------------------------------------------------------------
  class logger
  {
  public:
    using time_type = util::MilliTimer::time_type;

  protected:
    bool isStarted;
    bool isPrint;
    util::MilliTimer nextPrintTime;
    time_type cycleTime;

  public:
    logger() : isStarted(false)
    {}

    void start(time_type updateCycle) { isStarted = true; nextPrintTime.start(updateCycle); cycleTime = updateCycle; }
    void stop() { isStarted = false; }

    void printTime(const char * szNote) 
    { 
      hal::serial::print("[");
      hal::serial::print(hal::micros());
      hal::serial::print("] ");
      if (szNote)
      {
        hal::serial::print("(");
        hal::serial::print(szNote);
        hal::serial::print(") ");
      }
    }

    void begin(const char * szNote) 
    {
      if (isStarted && nextPrintTime.timeout()) 
      {
        printTime(szNote);
        nextPrintTime.increment(cycleTime);
        isPrint = true;
      }
    }
    void end() 
    { 
      if (isPrint) 
      { 
        hal::serial::println(); 
        isPrint = false;
      }
    }

    logger& operator<<(uint8 val)  { if (isPrint) { hal::serial::print(static_cast<int>(val)); } return *this; }
    logger& operator<<(uint16 val) { if (isPrint) { hal::serial::print(val); } return *this; }
    logger& operator<<(uint32 val) { if (isPrint) { hal::serial::print(static_cast<unsigned long>(val)); } return *this; }
    // TODO check how to print 64 bit values
    logger& operator<<(uint64 val) { if (isPrint) { hal::serial::print(static_cast<unsigned long>(val)); } return *this; }
    logger& operator<<(const char * val) { if (isPrint) { hal::serial::print(val); } return *this; }
    logger& operator<<(int val)    { if (isPrint) { hal::serial::print(val); } return *this; }
    template<int Size, class CharT>
    logger& operator<<(basic_string<Size, CharT>& str) { if (isPrint) { hal::serial::print(str.c_str()); } return *this; }
    
    // for logger::endl
    logger& operator<<(logger& (*pfunc)(logger&)) { return pfunc(*this); }

    logger& endl(logger& log) { (void) log; end(); return *this; }
  };
} // namespace util

#endif // UTIL_LOGGER_H
