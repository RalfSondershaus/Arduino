/**
 * @file Signal/Com/AsciiCom.h
 *
 * @brief
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

#ifndef ASCIICOM_H_
#define ASCIICOM_H_

#include <Std_Types.h>
#include <Com/Observer.h>
#include <Com/SerAsciiTP.h>
#include <Util/Array.h>

namespace com
{

  // -----------------------------------------------------------------------------------
  /// <payload>'\n'
  ///
  /// With <payload> such as:
  ///
  /// \code
  /// SET_SIGNAL id ASPECTS [aspect]+                          set (5) bits for aspect and blink (e.g. 11000)
  /// SET_SIGNAL id BLINKS  [blink]+                           set (5) bits for aspect and blink (e.g. 00000)
  /// SET_SIGNAL id TARGETS [(ONBOARD,ONB,EXTERN, EXT) pin]+   set (5) target ports for signal id
  /// SET_SIGNAL id INPUT CLASSIFIED id                        set input: type is classifier with ID id
  /// SET_SIGNAL id COT t1 t2                                  set change over times
  /// 
  /// SET_SIGNAL_ASPECTS    id 11000 00100 00110 11001 11111
  /// SET_SIGNAL_BLINKS     id 00000 00000 00000 00000 00000
  /// SET_SIGNAL_TARGETS    id ONB pin ONBOARD pin ONBOARD pin ONBOARD pin ONB pin
  /// SET_SIGNAL_INPUT      id ONBOARD class_id
  /// SET_SIGNAL_COT        id t1 t2
  ///
  /// GET_SIGNAL id ASPECTS                         returns all (5) aspects for signal id
  ///
  /// GET_SIGNAL 0 ASPECT                           
  /// GET_SIGNAL 0 TARGET                           returns all (5) target ports for signal 0
  ///
  /// CPY_SIGNAL 0 SIGNAL 1                         copies all aspects and target ports from signal 0 to signal 1
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
  };
} // namespace com

#endif // ASCIICOM_H_