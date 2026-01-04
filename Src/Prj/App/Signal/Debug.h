/**
 * @file Debug.h
 *
 * @author Ralf Sondershaus
 *
 * @brief
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <Hal/Serial.h>

namespace debug
{
    static constexpr uint8 kNoDebug = 0;       ///< No debug output
    static constexpr uint8 kImportant = 1;     ///< Important messages
    static constexpr uint8 kDetailed = 2;      ///< Detailed messages
    static constexpr uint8 kVeryDetailed = 3;  ///< Very detailed messages

    /**
     * @brief Enable debug output with given verbosity level
     * 
     * @param verbosity The verbosity level to set for debug output
     */
    void enable(uint8 verbosity);

    /**
     * @brief Check if a message with given level shall be printed
     * 
     * @param msg_level The message level
     * @return true Print the message
     * @return false Do not print the message
     */
    inline void disable() { enable(kNoDebug); }

    /**
     * @brief Check if a message with given level shall be printed
     * 
     * @param msg_level The message level
     * @return true Print the message
     * @return false Do not print the message
     */
    bool shall_print(uint8 msg_level) noexcept;

    inline size_t print(uint8 verbosity, const char *p)      { return shall_print(verbosity) ? hal::serial::print(p) : 0; }
    inline size_t print(uint8 verbosity, const __FlashStringHelper *p)      { return shall_print(verbosity) ? hal::serial::print(p) : 0; }
    inline size_t print(uint8 verbosity, char c)             { return shall_print(verbosity) ? hal::serial::print(c) : 0; }
    inline size_t print(uint8 verbosity, unsigned char uc, int base = 10)    { return shall_print(verbosity) ? hal::serial::print(uc, base) : 0; }
    inline size_t print(uint8 verbosity, int n, int base = 10)               { return shall_print(verbosity) ? hal::serial::print(n, base) : 0; }
    inline size_t print(uint8 verbosity, unsigned int un, int base = 10)     { return shall_print(verbosity) ? hal::serial::print(un, base) : 0; }
    inline size_t print(uint8 verbosity, long n, int base = 10)              { return shall_print(verbosity) ? hal::serial::print(n, base) : 0; }
    inline size_t print(uint8 verbosity, unsigned long un, int base = 10)    { return shall_print(verbosity) ? hal::serial::print(un, base) : 0; }
    inline size_t print(uint8 verbosity, double d, int digits = 2)            { return shall_print(verbosity) ? hal::serial::print(d, digits) : 0; }

    inline size_t println(uint8 verbosity, const char *p)                      { return shall_print(verbosity) ? hal::serial::println(p) : 0; }
    inline size_t println(uint8 verbosity, const __FlashStringHelper *p)      { return shall_print(verbosity) ? hal::serial::println(p) : 0; }
    inline size_t println(uint8 verbosity, char c)                             { return shall_print(verbosity) ? hal::serial::println(c) : 0; }
    inline size_t println(uint8 verbosity, unsigned char uc, int base = 10)   { return shall_print(verbosity) ? hal::serial::println(uc, base) : 0; }
    inline size_t println(uint8 verbosity, int n, int base = 10)              { return shall_print(verbosity) ? hal::serial::println(n, base) : 0; }
    inline size_t println(uint8 verbosity, unsigned int un, int base = 10)    { return shall_print(verbosity) ? hal::serial::println(un, base) : 0; }
    inline size_t println(uint8 verbosity, long n, int base = 10)             { return shall_print(verbosity) ? hal::serial::println(n, base) : 0; }
    inline size_t println(uint8 verbosity, unsigned long un, int base = 10)   { return shall_print(verbosity) ? hal::serial::println(un, base) : 0; }
    inline size_t println(uint8 verbosity, double d, int digits = 2)           { return shall_print(verbosity) ? hal::serial::println(d, digits) : 0; }
    inline size_t println(uint8 verbosity)                                     { return shall_print(verbosity) ? hal::serial::println() : 0; }
}

#endif // DEBUG_H_