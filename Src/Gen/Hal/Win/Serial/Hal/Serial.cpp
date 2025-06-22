/**
 * @file Hal/Win/Serial/Hal/Serial.cpp
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

#include <Std_Types.h>
#include <Hal/Serial.h>
#include <iomanip> // for std::setw
#include <iostream>
#include <cstring>

namespace hal
{
  namespace serial 
  {
    /**
     * @brief Sets the std::cout number base formatting according to the given base.
     * 
     * @param base The base to use: 10 for decimal, 16 for hexadecimal, 8 for octal.
     */
    void set_cout_base(int base)
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

    void begin(long baudrate)   {}
    int available()             { return 0; }
    int read()                  { return 0; }
    /// Print functions
    size_t print(const char *p)                       { std::cout << p; return std::strlen(p); }
    size_t print(char c)                              { std::cout << c; return 1; }
    size_t print(unsigned char uc, int base = DEC)    { set_cout_base(base); std::cout << static_cast<int>(uc); return 1; }
    size_t print(int n, int base = DEC)               { set_cout_base(base); std::cout << n;  return 1; }
    size_t print(unsigned int un, int base = DEC)     { set_cout_base(base); std::cout << un; return 1; }
    size_t print(long n, int base = DEC)              { set_cout_base(base); std::cout << n;  return 1; }
    size_t print(unsigned long un, int base = DEC)    { set_cout_base(base); std::cout << un; return 1; }
    size_t print(double d, int digits = 2)            { std::cout << std::setw(digits) << d; return 1; }

    size_t println(const char *p)                       { std::cout << p << std::endl; return std::strlen(p); }
    size_t println(char c)                              { std::cout << c << std::endl; return 1; }
    size_t println(unsigned char uc, int base = DEC)    { set_cout_base(base); std::cout << static_cast<int>(uc) << std::endl; return 1; }
    size_t println(int n, int base = DEC)               { set_cout_base(base); std::cout << n  << std::endl; return 1; }
    size_t println(unsigned int un, int base = DEC)     { set_cout_base(base); std::cout << un << std::endl; return 1; }
    size_t println(long n, int base = DEC)              { set_cout_base(base); std::cout << n  << std::endl;  return 1; }
    size_t println(unsigned long un, int base = DEC)    { set_cout_base(base); std::cout << un << std::endl; return 1; }
    size_t println(double d, int digits = 2)            { std::cout << std::setw(digits) << d  << std::endl; return 1; }
    size_t println()                                    { std::cout << std::endl; return 1; }
  }
} // namespace com
