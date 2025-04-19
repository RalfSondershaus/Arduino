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

    /// Return values of process() function family
    typedef enum
    {
      eOK = 0,                    ///< OK
      eINV_CMD,                   ///< Command invalid (or unknown)
      eERR_EEPROM,                ///< EEPROM update failure
      eINV_SIGNAL_ID,             ///< SIGNAL ID invalid
      eINV_SIGNAL_CMD,
      eINV_SIGNAL_ASPECTS,
      eINV_SIGNAL_BLINKS,
      eINV_SIGNAL_TARGETS,
      eINV_SIGNAL_INPUT,
      eINV_SIGNAL_CHANGEOVERTIME,
      eERR_UNKNOWN
    } tRetType;

    /// For each tRetType, an error description that is transmitted after
    /// processing the command.
    static constexpr const string_type::value_type* aRetTypeStrings[] = 
    {
      "OK",                                   // eOK
      "ERR: Invalid command",                 // eINV_CMD
      "ERR: EEPROM failure",                  // eERR_EEPROM
      "ERR: Signal id invalid",               // eINV_SIGNAL_ID
      "ERR: Unknown signal sub command",      // eINV_SIGNAL_CMD
      "ERR: Unknown signal aspects",          // eINV_SIGNAL_ASPECTS
      "ERR: Unknown signal blinks",           // eINV_SIGNAL_BLINKS
      "ERR: Unknown signal targets",          // eINV_SIGNAL_TARGETS
      "ERR: Unknown signal input",            // eINV_SIGNAL_INPUT
      "ERR: Unknown signal change over time", // eINV_SIGNAL_CHANGEOVERTIME
      "ERR: unknown error"                    // has to be the last element
    };

  protected:
    /// Currently, just one observer is supported
    util::ptr<SerAsciiTP> asciiTP;
    string_type response;

    //int process_set(stringstream_type& st);
    
  public:
    AsciiCom() = default;

    void update() override;

    void listenTo(SerAsciiTP& tp) { asciiTP = &tp; tp.attach(*this); }

    tRetType process(const string_type& telegram);
  };
} // namespace com

#endif // ASCIICOM_H_