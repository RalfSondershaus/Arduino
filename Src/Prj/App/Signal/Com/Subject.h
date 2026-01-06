/**
 * @file Signal/Com/Subject.h
 *
 * @brief Observer interface for ASCII communication
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
  // -----------------------------------------------------------------------------------
  /// Abstract class to define an Observer interface
  // -----------------------------------------------------------------------------------
  class Subject
  {
  protected:
    /// Currently, just one observer is supported
    util::ptr<Observer> observer;
    
    /// Notify observers about updates
    void notify() 
    { 
      if (observer) 
      {
        observer->update(); 
      }
    }

  public:
    Subject() = default;

    /// Attach an observer object such as an AsciiCom instance.
    /// Currently, just one observer is supported.
    void attach(Observer& obs) { observer = &obs; }
  };
} // namespace com

#endif // COM_SUBJECT_H_