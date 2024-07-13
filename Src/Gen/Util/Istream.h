/**
  * @file String.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines util::basic_istream
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

#ifndef UTIL_ISTREAM_H
#define UTIL_ISTREAM_H

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Ios_base.h>      // util::ios_base
#include <Util/Iterator.h>      // util::char_traits
#include <Util/Locale_facets.h> // util::num_get
#include <Util/StreamBuf.h>     // util::basic_streambuf

namespace util
{
  // ---------------------------------------------------
  /// Provides support for high-level input operations on character streams.
  /// 
  /// Default Traits type is defined in Ios_Fwd.h
  // ---------------------------------------------------
  template<class CharT, class Traits>
  class basic_istream : public basic_ios<CharT, Traits>
  {
  public:
    using char_type = CharT;
    using traits_type = Traits;
    using int_type = typename Traits::int_type;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;
    using basic_streambuf_type = basic_streambuf<char_type, traits_type>;
    using iter_type = istreambuf_iterator<char_type, traits_type>;
    using num_get_type = util::num_get<char_type, iter_type>;
    using ctype_type = util::ctype<char_type>;
    using basic_ios_type = basic_ios<char_type, traits_type>;

  protected:
    /// Input streams are not copyable
    basic_istream(const basic_istream& rhs) = delete;
    basic_istream& operator=(const basic_istream& rhs) = delete;

    /// Number of extracted characters
    streamsize my_gcount;

  public:
    /// construct
    explicit basic_istream(basic_streambuf_type* sb)
    {
      basic_ios_type::init(sb);
    }

    /// destruct
    virtual ~basic_istream() = default;

    /// nested class. The constructor prepares the formatted input functions and the
    /// unformatted input functions.
    class sentry
    {
    public:
      basic_istream& mystr;
      bool myok;

      explicit sentry(basic_istream& str, bool noskip = false) : mystr(str), myok(str.ipfx(noskip)) {}
      sentry(const sentry& s) = delete;
      sentry& operator=(const sentry& s) = delete;
      explicit operator bool() const { return myok; }
    };

    /// check if stream is good() and skip white spaces. Returns good().
    /// White spaces are detected by util::isspace(CharT c) instead of
    /// std::use_facet.
    bool ipfx(bool noskip = false)
    {
      if (!good())
      {
        setstate(failbit);
      }
      else
      {
        if (!noskip)
        {
          int_type nc = rdbuf()->sgetc();
          const ctype_type& facet = util::use_facet<ctype_type>(getloc());

          while ((!traits_type::eq_int_type(traits_type::eof(), nc))
            && (facet.isspace(traits_type::to_char_type(nc))))
          {
            nc = rdbuf()->snextc();
          }

          if (traits_type::eq_int_type(traits_type::eof(), nc))
          {
            setstate(eofbit | failbit);
          }
        }
      }
      return good();
    }

    // Formatted input

    basic_istream& operator>>(sint16& value)
    {
      const sentry ok(*this);
      long val;
      iostate err = goodbit;
      my_gcount = 0;
      if (ok)
      {
        const num_get_type& facet = util::use_facet<num_get_type>(getloc());
        facet.gets(*this, 0, *this, err, val);
        if (val > platform::numeric_limits<sint16>::max_())
        {
          err |= failbit;
          value = platform::numeric_limits<sint16>::max_();
        }
        else if (val < platform::numeric_limits<sint16>::min_())
        {
          err |= failbit;
          value = platform::numeric_limits<sint16>::min_();
        }
        else
        {
          value = static_cast<sint16>(val);
        }
      }

      setstate(err);

      return *this;
    }

    basic_istream& operator>>(uint16& value)
    {
      const sentry ok(*this);
      unsigned long val;
      iostate err = goodbit;
      my_gcount = 0;
      if (ok)
      {
        const num_get_type& facet = util::use_facet<num_get_type>(getloc());
        facet.getu(*this, 0, *this, err, val);
        if (val > platform::numeric_limits<uint16>::max_())
        {
          err |= failbit;
          value = platform::numeric_limits<uint16>::max_();
        }
        else
        {
          value = static_cast<uint16>(val);
        }
      }

      setstate(err);

      return *this;
    }

    // Unformatted input

    /// Reads one character. 
    /// If available, returns the character and advances the input sequence by one character.
    /// Otherwise, returns Traits::eof() and sets failbit and eofbit.
    int_type get() 
    {
      const sentry ok(*this, true);
      int_type ret;
      my_gcount = 0;

      if (ok)
      {
        ret = rdbuf()->sgetc();
        if (traits_type::eq_int_type(ret, traits_type::eof()))
        {
          setstate(failbit | eofbit);
        }
        else
        {
          rdbuf()->sbumpc();
          my_gcount = 1;
        }
      }
      else
      {
        ret = traits_type::eof();
        setstate(failbit | eofbit);
      }
      return ret;
    }

    /// Reads one character and stores it to ch if available.
    /// Otherwise, leaves ch unmodified and sets failbit and eofbit.
    basic_istream& get(char_type& ch)
    {
      int_type n = get();
      if (!traits_type::eq_int_type(n, traits_type::eof()))
      {
        ch = traits_type::to_char_type(n);
      }
      return *this;
    }

    /// Same as get(s, count, widen('\n')), that is, reads at most std::max(0, count - 1) 
    /// characters and stores them into character string pointed to by s until '\n' is found.
    basic_istream& get(char_type* s, streamsize count)
    {
      const ctype_type& facet = util::use_facet<ctype_type>(getloc());
      return get(s, count, facet.widen('\n'));
    }

    /// Reads characters and stores them into the successive locations of the character 
    /// array whose first element is pointed to by s. Characters are extracted and stored 
    /// until any of the following occurs:
    /// - count is less than 1 or count - 1 characters have been stored.
    /// - end of file condition occurs in the input sequence (setstate(eofbit) is called).
    /// - the next available input character c equals delim, as determined by Traits::eq(c, delim). 
    ///   This character is not extracted (unlike getline()).
    basic_istream& get(char_type* s, streamsize count, char_type delim)
    {
      const sentry ok(*this, true);
      iostate state = goodbit;
      my_gcount = 0;

      if (ok)
      {
        int_type n = rdbuf()->sgetc();
        while (count > static_cast<streamsize>(1))
        {
          if (traits_type::eq_int_type(n, traits_type::eof()))
          {
            state |= eofbit;
            break;
          }
          else if (traits_type::eq(traits_type::to_char_type(n), delim))
          {
            break;
          }
          else
          {
            *s++ = traits_type::to_char_type(n);
            my_gcount++;
          }
          n = rdbuf()->snextc();
          count--;
        }
      }
      else
      {
        state |= failbit | eofbit;
      }

      // if no characters were extracted, sets failbit
      if (my_gcount == 0)
      {
        state |= failbit;
      }

      setstate(state);
      *s = char_type(); // terminating 0
      return *this;
    }

    /// Returns the number of characters extracted by the last unformatted input operation, 
    /// or the maximum representable value of std::streamsize if the number is not representable.
    util::streamsize gcount() const { return my_gcount;  }

#if 0
    // not implemented yet

    /// reads the next character without extracting it
    int_type peek();

    /// unextracts a character. Makes the most recently extracted character available again.
    basic_istringstream& unget();

    /// Puts the character ch back to the input stream so the next extracted character will be ch
    basic_istringstream& putback(char_type ch);

    /// extracts characters until the given character is found
    basic_istringstream& getline(char_type* s, std::streamsize count);
    basic_istringstream& getline(char_type* s, std::streamsize count, char_type delim);

    /// Extracts and discards characters from the input stream until and including delim.
    basic_istringstream& ignore(std::streamsize count = 1, int_type delim = Traits::eof());

    /// extracts blocks of characters
    basic_istringstream& read(char_type* s, util::streamsize count);

    /// extracts already available blocks of characters
    util::streamsize readsome(char_type* s, util::streamsize count);
#endif

    // Positioning

    /// Returns input position indicator of the current associated streambuf object.
    pos_type tellg()
    {
      pos_type p;
      const sentry ok(*this, true);
      if (ok)
      {
        p = rdbuf()->pubseekoff(0, ios_base::cur, ios_base::in);
      }
      else
      {
        p = static_cast<pos_type>(-1);
      }
      return p;
    }
#if 0
    /// Sets input position indicator of the current associated streambuf object.
    basic_istringstream& seekg(pos_type pos);

    /// Sets input position indicator of the current associated streambuf object.
    basic_istringstream& seekg(off_type off, std::ios_base::seekdir dir);
#endif
  };

} // namespace util

#endif // UTIL_ISTREAM_H