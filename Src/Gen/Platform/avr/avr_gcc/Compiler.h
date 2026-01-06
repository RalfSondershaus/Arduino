/**
 * Specification of Compiler Abstraction.
 * 
 * This file defines:
 * - NULL_PTR (The compiler abstraction shall provide the NULL_PTR define with a void pointer to zero definition)
 * - INLINE (The compiler abstraction shall provide the INLINE define for abstraction of the keyword inline)
 * - LOCAL_INLINE (The compiler abstraction shall provide the LOCAL_INLINE define for abstraction of the keyword inline in functions with �static� scope)
 *
 * @file avr-gcc/Compiler.h
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMPILER_H
#define COMPILER_H

#include <avr/pgmspace.h>

#define NULL_PTR      ((void*) 0)
#define INLINE        inline
#define LOCAL_INLINE  static inline

/* Memory class for constants in Flash */
#define ROM_CONST_VAR           PROGMEM

/* Access macros for reading from Flash */
#define ROM_READ_BYTE(addr)     pgm_read_byte(addr)
#define ROM_READ_WORD(addr)     pgm_read_word(addr)
#define ROM_READ_DWORD(addr)    pgm_read_dword(addr)
#define ROM_READ_PTR(addr)      pgm_read_ptr(addr)
#define ROM_READ_STRING(dst, src) strcpy_P((dst), (src))
#define ROM_READ_STRUCT(dst, src, len) memcpy_P((dst), (src), (len))

#endif // COMPILER_H
// EOF