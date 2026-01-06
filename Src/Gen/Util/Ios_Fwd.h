/**
  * @file Ios_Fwd.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Forward declarations.
  *
  * @copyright Copyright 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_IOS_FWD_H
#define UTIL_IOS_FWD_H

#include <Std_Types.h>

namespace util
{
  // ---------------------------------------------------
  /// String.h
  // ---------------------------------------------------
  template<class CharT> class char_traits;
  template<>            class char_traits<char>;

  // ---------------------------------------------------
  /// Ios.h
  // ---------------------------------------------------
  class ios_base;
  template<class CharT, class Traits = char_traits<CharT> > class basic_ios;

  // ---------------------------------------------------
  /// StreamBuf.h
  // ---------------------------------------------------
  template<class CharT, class Traits = char_traits<CharT> > class basic_streambuf;

  // ---------------------------------------------------
  /// istreambuf_iterator
  // ---------------------------------------------------
  template< class CharT, class Traits = char_traits<CharT> > class istreambuf_iterator;

  // ---------------------------------------------------
  /// Istream.h
  // ---------------------------------------------------
  template<class CharT, class Traits = char_traits<CharT> > class basic_istream;
  template <class T, class CharT = char, class Traits = util::char_traits<CharT>, class Distance = ptrdiff_t> class istream_iterator;
}

#endif // UTIL_IOS_FWD_H