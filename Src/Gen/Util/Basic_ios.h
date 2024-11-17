/**
  * @file Basic_ios.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Declares and defines util::basic_ios.
  *
  * @copyright Copyright 2023, 2024 Ralf Sondershaus
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
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef UTIL_BASIC_IOS_H
#define UTIL_BASIC_IOS_H

#include <Std_Types.h>
#include <Util/Ios_base.h>
#include <Util/String.h>   // char_traits

namespace util
{

  // ----------------------------------------------------------------------------------------------------
  /// The class std::basic_ios provides facilities for interfacing with objects that have 
  /// std::basic_streambuf interface. Several std::basic_ios objects can refer to one actual 
  /// std::basic_streambuf object.
  /// 
  /// Default Traits type is defined in Ios_Fwd.h
  // ----------------------------------------------------------------------------------------------------
  template<class CharT, class Traits>
  class basic_ios : public ios_base
  {
  public:
    using char_type = CharT;
    using traits_type = Traits;
    using int_type = typename Traits::int_type;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;
    using basic_streambuf_type = basic_streambuf<char_type, traits_type>;

  protected:
    /// The internal state is not initialized. 
    /// init() must be called before the first use of the object or before destructor, 
    /// otherwise the behavior is undefined.
    basic_ios() {}

    /// Sets the associated stream buffer to sb and initializes the internal state.
    /// The postconditions of this function are indicated as follows:
    /// rdbuf()    sb
    /// rdstate()  goodbit if sb is not a null pointer, otherwise badbit.
    /// flags()    skipws | dec
    void init(basic_streambuf_type* sb)
    {
      my_sb = sb;
      clear((sb == nullptr) ? badbit : goodbit);
      setf(dec, basefield);
    }

    /// the associated stream buffer
    basic_streambuf_type* my_sb;

  public:
    /// Initializes the internal state by calling init(sb). The associated stream buffer is set to sb
    basic_ios(basic_streambuf_type* sb);

    /// I/O streams are not CopyConstructible
    basic_ios(const basic_ios&) = delete;

    /// the copy assignment operator is deleted
    basic_ios& operator=(const basic_ios&) = delete;

    /// Returns true if the most recent I / O operation on the stream completed successfully.
    bool good() const { return rdstate() == goodbit; }

    /// Returns true if the associated stream has reached end-of-file.
    bool eof() const { return rdstate() & eofbit; }

    /// Returns true if an error has occurred on the associated stream
    /// Specifically, returns true if badbit or failbit is set in rdstate().
    bool fail() const { return rdstate() & (failbit | badbit); }

    /// Returns true if an error has occurred on the associated stream. 
    /// Specifically, returns true if badbit or failbit is set in rdstate().
    bool operator!() const { return fail(); }

    /// Checks whether the stream has no errors
    explicit operator bool() const { return !fail(); }

    /// Returns the current stream error state
    iostate rdstate() const { return my_iostate; }

    /// Sets the stream error flags state in addition to currently set flags
    void setstate(iostate state) { clear(rdstate() | state); }

    /// Sets the stream error state flags by assigning them the value of state
    void clear(iostate state = goodbit) { my_iostate = state; }

    /// Returns the associated stream buffer. If there is no associated stream buffer, returns a null pointer
    basic_streambuf_type* rdbuf() const { return my_sb; }

    /// Sets the associated stream buffer to sb. The error state is cleared by calling clear(). 
    /// Returns the associated stream buffer before the operation. 
    /// If there is no associated stream buffer, returns a null pointer.
    basic_streambuf_type* rdbuf(basic_streambuf_type* sb)
    {
      basic_streambuf_type p = my_sb;
      my_sb = sb;
      clear();
      return p;
    }

    /// Sets the current locale to loc.
    /// If rdbuf() is not empty, calls rdbuf()->pubimbue to set the locale of the
    /// associated stream buffer.
    /// Returns the old locale of the stream.
    locale imbue(const locale& loc)
    {
      locale old_locale = ios_base::imbue(loc);
      const auto pBuf = rdbuf();
      if (pBuf != nullptr)
      {
        pBuf->pubimbue(loc);
      }
      return old_locale;
    }
  };
} // namespace util

#endif // UTIL_IOS_H