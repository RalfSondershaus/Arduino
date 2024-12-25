/**
 * @file Signal/Com/SerAsciiTP.h
 *
 * @brief 
 *
 * @copyright Copyright 2023 Ralf Sondershaus
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

#ifndef SERASCIITP_H_
#define SERASCIITP_H_

#include <Std_Types.h>
#include <Com/SerComDrv.h>
#include <Com/Subject.h>
#include <Util/String.h>
#include <Util/Ptr.h>

namespace com
{
  // -----------------------------------------------------------------------------------
  /// The main task of SerAsciiTP is to assemble the message until the
  /// message is complete (terminating '\n'). When the message has
  /// been received completely, the message is forwarded to the next
  /// level. 
  ///
  /// So, AsciiTP assembles the message until '\n' is received and forward
  /// the message afterwards.
  // -----------------------------------------------------------------------------------
  class SerAsciiTP : public Subject
  {
  public:
    static constexpr size_t kMaxLenTelegram = 64U;

    /// A telegram consists of max kMaxLenTelegram uint8's    
    using telegram_base_type = char;
    using string_type = typename util::basic_string<kMaxLenTelegram, telegram_base_type>;

    /// Store the raw data of one telegram
    string_type telegram_rawdata;

  protected:
    /// The communication driver, can be null, data are fetched from this driver
    util::ptr<SerComDrv> driver;
    bool bOverflow;
    /// Returns true if ch is a control character.
    /// TODO Utilize util::iscntrl
    bool iscntrl(int ch) { return ch == '\n'; }
  public:
    /// Construct.
    SerAsciiTP() = default;

    /// @brief Construct for the given driver
    /// @param drv The communication driver that shall be used to fetch data
    SerAsciiTP(SerComDrv& drv) : driver(&drv), bOverflow(false)
    {}

    /// Initialization
    void init();
    /// Receive data from low level drivers and process them
    void cycle();

    /// Returns a reference to the telegram raw data.
    const string_type& getTelegram() const noexcept { return telegram_rawdata; }
  };

} // namespace com

#endif // COMR_H_