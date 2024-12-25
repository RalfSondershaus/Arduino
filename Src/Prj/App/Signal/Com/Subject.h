/**
 * @file Signal/Com/Subject.h
 *
 * @brief Observer interface for ASCII communication
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * See <https://www.gnu.org/licenses/>.
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