/**
 * @file Subject.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Subject interface for the Observer/Subject pattern in the COM stack.
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef COM_SUBJECT_H_
#define COM_SUBJECT_H_

#include <Com/Observer.h>
#include <Util/Array.h>
#include <Util/Ptr.h>

namespace com
{
  /**
   * @brief Subject side of the Observer/Subject pattern used in the COM stack.
   *
   * Holds a reference to one @ref Observer and calls @ref Observer::update()
   * via @ref notify() when a new telegram is available. Currently limited to a
   * single attached observer.
   *
   * @note No virtual destructor is provided; objects are never heap-allocated.
   *
   * @see Observer, SerAsciiTP, AsciiCom
   */
  class Subject
  {
  protected:
    util::ptr<Observer> observer; ///< The single registered observer; null if none attached

    /**
     * @brief Invoke @ref Observer::update() on the attached observer, if any.
     *
     * Called internally (e.g. by @ref SerAsciiTP::cycle()) when a complete
     * telegram has been assembled and is ready for processing.
     */
    void notify() 
    { 
      if (observer) 
      {
        observer->update(); 
      }
    }

  public:
    Subject() = default;

    /**
     * @brief Attach an observer to this subject.
     *
     * Replaces any previously attached observer. Currently only one observer
     * is supported at a time.
     *
     * @param[in] obs Observer instance to register (e.g. an @ref AsciiCom object).
     */
    void attach(Observer& obs) { observer = &obs; }
  };
} // namespace com

#endif // COM_SUBJECT_H_