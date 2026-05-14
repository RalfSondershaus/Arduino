/**
  * @file Logger.h
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

/* 
 * Default is that CFG_NO_LOGGER is not defined, so logging is enabled. To disable logging, 
 * define CFG_NO_LOGGER in the project configuration.
 * This ensures that existing test projects do not break.
 */
#ifdef CFG_NO_LOGGER
#define DO_LOGGING 0
#else
#define DO_LOGGING 1
#endif // CFG_NO_LOGGER

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
#if DO_LOGGING
    open(filename);
#else
    (void)filename;
#endif
  }
  void stop()
  {
#if DO_LOGGING
    close();
#endif
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

#endif // WIN32

#endif // TEST_LOGGER_H
