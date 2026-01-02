/**
 * @file Hal/Serial/Hal/Serial.h
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
#include <Arduino.h>

namespace hal
{
  /**
   * @brief Sets up serial communication at the specified baud rate.
   * 
   * Read values from serial interface (fetch bytes) and store them into a queue.
   * 
   * @note The `F()` macro is used to store string literals in Flash memory on Arduino platforms.
   * On other platforms, such as Windows, this macro has no effect and simply returns the original 
   * string literal. Example:
   * @code
   * hal::serial::print(F("Hello, World!"));
   * @endcode
   * 
   * @note: For serial communication, Arduino uses a 16 byte buffer (for systems with 
   * RAM smaller than 1 KB) or a 64 byte buffer (else).
   */
  namespace serial
  {
    /// Start serial communication
    inline void begin(unsigned long baudrate)    { Serial.begin(baudrate); }
    /// Get the number of bytes (characters) available for reading from the serial port.
    inline int available(void)                   { return Serial.available(); }
    /// The first byte of incoming serial data available (or -1 if no data is available).
    inline int read(void)                        { return Serial.read(); }

    /// Print functions
    inline size_t print(const char *p)                       { return Serial.print(p); }
    inline size_t print(const __FlashStringHelper *p)        { return Serial.print(p); }
    inline size_t print(char c)                              { return Serial.print(c); }
    inline size_t print(unsigned char uc, int base = DEC)    { return Serial.print(uc, base); }
    inline size_t print(int n, int base = DEC)               { return Serial.print(n, base); }
    inline size_t print(unsigned int un, int base = DEC)     { return Serial.print(un, base); }
    inline size_t print(long n, int base = DEC)              { return Serial.print(n, base); }
    inline size_t print(unsigned long un, int base = DEC)    { return Serial.print(un, base); }
    inline size_t print(double d, int digits = 2)            { return Serial.print(d, digits); }

    inline size_t println(const char *p)                       { return Serial.println(p); }
    inline size_t println(const __FlashStringHelper *p)          { return Serial.println(p); }
    inline size_t println(char c)                              { return Serial.println(c); }
    inline size_t println(unsigned char uc, int base = DEC)    { return Serial.println(uc, base); }
    inline size_t println(int n, int base = DEC)               { return Serial.println(n, base); }
    inline size_t println(unsigned int un, int base = DEC)     { return Serial.println(un, base); }
    inline size_t println(long n, int base = DEC)              { return Serial.println(n, base); }
    inline size_t println(unsigned long un, int base = DEC)    { return Serial.println(un, base); }
    inline size_t println(double d, int digits = 2)            { return Serial.println(d, digits); }
    inline size_t println()                                    { return Serial.println(); }
}
} // namespace com

#endif // SERDRV_H_