/**
 * @file Signal/Com/ComDrv.h
 *
 * @brief Interface for communication drivers
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
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