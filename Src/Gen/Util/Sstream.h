/**
  * @file Sstream.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines I/O streams
  * 
  * @desc Defines classes
  * - basic_stringbuf
  * - basic_istringstream
  * - basic_ostringstream
  * - basic_stringstream
  *
  * @copyright Copyright 2023, 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_SSTREAM_H
#define UTIL_SSTREAM_H

#include <Std_Types.h>
#include <Util/String.h>    // util::char_traits, util::basic_string
#include <Util/StreamBuf.h> // util::basic_streambuf
#include <Util/Istream.h>   // util::basic_istream

namespace util
{
  // ---------------------------------------------------
  /// A stream buffer whose associated character sequence is a string.
  // ---------------------------------------------------
  template<int Size, class CharT, class Traits = char_traits<CharT> >
  class basic_stringbuf : public basic_streambuf<CharT, Traits>
  {
  public:
    using char_type = CharT;
    using traits_type = Traits;
    using string_type = basic_string<Size, CharT, Traits>;
    using basic_streambuf_type = basic_streambuf<CharT, Traits>;
    using pos_type = typename basic_streambuf_type::pos_type;
    using off_type = typename basic_streambuf_type::off_type;
    using int_type = typename basic_streambuf_type::int_type;

    static constexpr int kSize = Size;

  protected:
    string_type my_str;

    void init(const string_type& s, ios_base::openmode which)
    {
      my_str = s;

      if ((which & ios_base::in) != 0)
      {
        // set begin, current, end
        this->setg(my_str.begin(), my_str.begin(), my_str.end());
      }
    }

    /// The underlying buffer cannot be changed, so nullptr is returned.
    virtual basic_streambuf_type* setbuf(char_type* s, streamsize n) override
    {
      (void)s;
      (void)n;
      return nullptr;
    }

    /// Sets the position indicator to a position relative to beg, cur, or end.
    /// Repositions gptr and pptr depending on parameter which.
    pos_type seekoff(off_type off, ios_base::seekdir dir, ios_base::openmode which = ios_base::in | ios_base::out) override
    {
      off_type newoff;
      switch (dir)
      {
      case ios_base::beg: newoff = 0; break;
      case ios_base::end: newoff = this->egptr() - this->gptr(); break;
      case ios_base::cur: newoff = this->gptr() - this->eback(); break;
      default: newoff = 0; break;
      }

      char_type* const newgptr = this->eback() + (newoff + off);

      if ((newgptr < this->eback()) || (newgptr > this->egptr()))
      {
        newoff = static_cast<off_type>(-1);
      }
      else
      {
        if (which & ios_base::in)
        {
          this->setg(this->eback(), newgptr, this->egptr());
        }
      }

      return static_cast<pos_type>(newoff);
    }

    /// Sets the position indicator to the absolute position pos.
    /// Repositions gptr and pptr depending on parameter which.
    pos_type seekpos(pos_type pos, ios_base::openmode which = ios_base::in | ios_base::out) override
    {
      char_type* const newgptr = this->eback() + pos;

      if ((newgptr < this->eback()) || (newgptr >= this->egptr()))
      {
        pos = static_cast<off_type>(-1);
      }
      else
      {
        if (which & ios_base::in)
        {
          this->setg(this->eback(), newgptr, this->egptr());
        }
      }

      return pos;
    }

    /// Returns eof() because there are no more data to be loaded from a string
    virtual int_type underflow() override { return traits_type::eof(); }

    /// Returns eof() because there are no more data to be loaded from a string
    virtual int_type uflow() override { return traits_type::eof(); }

  public:

    // Construct
    basic_stringbuf() : basic_stringbuf(ios_base::in | ios_base::out) {}
    explicit basic_stringbuf(ios_base::openmode which) 
    {
      init(string_type{}, which);
    }
    explicit basic_stringbuf(const string_type& s, ios_base::openmode which = ios_base::in | ios_base::out) 
    {
      init(s, which);
    }

    // Access to buffer

    /// Returns a string object containing a copy of this basic_stringbuf's underlying character sequence.
    string_type str() const { return my_str; }
    
    /// Replaces the underlying character sequence as if by buf = s.
    template<int Size2>
    void str(const basic_string<Size2, char_type>& s) { init(s, ios_base::in | ios_base::out); }
  };

  // ---------------------------------------------------
  /// Input operations on string based streams.
  // ---------------------------------------------------
  template<int Size, class CharT, class Traits = char_traits<CharT> >
  class basic_istringstream : public basic_istream<CharT, Traits>
  {
  public:
    using char_type = CharT;
    using traits_type = Traits;
    using int_type = typename Traits::int_type;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;
    using string_type = basic_string<Size, char_type, traits_type>;
    using stringbuf_type = basic_stringbuf<Size, char_type, traits_type>;
    using basic_istream_type = basic_istream<char_type, traits_type>;

    /// The maximal size of the stream
    static constexpr size_t MaxLength = Size;

  protected:
    stringbuf_type my_strbuf;

  public:
    /// construct
    explicit basic_istringstream() : basic_istringstream(ios_base::in) {}
    explicit basic_istringstream(ios_base::openmode mode) : basic_istream_type(&my_strbuf), my_strbuf(mode) {}
    explicit basic_istringstream(const string_type& s, ios_base::openmode mode = ios_base::in) : basic_istream_type(&my_strbuf), my_strbuf(s, mode) {}

    stringbuf_type* rdbuf() const { return const_cast<stringbuf_type *>(&my_strbuf); }

    /// gets the contents of underlying string device object
    string_type str() const { return rdbuf()->str(); }
  };
} // namespace util

#endif // UTIL_SSTREAM_H