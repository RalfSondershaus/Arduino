/**
  * @file ctype.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Character classification functions as classified by the currently installed C locale.
  *
  * @copyright Copyright 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UTIL_CTYPE_H
#define UTIL_CTYPE_H

#include <ctype.h>

namespace util
{
  /// isalnum: 
  /// - digits (0123456789)
  /// - uppercase letters (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
  /// - lowercase letters (abcdefghijklmnopqrstuvwxyz)

  using ::isalnum;
  using ::isalpha;
  using ::isspace;
  using ::iscntrl;
  using ::ispunct; // !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
} // namespace util

#endif // UTIL_CTYPE_H