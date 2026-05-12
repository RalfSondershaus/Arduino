/**
 * @file Observer.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Observer interface, currently used for the serial ASCII communication stack.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OBSERVER_H_
#define OBSERVER_H_

namespace com
{
  /**
   * @brief Abstract base class for observers, currently used in the serial ASCII communication stack.
   *
   * Implements the Observer side of the Observer/Subject pattern. It is used between
   * @ref SerAsciiTP (subject) and @ref AsciiCom (observer). When @ref SerAsciiTP
   * has assembled a complete telegram it calls @ref notify() on the registered
   * @ref Subject, which in turn calls @ref update() on every attached Observer.
   *
   * @note No virtual destructor is provided because AVR-GCC requires
   *       `operator delete` for virtual destructors (which is not available). Objects of this class
   *       are never heap-allocated or destroyed at run-time.
   *
   * @see Subject, AsciiCom, SerAsciiTP
   */
  class Observer
  {
  public:
    Observer() = default;

    /**
     * @brief Called by the subject when a new telegram has been received.
     *
     * Implementations must read the telegram from the subject, process it,
     * and transmit a response if required.
     */
    virtual void update() = 0;
  };
} // namespace com

#endif // OBSERVER_H_