/**
  * @file Locale_Fwd.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Forward declarations for locale classes.
  *
  * @copyright Copyright 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
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