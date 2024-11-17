/**
  * @file Locale_Fwd.h
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

#ifndef UTIL_LOCALE_FWD_H
#define UTIL_LOCALE_FWD_H

#include <Std_Types.h>
#include <Util/Ios_Fwd.h>     // istreambuf_iterator

namespace util
{
  class locale;

  class ctype_base;

  template<class CharT>
  class ctype;

  template<class CharT>
  class numpunct;

  template<class CharT, class InputIt = istreambuf_iterator<CharT> >
  class num_get;
} // namespace util

#endif // UTIL_LOCALE_FWD_H