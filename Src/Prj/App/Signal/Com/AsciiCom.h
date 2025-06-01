/**
 * @file Signal/Com/AsciiCom.h
 *
 * @brief Handles ASCII-based telegram communication for signal control.
 *
 * The AsciiCom class is responsible for receiving,
 * processing, and responding to ASCII telegrams related to signal configuration
 * and status queries. The supported telegrams allow setting and getting signal
 * aspects, blinks, targets, input classification, and change-over times, as well
 * as copying signal configurations between IDs.
 *
 * Example telegrams:
 * - SET_SIGNAL 0 ASPECTS 11000 00100 00000 00000 00000
 * - SET_SIGNAL 0 BLINKS  00110 11001 11111 00000 00000
 * - SET_SIGNAL 0 TARGETS ONBOARD pin ONBOARD pin ONB pin
 * - SET_SIGNAL 0 INPUT CLASSIFIED 1
 * - SET_SIGNAL 0 COT 10 20
 * - GET_SIGNAL 0 ASPECTS
 * - GET_SIGNAL 0 TARGETS
 * - SET_CLASSIFIER 1 PIN 5
 * - SET_CLASSIFIER 1 LIMITS 2 10 20
 * - SET_CLASSIFIER 1 DEBOUNCE 5
 * - GET_CLASSIFIER 1 PIN
 * - GET_CLASSIFIER 1 LIMITS
 * - GET_CLASSIFIER 1 DEBOUNCE
 *
 * @note The signal ID is a number from 0 to 63. Aspects and blinks are bit values.
 * Target ports can be ONBOARD, ONB, EXTERN, or EXT, followed by a pin number (0-31).
 * Change-over times are two values in [10 ms].
 * 
 * @note Parts of the documentation of this file was created by GitHub Copilot.
 * 
 * @copyright Copyright 2024-2025 Ralf Sondershaus
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

#ifndef ASCIICOM_H_
#define ASCIICOM_H_

#include <Std_Types.h>
#include <Com/Observer.h>
#include <Com/SerAsciiTP.h>
#include <Util/Array.h>

namespace com
{

  // -----------------------------------------------------------------------------------
  /// Receives telegrams and processes them:
  ///
  /// SET_SIGNAL group:
  /// \code
  /// SET_SIGNAL id ASPECTS [aspect]+                          Set the aspects for a signal. 
  ///                                                          [aspects]+ can include up to five aspects. 
  ///                                                          If fewer than five aspects are provided, only the specified aspects are updated; the remaining aspects will stay unchanged
  /// SET_SIGNAL id BLINKS  [blink]+                           set (5) bits for blink (e.g. 00000)
  /// SET_SIGNAL id TARGETS [(ONBOARD,ONB,EXTERN, EXT) pin]+   set (5) target ports for signal id
  /// SET_SIGNAL id INPUT CLASSIFIED id                        set input: type is classifier with ID id
  /// SET_SIGNAL id COT t1 t2                                  set change over times
  /// \endcode
  ///
  /// id is the signal ID, which is a number from 0 to 63.
  /// aspect is a bit value, blink is a bit value, pin is a pin number (0-31).
  /// The target ports can be ONBOARD, ONB, EXTERN, or EXT, followed by a pin number.
  /// The change over times are two values in [10 ms].
  ///  
  /// Examples:
  /// \code
  /// SET_SIGNAL 0 ASPECTS 11000 00100 00110 11001 11111      sets aspects for signal 0
  /// SET_SIGNAL 0 BLINKS  00000 00000 00000 00000 00000      sets blinks for signal 0
  /// SET_SIGNAL 0 TARGETS ONBOARD pin ONBOARD pin ONB pin    sets target ports for signal 0
  /// SET_SIGNAL 0 INPUT CLASSIFIED 1                         sets input for signal 0 to classifier with ID 1
  /// SET_SIGNAL 0 COT 10 20                                  sets change over times for signal 0 to 100 ms and 200 ms
  /// \endcode
  ///
  /// \code
  /// GET_SIGNAL id ASPECTS                         returns all (5) aspects for signal id
  ///
  /// GET_SIGNAL 0 ASPECT                           
  /// GET_SIGNAL 0 TARGET                           returns all (5) target ports for signal 0
  ///
  /// SET_CLASSIFIER id [PIN, LIMITS, DEBOUNCE] ...
  /// \code
  // -----------------------------------------------------------------------------------
  class AsciiCom : public Observer
  {
  public:
    using char_type = SerAsciiTP::telegram_base_type;
    using string_type = SerAsciiTP::string_type;
    using size_type = string_type::size_type;


  protected:
    /// Currently, just one observer is supported
    util::ptr<SerAsciiTP> asciiTP;
    string_type telegram_response;

  public:
    AsciiCom() = default;

    void update() override;

    void listenTo(SerAsciiTP& tp) { asciiTP = &tp; tp.attach(*this); }

    void process(const string_type& telegram, string_type& response);

    void cycle();
  };
} // namespace com

#endif // ASCIICOM_H_