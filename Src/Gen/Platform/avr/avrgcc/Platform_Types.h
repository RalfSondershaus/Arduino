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
 */

#ifndef PLATFORM_TYPES_H
#define PLATFORM_TYPES_H

// Atmel's AVR GCC version doesn't have C++ standard headers, so we use C standard headers instead.
#include <stdint.h>
#include <stddef.h> // size_t

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
typedef int8_t sint8;
typedef int16_t sint16;
typedef int32_t sint32;
typedef int64_t sint64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int_least8_t sint8_least;
typedef int_least16_t sint16_least;
typedef int_least32_t sint32_least;
typedef uint_least8_t uint8_least;
typedef uint_least16_t uint16_least;
typedef uint_least32_t uint32_least;

/* floating types */
typedef float float32;
typedef double float64;

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
