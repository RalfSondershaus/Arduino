/**
  * @file Ios_Fwd.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Forward declarations.
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