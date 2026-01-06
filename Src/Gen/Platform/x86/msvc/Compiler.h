/**
 * Specification of Compiler Abstraction.
 * 
 * This file defines:
 * - NULL_PTR (The compiler abstraction shall provide the NULL_PTR define with a void pointer to zero definition)
 * - INLINE (The compiler abstraction shall provide the INLINE define for abstraction of the keyword inline)
 * - LOCAL_INLINE (The compiler abstraction shall provide the LOCAL_INLINE define for abstraction of the keyword inline in functions with �static� scope)
 *
 * @file x86/msvc/Compiler.h
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COMPILER_H
#define COMPILER_H

#define NULL_PTR      ((void*) 0)
#define INLINE        inline
#define LOCAL_INLINE  static inline

#define ROM_CONST_VAR
#define ROM_READ_BYTE(addr)     (*(const uint8_t*)(addr))
#define ROM_READ_WORD(addr)     (*(const uint16_t*)(addr))
#define ROM_READ_DWORD(addr)    (*(const uint32_t*)(addr))
#define ROM_READ_PTR(addr)      (*(void* const *)addr)
#define ROM_READ_STRING(dst, src) strcpy((dst), (src))
#define ROM_READ_STRUCT(dst, src, len) memcpy((dst), (src), (len))

#endif // COMPILER_H
// EOF