/**
 * @file Signal/Com/Observer.h
 *
 * @brief Observer interface for ASCII communication
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OBSERVER_H_
#define OBSERVER_H_

namespace com
{
  // -----------------------------------------------------------------------------------
  /// Abstract class to define an Observer interface
  // -----------------------------------------------------------------------------------
  class Observer
  {
  public:
    Observer() = default;

    virtual void update() = 0;
  };
} // namespace com

#endif // OBSERVER_H_