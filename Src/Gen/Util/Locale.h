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
#include <Util/String.h>    // util::char_traits
#include <Util/Array.h>

namespace util
{

    /**
     * @brief Represents a customizable C++ locale system with support for user-defined facets.
     *
     * The locale class provides a mechanism to manage and access formatting and classification
     * components (facets) such as character types, numeric formatting, and collation rules.
     * It mimics the behavior of the standard C++ locale system but allows for controlled
     * registration and retrieval of facets using a static ID-based dispatch mechanism.
     *
     * Each facet is associated with a unique ID, managed by the nested @ref locale::id class.
     * Facets are stored in a fixed-size array and accessed via their ID. The locale supports
     * combining and overriding facets from other locale instances.
     *
     * Each locale holds at least the following standard facets (i.e. util::has_facet returns true
     * for all these facet types), but a program may define additional specializations or 
     * completely new facets and add them to any existing locale object.
     * - ctype
     * - num_get
     * - numpunct
     * 
     * @note The C++ standard specifies further facets that are not currently supported by this 
     * implementation's default configuration. 
     * 
     * Internally, a locale object is implemented as if it is a reference-counted pointer to an 
     * array (indexed by std::locale::id) of reference-counted pointers to facets.
     * 
     * [https://en.cppreference.com/w/cpp/locale/locale.html]
     * 
     * @note This implementation assumes a maximum number of facets defined by @ref locale::id::kMaxNrFacets.
     *       Facets must be manually registered using @ref set_facet before they can be retrieved.
     *
     * @see locale::facet
     * @see locale::id
     * @see locale::get_facet
     * @see locale::combine
     * @see util::use_facet
     * @see util::has_facet
     */
  class locale
  {
  public:
    /**
     * @brief Abstract base class for all locale facets.
     *
     * Facets represent modular components of a locale, such as character classification,
     * numeric formatting, or collation rules. Each facet is typically derived from this
     * base class and registered within a locale instance using a unique ID.
     *
     * This class serves as a polymorphic anchor for all facet types and allows
     * safe storage and retrieval via base-class pointers.
     *
     * @note Facets are managed through pointers and stored in a fixed-size array
     *       inside the locale. They are accessed via their associated @ref locale::id.
     *
     * @see locale
     * @see locale::id
     */
    class facet
    {
    public:
    };

    /**
     * @brief Unique identifier class for locale facets.
     *
     * Each facet type is associated with a static instance of this class, which
     * provides a unique numeric ID used for indexing into the locale's facet array.
     * The ID is lazily assigned during runtime via static initialization and dynamic
     * sequencing, as defined by the C++ standard.
     *
     * The default value of `unique_id` is zero, which indicates that no ID has been
     * assigned yet. Upon first use, a new ID is generated and stored.
     *
     * @note The ID assignment relies on static initialization rules. See the C++ standard
     *       for details on initialization sequencing.
     *
     * @warning Copying is disabled to ensure uniqueness of each facet ID.
     *
     * @see locale::facet
     * @see locale::set_facet
     * @see locale::get_facet
     * @see https://en.cppreference.com/w/cpp/language/initialization
     */
    class id
    {
    public:
      static constexpr size_t kMaxNrFacets = 8;

      /// Default constructor shall not change the value of unique_id because unique_id was
      /// default initialized to zero during static initialization
      id()
      {
      }

      /// no copy constructur since an id shall be unique
      id(const id&) = delete;

      /// returns the (unique) id and generates a new id if no id was provided before
      operator size_t() 
      { 
        if (unique_id == 0)
        {
          unique_id = static_cast<size_t>(++id_cnt);
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
    using facet_array_type = array<const_facet_pointer, id::kMaxNrFacets>;
    // pointer to the table with facets
    using facet_array_const_pointer = const facet_array_type*;

  protected:
    /// The global C++ locale. There is just one global C++ locale.
    static locale global_locale;

    /// array of facets
    //facet_array_type facets;
    facet_array_const_pointer facets_pointer;

    /// construct facets
    void construct();

    /// Use facet f from now on
    template<class Facet>
    void set_facet(Facet* f)
    {
      facets_pointer->at(Facet::id) = f;
    }

  public:

    /// Construct with default facets
    locale() noexcept;

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
      
      if (facets_pointer->check_boundary(idx))
      {
        pFacet = facets_pointer->at(idx);
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