/**
 * Specification of Compiler Abstraction.
 * 
 * This file defines:
 * - NULL_PTR (The compiler abstraction shall provide the NULL_PTR define with a void pointer to zero definition)
 * - INLINE (The compiler abstraction shall provide the INLINE define for abstraction of the keyword inline)
 * - LOCAL_INLINE (The compiler abstraction shall provide the LOCAL_INLINE define for abstraction of the keyword inline in functions with “static” scope)
 *
 * @file x86/gcc/Compiler.h
 */

#ifndef COMPILER_H
#define COMPILER_H

#define NULL_PTR      ((void*) 0)
#define INLINE        inline
#define LOCAL_INLINE  static inline

#endif // COMPILER_H
// EOF