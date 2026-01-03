/**
 * @file Hal/Stub/Hal/Serial.h
 *
 * @brief 
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_SERIAL_H_
#define HAL_SERIAL_H_

#include <Std_Types.h>

/**
 * @brief Macro to mark string literals for storage in Flash memory on Arduino platforms.
 * 
 * On unit tests, this macro has no effect and simply returns the original string literal.
 * 
 * Example:
 * @code
 * hal::serial::print(F("Hello, World!"));
 * @endcode
 * 
 * @param string_literal The string literal to be marked for Flash storage.
 * @return The original string literal.
 */
#define F(string_literal) (string_literal)
class __FlashStringHelper; // needed for namespace debug to avoid #ifdefs

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
    namespace stubs
    {
        extern unsigned long baudrate;
        extern unsigned int available;
        extern int read;
    }
    
    /// Start serial communication
    inline void begin(unsigned long baudrate)  { stubs::baudrate = baudrate; }
    /// Get the number of bytes (characters) available for reading from the serial port.
    inline int available(void)                 { return stubs::available; }
    /// The first byte of incoming serial data available (or -1 if no data is available).
    inline int read(void)                      { return stubs::read; }

    /// Print functions
    inline size_t print(const char *p)                       { (void) p; return 0; }
    inline size_t print(const __FlashStringHelper *p)        { return Serial.print(p); }
    inline size_t print(char c)                              { (void) c; return 0; }
    inline size_t print(unsigned char uc, int base = 10)     { (void) uc; (void) base; return 0; }
    inline size_t print(int n, int base = 10)                { (void) n; (void) base; return 0; }
    inline size_t print(unsigned int un, int base = 10)      { (void) un; (void) base; return 0; }
    inline size_t print(long n, int base = 10)               { (void) n; (void) base; return 0; }
    inline size_t print(unsigned long un, int base = 10)     { (void) un; (void) base; return 0; }
    inline size_t print(double d, int digits = 2)            { (void) d; (void) digits; return 0; }

    inline size_t println(const char *p)                       { (void) p; return 0; }
    inline size_t println(const __FlashStringHelper *p)        { return Serial.println(p); }
    inline size_t println(char c)                              { (void) c; return 0; }
    inline size_t println(unsigned char uc, int base = 10)     { (void) uc; (void) base; return 0; }
    inline size_t println(int n, int base = 10)                { (void) n; (void) base; return 0; }
    inline size_t println(unsigned int un, int base = 10)      { (void) un; (void) base; return 0; }
    inline size_t println(long n, int base = 10)               { (void) n; (void) base; return 0; }
    inline size_t println(unsigned long un, int base = 10)     { (void) un; (void) base; return 0; }
    inline size_t println(double d, int digits = 2)            { (void) d; (void) digits; return 0; }
    inline size_t println()                                    { return 0; }
  }
} // namespace com

#endif // SERDRV_H_