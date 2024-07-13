/**
  * @file Locale.cpp
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

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Ios.h>       // util::ios_base
#include <Util/Locale.h>
#include <Util/Locale_facets.h>
#include <iostream>

namespace util
{
  struct
  {
    num_get<char> num_get_;
    numpunct<char> numpunct_;
    ctype<char> ctype_;
  } global_facets;

  // The global locale
  locale locale::global_locale;

  // The ID counter that is incremented with each facet class
  int locale::id::id_cnt;

  // The (unique) IDs of specializations
  locale::id numpunct<char>::id;
  locale::id ctype<char>::id;

  void locale::construct()
  {
    facets.at(num_get<char>::id ) = &global_facets.num_get_;
    facets.at(ctype<char>::id   ) = &global_facets.ctype_;
    facets.at(numpunct<char>::id) = &global_facets.numpunct_;
  }
} // namespace util
