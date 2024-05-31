/**
  * @file Locale.h
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

#ifndef UTIL_LOCALE_H
#define UTIL_LOCALE_H

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Ios.h>       // util::ios_base
#include <Util/Iterator.h>  // util::begin, util::end, util::streambuf_iterator
#include <Util/String.h>    // util::char_traits
#include <Util/Array.h>
#include <iostream>

namespace util
{
  // ---------------------------------------------------
  /// Locale
  // ---------------------------------------------------
  class locale
  {
  public:
    // ---------------------------------------------------
    /// Base class for facets
    // ---------------------------------------------------
    class facet
    {
    public:
    };

    // ---------------------------------------------------
    /// Base class for facet IDs.
    /// 
    /// This class is used as static data member in facets.
    /// 
    /// The logic relies on the initialization sequence as defined by the C++ standard.
    /// The data member unique_id is default initialized to zero as part of the static
    /// initialization stage as described below.
    /// 
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
    // ---------------------------------------------------
    class id
    {
    public:
      static constexpr size_t kMaxNrFacets = 8;

      /// Default constructor shall not change the value of unique_id because unique_id was
      /// default initialized to zero during static initialization
      id()
      {
        std::cout << "locale::id::id(): unique_id = " << unique_id << std::endl;
      }

      /// no copy constructur since an id shall be unique
      id(const id&) = delete;

      /// return the id and construct a new id if no id was provided before
      operator size_t() 
      { 
        std::cout << "locale::id::operator size_t(): unique_id=" << unique_id << std::endl;
        if (unique_id == 0)
        {
          unique_id = static_cast<size_t>(++id_cnt);
          std::cout << "locale::id::operator size_t(): unique_id=" << unique_id << std::endl;
        }
        return unique_id;
      }

    protected:
      /// Unique Id [1 ... kMaxNrFacets-1], the value 0 is the default value after initialization
      size_t unique_id;

      /// The global ID counter that is incremented with each facet class
      static int id_cnt;
    };

    // pointer to a facet
    using const_facet_pointer = const facet*;
    // array type of facets
    using facet_array_type = typename array<const_facet_pointer, id::kMaxNrFacets>;

  protected:
    /// The global C++ locale. There is just one global C++ locale.
    static locale global_locale;

    /// array of facets
    facet_array_type facets;

    /// construct facets
    void construct();

    /// Use facet f from now on
    template<class Facet>
    void set_facet(Facet* f)
    {
      facets.at(Facet::id) = f;
    }

  public:
    /// Construct with default facets
    locale() noexcept { construct(); }

    /// Construct as a copy of other except for Facet f
    template<class Facet>
    locale(const locale& other, Facet* f)
    {
      *this = other;
      set_facet<Facet>(f);
    }

    // Replaces the global C++ locale with loc and returns the old global C++ locale
    static locale global(const locale& loc)
    {
      locale old_global = global_locale;
      global_locale = loc;
      return old_global;
    }

    /// Constructs a new locale which is a copy of *this except for the facet of type Facet
    /// which is copied from the locale other.
    template<class Facet>
    locale combine(const locale& other) const
    {
      locale new_locale = *this;
      set_facet<Facet>(other.get_facet(Facet::id));
      return new_locale;
    }

    /// Returns pointer to facet with index idx. Returns nullptr if no such facet exists.
    const_facet_pointer get_facet(size_t idx) const
    {
      const_facet_pointer pFacet;
      
      if (facets.check_boundary(idx))
      {
        pFacet = facets.at(idx);
      }
      else
      {
        pFacet = nullptr;
      }

      return pFacet;
    }
  };

  // ---------------------------------------------------
  /// Get the facet from a locale. If the facet does not exist, the behaviour
  /// is undefined. Therefore, the caller needs to check if facet exists before
  /// calling this function.
  /// 
  /// Example with a given locale loc:
  /// 
  /// ctype<char>& ctype_fac = util::use_facet<ctype<char> >(loc);
  // ---------------------------------------------------
  template<class Facet>
  const Facet& use_facet(const locale& loc)
  {
    const size_t id = Facet::id;
    const locale::facet* f = loc.get_facet(id);
    return static_cast<const Facet&>(*f);
  }

  // ---------------------------------------------------
  /// Returns true is the facet exists, returns false otherwise.
  // ---------------------------------------------------
  template<class Facet>
  bool has_facet(const locale& loc)
  {
    return loc.get_facet(Facet::id) != nullptr;
  }
} // namespace util

#endif // UTIL_LOCALE_H