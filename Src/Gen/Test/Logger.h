/**
  * @file logger.h
  * @author Ralf Sondershaus
  *
  * @brief Defines a class to log data
  * 
  * @copyright Copyright 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */


#ifndef TEST_LOGGER_H
#define TEST_LOGGER_H

#ifdef WIN32
#include <ios> // for Logger on Windows
#include <iomanip> // for Logger on Windows
#include <fstream> // for Logger on Windows
#endif

#ifdef WIN32
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
class Logger : public std::ofstream
{
public:
  void start(const std::string& filename)
  {
    open(filename);
  }
  void stop()
  {
    close();
  }
};
#else
// ------------------------------------------------------------------------------------------------
/// This dummy logger for Arduino is doing nothing.
// ------------------------------------------------------------------------------------------------
class Logger
{
public:
  void start(const char* filename)
  {
  }
  void stop()
  {
  }
  Logger& operator<<(uint16) { return *this; }
  Logger& operator<<(const char *) { return *this; }
};

namespace std
{
  // simple fix for std::endl on Arduino
  constexpr char endl = '\n';
  // simple fix for std::setw on Arduino
  uint16 setw(int) { return 0; }
}
#endif

#endif // TEST_LOGGER_H
