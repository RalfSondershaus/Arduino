/**
 * @file Signal/Com/ComDrv.h
 *
 * @brief Interface for communication drivers
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
 * See <https://www.gnu.org/licenses/>.
 */

#ifndef COMDRV_H_
#define COMDRV_H_

#include <Std_Types.h>
#include <Util/Fix_Queue.h>
#include <Hal/Serial.h>

namespace com
{
  // -----------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------
  class SerComDrv
  {
  public:
    using size_type = size_t;
    using base_type = uint8_t;

  public:
    SerComDrv() = default;

    /// Start serial communication
    void begin(unsigned long baudrate) { hal::serial::begin(baudrate); }
    /// Get the number of bytes (characters) available for reading from the serial port.
    int available(void)             { return hal::serial::available(); }
    /// The first byte of incoming serial data available (or -1 if no data is available).
    int read(void) const noexcept   { return hal::serial::read(); }
    
    /// Write a character string to the serial port
    int write(const char* p)        { return hal::serial::print(p); }

  };
} // namespace com

#endif // COMDRV_H_