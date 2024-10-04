/**
  * @file logger.h
  * @author Ralf Sondershaus
  *
  * @brief Defines a class to log data
  * 
  * @copyright Copyright 2024 Ralf Sondershaus
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
}
#endif

#endif // TEST_LOGGER_H
