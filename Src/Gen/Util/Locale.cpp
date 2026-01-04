/**
  * @file Locale.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief 
  *
  * @copyright Copyright 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Ios_base.h>       // util::ios_base
#include <Util/Locale.h>
#include <Util/Locale_facets.h>
#include <Util/Locale_Fwd.h>     // for default template parameters
#include <Util/Istream.h>
//#include <iostream>

/// A variable without an initializer is default initialized. The default value
/// depends on the type of the variable and where the variable is defined.
/// 
/// The value of a variable of built-in type that is not explicitly initialized 
/// depends on where it is defined. Variables of built in types that are
/// defined outside any function body are initialized to zero, so the default value
/// is zero.
/// 
/// All non-local variables with static storage duration are initialized as part of
/// program startup. The initialization occurs in two stages.
/// (1) static initialization
/// - Either 
/// -- constant initialization or 
/// -- zero-initialization
/// (2) dynamic initialization
/// - Unordered dynamic initialization
/// -- applies only to class template static data members that are not explicitly specialized
/// -- initialization is indeterminately sequenced to all other dynamic initialization
/// - Ordered dynamic initialization
/// -- applies to all other non-local variables
/// -- within a single translation unit, initialization is always sequenced in exact order 
///    their definitions appear in the source code
/// -- initialization in different translation units is indeterminately sequenced
/// 
/// For example: constructors are called as part of dynamic initialization and after static
/// initialization.
/// 
/// See also https://en.cppreference.com/w/cpp/language/initialization
namespace util
{
  struct
  {
    num_get<char> num_get_;
    numpunct<char> numpunct_;
    ctype<char> ctype_;
  } global_facets;

  locale::facet_array_type facets;

  // The global locale
  locale locale::global_locale;

  // The ID counter that is incremented with each facet class
  int locale::id::id_cnt;

  // The (unique) IDs of specializations
  locale::id numpunct<char>::id;
  locale::id ctype<char>::id;

  /// Construct with default facets
  locale::locale() noexcept : facets_pointer(&facets)
  {
    construct(); 
  }

  void locale::construct()
  {
    facets.at(num_get<char>::id ) = &global_facets.num_get_;
    facets.at(ctype<char>::id   ) = &global_facets.ctype_;
    facets.at(numpunct<char>::id) = &global_facets.numpunct_;
  }
} // namespace util
