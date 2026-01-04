/**
  * @file StreamBuf.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines util::basic_streambuf
  *
  * @copyright Copyright 2023, 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_STREAMBUF_H
#define UTIL_STREAMBUF_H

#include <Std_Types.h>
#include <Util/Ios_base.h>       // streamsize, ios_base
#include <Util/Locale.h>

namespace util
{
  // --------------------------------------------------------------------------
  /// Abstracts a stream buffer with get area (read) and set area (write).
  /// 
  /// It controls input and output to a character sequence.
  /// 
  /// Default Traits type is defined in Ios_Fwd.h
  // --------------------------------------------------------------------------
  template<class CharT, class Traits>
  class basic_streambuf
  {
  public:
    using char_type = CharT;
    using traits_type = Traits;
    using int_type = typename Traits::int_type;
    using pos_type = typename Traits::pos_type;
    using off_type = typename Traits::off_type;

    /// The base class version of this function has no effect. 
    /// The derived classes may override this function to allow removal or 
    /// replacement of the controlled character sequence (the buffer) with 
    /// a user-provided array, or for any other implementation-specific 
    /// purpose.
    basic_streambuf* pubsetbuf(char_type* s, streamsize n)
    { 
      return setbuf(s, n); 
    }

    /// Sets the position indicator to a relative position (off).
    pos_type pubseekoff(off_type off, ios_base::seekdir dir, ios_base::openmode which = ios_base::in | ios_base::out)
    {
      return seekoff(off, dir, which);
    }

    /// Sets the position indicator to an absolute position (pos).
    pos_type pubseekpos(pos_type pos, ios_base::openmode which = ios_base::in | ios_base::out)
    {
      return seekpos(pos, which);
    }

    /// Synchronizes the controlled character sequence (the buffers) with the associated character sequence.
    int pubsync() { return sync(); }

    // get area

    /// Returns the number of characters available in the get area. 
    streamsize in_avail()
    {
      const streamsize sz = gavail();
      return (sz > 0) ? sz : showmanyc();
    }

    /// Advances the input sequence by one character and reads this (next) character.
    int_type snextc()
    {
      return (sbumpc() == traits_type::eof()) ? traits_type::eof() : sgetc();
    }

    /// Reads one character and advances the input sequence by one character afterwards.
    int_type sbumpc()
    {
      return (gavail() > 0) ? traits_type::to_int_type(inc_gptr()) : uflow();
    }

    /// Reads one character from the input sequence. Does not advance the input sequence.
    int_type sgetc()
    {
      return (gavail() > 0) ? traits_type::to_int_type(*gptr()) : underflow();
    }

    /// Reads count characters from the input sequence and stores them into a 
    /// character array pointed to by s.
    streamsize sgetn(char_type* s, streamsize count) { return xsgetn(s, count); }

    /// Sets the associated locale
    locale pubimbue(const locale& loc) { imbue(loc); return getloc(); }

    /// Returns the associated locale.
    /// If pubimbue has never been called, returns the global locale at the time
    /// of the construction of the stream buffer.
    locale getloc() const { return my_locale; }

  protected:

    /// While the names "gptr" and "egptr" refer to the get area, the name "eback" 
    /// refers to the end of the putback area: stepping backwards from gptr, 
    /// characters can be put back until eback.
    /// my_eback: pointer to the beginning of the get area
    /// my_gptr : pointer to the current character (get pointer) in the get area
    /// my_egptr: pointer one past the end of the get area.
    char_type* my_eback;
    char_type* my_gptr;
    char_type* my_egptr;

    /// the locale of the stream buffer.
    /// Is initialized with the global locale at the time of the construction
    /// of the stream buffer.
    locale my_locale;

    /// Remove or replace the controlled character sequence (the buffer) with 
    /// a user-provided array.
    /// The base class version of this function has no effect. 
    virtual basic_streambuf* setbuf(char_type* s, streamsize n)
    {
      (void)s;
      (void)n;
      return nullptr;
    }

    /// Sets the position indicator to a relative position (off).
    /// The base class version of this function has no effect. 
    virtual pos_type seekoff(off_type off, ios_base::seekdir dir, ios_base::openmode which = ios_base::in | ios_base::out)
    {
      (void)off;
      (void)dir;
      (void)which;
      return static_cast<pos_type>(-1);
    }

    /// Sets the position indicator to an absolute position.
    /// The base class version of this function has no effect.
    virtual pos_type seekpos(pos_type pos, ios_base::openmode which = ios_base::in | ios_base::out)
    {
      (void)pos;
      (void)which;
      return static_cast<pos_type>(-1);
    }

    /// Synchronizes the controlled character sequence (the buffers) with the associated character sequence.
    /// The base class version of this function has no effect. 
    virtual int sync() { return 0; }

    /// Increment the get pointer by one. Return the value at the get pointer before
    /// the get pointer is increased.
    /// Does not check if the get pointer is past the pointer to the end of the get area.
    char_type inc_gptr() { return *my_gptr++; }

    // get area

    /// Returns the pointer to the beginning of the get area.
    char_type* eback() const { return my_eback; }
    /// Returns the pointer to the current character (get pointer) in the get area.
    char_type* gptr() const { return my_gptr; }
    /// Returns the pointer one past the end of the get area.
    char_type* egptr() const { return my_egptr; }

    /// Returns the number of characters that are available in the read buffer
    streamsize gavail() const { return egptr() - gptr(); }

    /// Skips count characters in the get area. This is done by adding count to 
    /// the get pointer. No checks are done for underflow.
    void gbump(int count) { my_gptr += count; }

    /// Sets the values of the pointers defining the get area. 
    /// @param gbeg   pointer to the beginning of the get area
    /// @param gcurr  pointer to the current character (get pointer) in the get area
    /// @param gend   pointer one past the end of the get area
    void setg(char_type* gbeg, char_type* gcurr, char_type* gend)
    {
      my_eback = gbeg;
      my_gptr = gcurr;
      my_egptr = gend;
    }

    /// Estimates the number of characters available for input in the associated character sequence. 
    /// Guarantees that Traits::eof() is never returned.
    /// The name of this function stands for "stream: how many characters?", so it is pronounced 
    /// "S how many C", rather than "show many C".
    virtual streamsize showmanyc() { return static_cast<streamsize>(0); }

    /// Reads more data in from the input sequence by updating the pointers to the input area.
    /// 
    /// On success, returns the first character of the newly loaded data (converted 
    /// to int_type with Traits::to_int_type(c)). The get pointer points to 
    /// the first character.
    /// 
    /// On failure, returns Traits::eof(). The function ensures that either 
    /// gptr() == nullptr or gptr() == egptr.
    /// 
    /// The base class version of the function does nothing.
    virtual int_type underflow() { return traits_type::eof(); }

    /// Reads more data in from the input sequence by updating the pointers to the input area.
    /// 
    /// On success, returns the first character of the newly loaded data (converted 
    /// to int_type with Traits::to_int_type(c)) and advances the get pointer by 
    /// one character.
    /// 
    /// On failure, returns Traits::eof(). The function ensures that either 
    /// gptr() == nullptr or gptr() == egptr.
    /// 
    /// The base class version of the function calls underflow() and increments gptr().
    virtual int_type uflow() 
    {
      int_type c = underflow();
      if (c != traits_type::eof())
      {
        inc_gptr();
      }
      return c;
    }

    /// Reads count characters from the input sequence and stores them into 
    /// a character array pointed to by s. The characters are read as if by 
    /// repeated calls to sbumpc(). That is, if less than count characters are 
    /// immediately available, the function calls uflow() to provide more 
    /// until Traits::eof() is returned.
    /// Classes derived from std::basic_streambuf are permitted to provide 
    /// more efficient implementations of this function.
    virtual streamsize xsgetn(char_type* s, streamsize count)
    {
      int_type n = sgetc();
      const char_type* s_end = s + count;
      const char_type* const s_start = s;
      while ((s < s_end) && (n != traits_type::eof()))
      {
        *s = traits_type::to_char_type(n);
        ++s;
        n = sbumpc();
      }
      return s - s_start;
    }

    /// The base class implementation has no effect.
    /// Derived classes may override this function in order to be informed about
    /// the change of the locale.
    virtual void imbue(const locale& loc) { (void)loc; }

  };
} // namespace util

#endif // UTIL_STREAMBUF_H