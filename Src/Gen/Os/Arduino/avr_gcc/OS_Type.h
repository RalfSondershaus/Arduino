/**
 * Specification of OS Abstraction.
 *
 * Implementation for AVR GCC for AVR Arduinos.
 * 
 * This file defines:
 * - ISR (The compiler abstraction shall provide the NULL_PTR define with a void pointer to zero definition)
 * -  (The compiler abstraction shall provide the INLINE define for abstraction of the keyword inline)
 *
 * @file avr-gcc/OS_Type.h
 */

#ifndef OS_TYPE_H
#define OS_TYPE_H

#include <Arduino.h>

/* Interrupt handling */
 
/* Macro for interrupt service routines */
#define ISR(handler)  void handler(void)

/* Enable / disable interrupts */
/* We rely on Arduino implementation, that is, on functions noInterrupts() and interrupts() */
#define SuspendAllInterrupts()  noInterrupts()
#define ResumeAllInterrupts()  interrupts()

#endif // OS_TYPE_H
// EOF