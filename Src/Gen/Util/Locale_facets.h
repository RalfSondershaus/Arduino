/**
  * @file Locale_facets.h
  *
  * @author Ralf Sondershaus
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
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef UTIL_LOCALE_FACETS_H
#define UTIL_LOCALE_FACETS_H

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Ios_base.h>  // util::ios_base
#include <Util/Ios_Fwd.h>   // util::streambuf_iterator
#include <Util/String.h>    // util::char_traits
#include <Util/Locale.h>    // util::locale::facet
#include <Util/Array.h>

namespace util
{
  // ---------------------------------------------------
  /// Base class of facet for character classification features.
  // ---------------------------------------------------
  class ctype_base : public locale::facet
  {
  public:
    typedef short mask;

    static constexpr mask kUP = 0x01; // 'A'-'Z'
    static constexpr mask kLO = 0x02; // 'a'-'z'
    static constexpr mask kDI = 0x04; // '0'-'9'
    static constexpr mask kCN = 0x08; // CR, FF, HT, NL, VT
    static constexpr mask kPU = 0x10; // punctuation
    static constexpr mask kBB = 0x20; // BEL, BS, etc.
    static constexpr mask kSP = 0x40; // space
    static constexpr mask kXD = 0x80; // '0'-'9', 'A'-'F', 'a'-'f'

    static constexpr mask alpha = kDI | kLO | kUP;
    static constexpr mask cntrl = kBB;
    static constexpr mask digit = kDI;
    static constexpr mask lower = kLO;
    static constexpr mask print = kDI | kLO | kPU | kSP | kUP | kXD;
    static constexpr mask punct = kPU;
    static constexpr mask space = kCN | kSP;
    static constexpr mask upper = kUP;
    static constexpr mask xdigit = kDI;
    static constexpr mask alnum = alpha | digit;
    static constexpr mask graph = alnum | punct;

  protected:
    /// Returns the upper case letter of c if available. Returns c otherwise.
    static char upper_c(char c)
    {
      if ((c >= 'a') && (c <= 'z'))
      {
        c = 'A' + (c - 'a');
      }
      return c;
    }

    /// Returns the lower case letter of c if available. Returns c otherwise.
    static char lower_c(char c)
    {
      if ((c >= 'A') && (c <= 'Z'))
      {
        c = 'a' + (c - 'A');
      }
      return c;
    }

    /// Returns true if c is a space character
    static bool isspace_c(char c)
    {
      bool ret;
      switch (c)
      {
      case ' ':  // space (0x20)
      case '\f': // form feed (0x0c)
      case '\n': // line feed (0x0a)
      case '\r': // carriage return (0x0d)
      case '\t': // horizontal tab (0x09)
      case '\v': // vertical tab (0x0b)
        ret = true;
        break;
      default:
        ret = false;
        break;
      }
      return ret;
    }
  };

  template<class CharT>
  class ctype : public ctype_base
  {
  public:
    using char_type = CharT;

    /// the unique facet ID
    static locale::id id;

    // standard
    char_type        widen(char c) const { return do_widen(c); }
    const char_type* widen(const char* beg, const char* end, char_type* dst) const { return do_widen(beg, end, dst); }
    char        narrow(char_type c, char dflt) const { return do_narrow(c, dflt); }
    bool        is(mask m, char_type c) const { return do_is(m, c); }
    char_type   toupper(char_type c) const { return do_toupper(c); }
    char_type   tolower(char_type c) const { return do_tolower(c); }

    // extensions
    bool isspace(char_type c) const { return do_isspace(c); }

  protected:
    // standard
    virtual char_type  do_widen(char c) const { return static_cast<char_type>(static_cast<unsigned char>(c)); }
    virtual const char* do_widen(const char* beg, const char* end, char_type* dst) const
    {
      for (; beg != end; beg++, dst++)
      {
        *dst = widen(*beg);
      }
      return beg;
    }
    virtual char do_narrow(char_type c, char dflt) const { (void)dflt; return static_cast<char>(static_cast<unsigned char>(c)); }
    virtual bool do_is(mask m, char_type c) const { (void)m; (void)c; return false; } // not implemented yet
    virtual char_type do_toupper(char_type c) const
    {
      return do_widen(upper_c(do_narrow(c, '\0')));
    }
    virtual char_type do_tolower(char_type c) const
    {
      return do_widen(lower_c(do_narrow(c, '\0')));
    }

    // extensions

    /// Returns true if c is a space character
    virtual bool do_isspace(char_type c) const
    {
      return isspace_c(do_narrow(c, '\0'));
    }
  };

  // ---------------------------------------------------
  /// Specialization for character type char.
  // ---------------------------------------------------
  template<>
  class ctype<char> : public ctype_base
  {
  public:
    using char_type = char;

    /// the unique facet ID
    static locale::id id; // { locale::id::kCType };

    // standard
    char_type   widen(char c) const { return do_widen(c); }
    const char* widen(const char* beg, const char* end, char_type* dst) const { return do_widen(beg, end, dst); }
    char        narrow(char_type c, char dflt) const { return do_narrow(c, dflt); }
    bool        is(mask m, char_type c) const { return do_is(m, c); }
    char_type   toupper(char_type c) const { return do_toupper(c); }
    char_type   tolower(char_type c) const { return do_tolower(c); }

    // extensions
    bool isspace(char_type c) const { return do_isspace(c); }

  protected:
    // standard
    virtual char_type  do_widen(char c) const { return c; }
    virtual const char* do_widen(const char* beg, const char* end, char_type* dst) const 
    {
      for (;beg != end; beg++, dst++)
      {
        *dst = widen(*beg);
      }
      return beg;
    }
    virtual char do_narrow(char_type c, char dflt) const { (void)dflt; return c; }
    virtual bool do_is(mask m, char_type c) const { (void)m; (void)c; return false; }
    virtual char_type do_toupper(char_type c) const 
    { 
      char_type cret = c;

      if ((c >= 'a') && (c <= 'z'))
      {
        cret = 'A' + (c - 'a');
      }

      return cret;
    }
    virtual char_type do_tolower(char_type c) const 
    {
      char_type cret = c;

      if ((c >= 'A') && (c <= 'Z'))
      {
        cret = 'a' + (c - 'A');
      }

      return cret;
    }

    // extensions

    /// Returns true if c is a space character
    virtual bool do_isspace(char_type c) const 
    {
      bool ret;
      switch (c)
      {
        case ' ':  // space (0x20)
        case '\f': // form feed (0x0c)
        case '\n': // line feed (0x0a)
        case '\r': // carriage return (0x0d)
        case '\t': // horizontal tab (0x09)
        case '\v': // vertical tab (0x0b)
          ret = true;
          break;
        default:
          ret = false;
          break;
      }

      return ret;
    }
  };

  /// The (unique) ID of ctype (but not for specializations)
  template<class CharT>
  locale::id ctype<CharT>::id;

  // ---------------------------------------------------
  /// Facet for numeric punctuation preferences.
  /// Has specializations, see below.
  // ---------------------------------------------------
  template<class CharT>
  class numpunct : public locale::facet
  {
  public:
    using char_type = CharT;

    /// the unique facet ID
    static locale::id id; // = locale::id::kNumPunct;

    // standard
    char_type   decimal_point() const { return do_decimal_point(); }
    char_type   thousands_sep() const { return do_thousands_sep(); }

  protected:
    // standard
    virtual char_type   do_decimal_point() const {}
    virtual char_type   do_thousands_sep() const {}
  };

  // ---------------------------------------------------
  /// Facet for numeric punctuation preferences.
  /// Has specializations, see below.
  // ---------------------------------------------------
  template<>
  class numpunct<char> : public locale::facet
  {
  public:
    using char_type = char;

    /// the unique facet ID
    static locale::id id; // = locale::id::kNumPunct;

    // standard
    char_type   decimal_point() const { return do_decimal_point(); }
    char_type   thousands_sep() const { return do_thousands_sep(); }

  protected:
    // standard
    virtual char_type   do_decimal_point() const { return '.'; }
    virtual char_type   do_thousands_sep() const { return ','; }
  };

  /// The (unique) ID of numpunct (but not for specializations)
  template<class CharT>
  locale::id numpunct<CharT>::id;

  // ---------------------------------------------------
  /// 
  // ---------------------------------------------------
  template<class CharT, class InputIt>
  class num_get : public locale::facet
  {
  public:
    using char_type = CharT;
    using iter_type = InputIt;
    using traits_type = typename iter_type::traits_type;
    using c_type = typename util::ctype<char_type>;
    using numpunct_type = typename util::numpunct<char_type>;

    static constexpr size_t kMaxIntDigits = 32U;

    /// the unique facet ID
    static locale::id id; // { locale::id::kNumGet };

  public:
    /// Parses the sequences of characters between start and end for a numerical value, and stores it into v.
    /// It uses formatting options selected in str. When necessary, it updates state.
    /// Implementation for signed types.
    template<typename T>
    iter_type gets(iter_type start, iter_type end, ios_base& iosbase, ios_base::iostate& state, T& val) const
    {
      char acBuf[kMaxIntDigits];
      char* pEnd;
      int nBase;
      util::tStringError strerr;

      nBase = put_integer(acBuf, kMaxIntDigits, start, end, iosbase.flags(), iosbase.getloc());

      // On success, convert integer
      if (acBuf[0] != 0)
      {
        val = util::strtoi<T>(acBuf, &pEnd, nBase, &strerr);
        if (strerr != tStringError::ERR_NONE)
        {
          state = ios_base::failbit;
          val = 0;
        }
      }
      else
      {
        state = ios_base::failbit;
        val = 0;
      }

      if (start == end)
      {
        state |= ios_base::eofbit;
      }

      return start;
    }

    // unsigned types
    template<typename T>
    iter_type getu(iter_type start, iter_type end, ios_base& str, ios_base::iostate& state, T& val) const
    {
      char acBuf[kMaxIntDigits];
      char* pEnd;
      int nBase;
      util::tStringError strerr;

      nBase = put_integer(acBuf, kMaxIntDigits, start, end, str.flags(), str.getloc());

      // On success, convert integer
      if (acBuf[0] != 0)
      {
        const char* pBuf = acBuf;
        // C99 7.20.1.4: ... optionally preceded by a plus or minus sign ...
        if (*pBuf == '-')
        {
          pBuf++;
        }
        val = util::strtoui<T>(pBuf, &pEnd, nBase, &strerr);
        if (strerr != tStringError::ERR_NONE)
        {
          state = ios_base::failbit;
          val = 0;
        }
      }
      else
      {
        state = ios_base::failbit;
        val = 0;
      }

      if (start == end)
      {
        state |= ios_base::eofbit;
      }

      return start;
    }

  protected:
    /// Returns true if c7 is
    /// - 0-7
    /// - 8, 9 if nBase is 10 or 16
    /// - A-F if nBase is 16
    /// - + or -
    static bool is_digit(char c7, int nBase)
    {
      bool ret;

      if ((c7 >= '0') && (c7 <= '7'))
      {
        ret = true;
      }
      else if ((c7 == '+') || (c7 == '-'))
      {
        ret = true;
      }
      else if ((nBase >= 8) && (c7 >= '8') && (c7 <= '9'))
      {
        ret = true;
      }
      else if ((nBase == 16) && (c7 >= 'A') && (c7 <= 'F'))
      {
        ret = true;
      }
      else
      {
        ret = false;
      }

      return ret;
    }
    /// Convert a character c into a 7 bit ASCII character
    /// Returns true if character is equvalent to 0-9, a-f, A-F, x, X, +, or -.
    static char narrow(iter_type& it, const c_type& facet, char dflt = '\0')
    {
      return facet.narrow(traits_type::to_char_type(*it), dflt);
    }

    /// Iterates from first to end and stores the integer into pBuffer.
    /// pBuffer needs to be of sufficient size.
    /// The characters from first to end are mapped to ASCII7 characters in pBuffer using put_char.
    static int put_integer(char* pBuffer, size_t len, iter_type& first, iter_type& end, ios_base::fmtflags fmt, const locale& loc)
    {
      int nBase;
      char* pBuf = pBuffer;
      const char* const pBufEnd = pBuffer + (len - static_cast<size_t>(1)); // reserve 1 byte for trailing '\0'
      // possible underflow is handled in final if statement
      bool hasDigit = false;
      char c;

      const c_type& ctype_fac = use_facet<c_type>(loc);
      const char_type thousands_sep = use_facet<numpunct_type>(loc).thousands_sep();

      fmt = fmt & util::ios_base::basefield;

      // copy leading plus sign and minus sign
      if (first != end)
      {
        c = narrow(first, ctype_fac);
        if ((c == '+') || (c == '-'))
        {
          *pBuf++ = c;
          ++first; // this call might wait until a new char is received
        }
      }

      if (fmt == util::ios_base::oct)
      {
        nBase = 8;
      }
      else if (fmt == util::ios_base::hex)
      {
        nBase = 16;
      }
      else
      {
        nBase = (fmt == util::ios_base::dec) ? 10 : 0;
      }

      // check leading 0, 0x and 0X, identify base if requested (nBase is zero, fmt resp.)
      // skip 0, 0x, or 0X because nBase will be 8 or 16 if there is a leading 0, 0x, or 0X
      // and util::strtoui, strzoi can deal with skipped leading 0, 0x, 0X if nBase is non-zero.
      if (first != end)
      {
        c = narrow(first, ctype_fac);
        if (c == '0')
        {
          ++first;
          hasDigit = true; // at least one digit is available, special case "0"
          if (first != end)
          {
            c = narrow(first, ctype_fac);
            if (((c == 'X') || (c == 'x')) && ((nBase == 0) || (nBase == 16)))
            {
              nBase = 16;
              ++first;
              hasDigit = false; // revert: digit was part of 0x or 0X
            }
            else 
            {
              if (nBase == 0)
              {
                nBase = 8;
              }
              // special case "0" or "0 ": first points to a non-digit element,
              // so the single 0 is stored here (because later while loop does
              // store it only if the character is a digit).
              if (!is_digit(c, nBase) && (pBuf < pBufEnd))
              {
                *pBuf++ = c;
              }
            }
          }
          else
          {
            // special case "0" and first == end (e.g. end points to the end
            // of the search area).
            *pBuf++ = c;
          }
        }
      }

      while (first != end)
      {
        c = narrow(first, ctype_fac);
        if (is_digit(c, nBase) && (pBuf < pBufEnd))
        {
          *pBuf++ = c;
          ++first;
          hasDigit = true;
        }
        else if (*first == thousands_sep)
        {
          // to do: check grouping (number of digits between thousands separators)
          ++first;
        }
        else
        {
          // invalid character, stop to copy characters
          break;
        }
      }

      // If first is EOF, first still has a pointer to the buffer. 
      // Force first == end to enable calling functions to check for first == end.
      // (first == end if both first and end have a pointer to a buffer, or both don't have a pointer to a buffer.)
      if ((first != end) && traits_type::eq_int_type(traits_type::eof(), *first))
      {
        ++first;
      }

      if ((!hasDigit) || (len == 0))
      {
        // no digit found
        pBuffer[0] = '\0';
      }
      else
      {
        // above checks ensure that one element is left to store trailing '\0'
        *pBuf = '\0';
      }
      return nBase;
    }
  };

  /// The (unique) ID of num_get
  template<class CharT, class InputIt>
  locale::id num_get<CharT, InputIt>::id;

} // namespace util

#endif // UTIL_LOCALE_FACETS_H