/**
 * Specification of (microcontroller) platform types for Arduino AVR projects and the AVR GCC compiler.
 * 
 * This file defines:
 * - boolean
 * - uintN with N = 8, 16, 32, 64
 * - sintN with N = 8, 16, 32, 64
 * - uintN_least with N = 8, 16, 32
 * - sintN_least with N = 8, 16, 32
 * - CPU_TYPE (any of CPU_TYPE_8, CPU_TYPE_16, CPU_TYPE_32 or CPU_TYPE_64)
 * - CPU_BIT_ORDER (any of MSB_FIRST, or LSB_FIRST)
 * - CPU_BYTE_ORDER (any of HIGH_BYTE_FIRST or LOW_BYTE_FIRST)
 * - TRUE, FALSE
 *
 * @file avr-gcc/Platform_Types.h
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

// Atmel's AVR GCC version doesn't have C++ standard headers, so we use C standard headers instead.
#include <stdint.h>
#include <stddef.h> // size_t, ptrdiff_t

/* In the C++ standard, fundamtental types are defined with a minimum width,
   see also https://eel.is/c++draft/tab:basic.fundamental.width

   signed char     8
   short int      16
   int            16
   long int       32
   long long int  64
*/

/*
   C++11 standard
   
   3.9.1 Fundamental types [basic.fundamental]

   Objects declared as characters (char) shall be large enough to store any member of the 
   implementation's basic character set. If a character from this set is stored in a character 
   object, the integral value of that character object is equal to the value of the single 
   character literal form of that character. It is implementation-defined whether a char 
   object can hold negative values. Characters can be explicitly declared unsigned or signed. 
   Plain char, signed char, and unsigned char are three distinct types. A char, a signed char,
   and an unsigned char occupy the same amount of storage and have the same alignment 
   requirements (basic.types); that is, they have the same object representation. 
   For character types, all bits of the object representation participate in the value 
   representation. For unsigned character types, all possible bit patterns of the value 
   representation represent numbers. These requirements do not hold for other types. In any 
   particular implementation, a plain char object can take on either the same values as a 
   signed char or an unsigned char; which one is implementation-defined.
 */

/* 
  integer types.

  For AVR - GCC compiler, we derive below types from stdint.h.

  Remark: AVR GCC stdint.h uses __attribute__ definitions:
  QI: An integer that is as wide as the smallest addressable unit, usually 8 bits.
  HI: An integer, twice as wide as a QI mode integer, usually 16 bits.
  SI: An integer, four times as wide as a QI mode integer, usually 32 bits.
  DI: An integer, eight times as wide as a QI mode integer, usually 64 bits.
  SF: A floating point value, as wide as a SI mode integer, usually 32 bits.
  DF: A floating point value, as wide as a DI mode integer, usually 64 bits.
  From a 32-bit perspective:
  QI - Quarter Int
  HI - Half Int
  SI - Single Int
  DI - Double Int
  SF - Single Float
  DF - Double Float
*/
typedef int8_t sint8;    // note: char is a different type, see 3.9.1 Fundamental types
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;
typedef uint8_t uint8;   // note: char is a different type, see 3.9.1 Fundamental types
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int_least8_t sint8_least;
typedef int_least16_t sint16_least;
typedef int_least32_t sint32_least;
typedef uint_least8_t uint8_least;
typedef uint_least16_t uint16_least;
typedef uint_least32_t uint32_least;

typedef int_fast8_t sint8_fast;
typedef int_fast16_t sint16_fast;
typedef int_fast32_t sint32_fast;
typedef uint_fast8_t uint8_fast;
typedef uint_fast16_t uint16_fast;
typedef uint_fast32_t uint32_fast;

/* floating types */
typedef float float32;
typedef double float64;

/* POSIX types */
typedef sint32 ssize_t;

/* CPU_TYPE */
#define CPU_TYPE_8        8
#define CPU_TYPE_16       16
#define CPU_TYPE_32       32
#define CPU_TYPE_64       64

#define CPU_TYPE          CPU_TYPE_8

/* CPU_BIT_ORDER */
#define MSB_FIRST         0
#define LSB_FIRST         1

#define CPU_BIT_ORDER     LSB_FIRST

/* CPU_BYTE_ORDER */
/* Big Endian */
#define HIGH_BYTE_FIRST   0
/* Little Endian */
#define LOW_BYTE_FIRST    1

#define CPU_BYTE_ORDER    LOW_BYTE_FIRST

/* boolean */

typedef bool boolean;

/* FALSE */
#ifndef FALSE
#define FALSE false
#endif

/* TRUE */
#ifndef TRUE
#define TRUE true
#endif

/* size types */

#endif // PLATFORM_TYPES_H
