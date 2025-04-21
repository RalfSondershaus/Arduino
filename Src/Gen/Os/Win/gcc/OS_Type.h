/**
 * Specification of OS Abstraction.
 *
 * Implementation for GCC for Windows or Linux.
 * 
 * This file defines:
 * - ISR (The compiler abstraction shall provide the NULL_PTR define with a void pointer to zero definition)
 * -  (The compiler abstraction shall provide the INLINE define for abstraction of the keyword inline)
 *
 * @file gcc/OS_Type.h
 */

#ifndef OS_TYPE_H
#define OS_TYPE_H

/* Interrupt handling */
 
/* Macro for interrupt service routines */
#define ISR(handler)  void handler(void)

/* Enable / disable interrupts */
#define SuspendAllInterrupts()
#define ResumeAllInterrupts()

#endif // OS_TYPE_H
// EOF