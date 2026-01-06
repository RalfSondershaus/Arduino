/**
 * @file Hal/Win/Hal/Serial.h
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
#include <iomanip> // for std::setw
#include <iostream>
#include <cstring>

// From Arduino's Print.h
#define DEC 10
#define HEX 16
#define OCT 8

/**
 * @brief Macro to mark string literals for storage in Flash memory on Arduino platforms.
 * 
 * On Windows, this macro has no effect and simply returns the original string literal.
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
        /**
         * @brief Sets the std::cout number base formatting according to the given base.
         *
         * @param base The base to use: 10 for decimal, 16 for hexadecimal, 8 for octal.
         */
        inline void set_cout_base(int base)
        {
            switch (base)
            {
            case 16:
                std::cout << std::hex;
                break;
            case 8:
                std::cout << std::oct;
                break;
            default:
                std::cout << std::dec;
                break;
            }
        }
        /// Start serial communication
        inline void begin(unsigned long baudrate) { (void)baudrate; /* Nothing to do on Windows */ }
        inline int available() { return 0; }
        inline int read() { return 0; }
        /// Print functions
        inline size_t print(const char *p)
        {
            std::cout << p;
            return std::strlen(p);
        }
        inline size_t print(const __FlashStringHelper *p)       
        { 
            std::cout << reinterpret_cast<const char*>(p); 
            return std::strlen(reinterpret_cast<const char*>(p));
        }
        inline size_t print(char c)
        {
            std::cout << c;
            return 1;
        }
        inline size_t print(unsigned char uc, int base = DEC)
        {
            set_cout_base(base);
            std::cout << static_cast<int>(uc);
            return 1;
        }
        inline size_t print(int n, int base = DEC)
        {
            set_cout_base(base);
            std::cout << n;
            return 1;
        }
        inline size_t print(unsigned int un, int base = DEC)
        {
            set_cout_base(base);
            std::cout << un;
            return 1;
        }
        inline size_t print(long n, int base = DEC)
        {
            set_cout_base(base);
            std::cout << n;
            return 1;
        }
        inline size_t print(unsigned long un, int base = DEC)
        {
            set_cout_base(base);
            std::cout << un;
            return 1;
        }
        inline size_t print(double d, int digits = 2)
        {
            std::cout << std::setw(digits) << d;
            return 1;
        }

        inline size_t println(const char *p)
        {
            std::cout << p << std::endl;
            return std::strlen(p);
        }
        inline size_t println(const __FlashStringHelper *p)       
        { 
            std::cout << reinterpret_cast<const char*>(p) << std::endl;
            return std::strlen(reinterpret_cast<const char*>(p));
        }
        inline size_t println(char c)
        {
            std::cout << c << std::endl;
            return 1;
        }
        inline size_t println(unsigned char uc, int base = DEC)
        {
            set_cout_base(base);
            std::cout << static_cast<int>(uc) << std::endl;
            return 1;
        }
        inline size_t println(int n, int base = DEC)
        {
            set_cout_base(base);
            std::cout << n << std::endl;
            return 1;
        }
        inline size_t println(unsigned int un, int base = DEC)
        {
            set_cout_base(base);
            std::cout << un << std::endl;
            return 1;
        }
        inline size_t println(long n, int base = DEC)
        {
            set_cout_base(base);
            std::cout << n << std::endl;
            return 1;
        }
        inline size_t println(unsigned long un, int base = DEC)
        {
            set_cout_base(base);
            std::cout << un << std::endl;
            return 1;
        }
        inline size_t println(double d, int digits = 2)
        {
            std::cout << std::setw(digits) << d << std::endl;
            return 1;
        }
        inline size_t println()
        {
            std::cout << std::endl;
            return 1;
        }
    }
} // namespace com

#endif // SERDRV_H_