/**
 * @file Hal/Win/Hal/Serial.h
 *
 * @brief 
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
 * See <https://www.gnu.org/licenses/>.
 */

#ifndef HAL_SERIAL_H_
#define HAL_SERIAL_H_

#include <Std_Types.h>

// From Arduino's Print.h
#define DEC 10
#define HEX 16
#define OCT 8

namespace hal
{
  // -----------------------------------------------------------------------------------
  /// Read values from serial interface (fetch bytes) and store them into a queue.
  ///
  /// Remark: For serial communication, Arduino uses a 16 byte buffer (for systems with 
  /// RAM smaller than 1 KB) or a 64 byte buffer (else).
  // -----------------------------------------------------------------------------------
  namespace serial
  {
    /// Start serial communication
    void begin(unsigned long baudrate);
    /// Get the number of bytes (characters) available for reading from the serial port.
    int available(void);
    /// The first byte of incoming serial data available (or -1 if no data is available).
    int read(void);

    /// Print functions
    size_t print(const char *p);
    size_t print(char c);
    size_t print(unsigned char uc, int base = DEC);
    size_t print(int n, int base = DEC);
    size_t print(unsigned int un, int base = DEC);
    size_t print(long n, int base = DEC);
    size_t print(unsigned long un, int base = DEC);
    size_t print(double d, int digits = 2);

    size_t println(const char *p);
    size_t println(char c);
    size_t println(unsigned char uc, int base = DEC);
    size_t println(int n, int base = DEC);
    size_t println(unsigned int un, int base = DEC);
    size_t println(long n, int base = DEC);
    size_t println(unsigned long un, int base = DEC);
    size_t println(double d, int digits = 2);
    size_t println();
  }
} // namespace com

#endif // SERDRV_H_