/**
  * @file Iterator.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines iterator classes
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

#ifndef UTIL_ITERATOR_H
#define UTIL_ITERATOR_H

// This file shall include Ios_Fwd.h only in order to avoid circular includes (for Locale.h, for example)

#include <Std_Types.h>
#include <Util/Ios_Fwd.h>  // forward declarations

namespace util
{
  // ---------------------------------------------------
  /// A single-pass input iterator that reads successive characters 
  /// from the std::basic_streambuf object for which it was constructed.
  /// Default Traits type is defined in Ios_Fwd.h
  // ---------------------------------------------------
  template< class CharT, class Traits>
  class istreambuf_iterator
  {
  public:
    using value_typ = CharT;
    using difference_type = typename Traits::off_type;
    using reference = value_typ;
    using char_type = value_typ;
    using traits_type = Traits;
    using int_type = typename traits_type::int_type;
    using streambuf_type = basic_streambuf<char_type, traits_type>;
    using istream_type = basic_istream<char_type, traits_type>;

    /// Construct an end-of-stream iterator
    constexpr istreambuf_iterator() noexcept : pBuf(nullptr) {}
    /// Construct an iterator for the stream buffer of the input stream.
    /// If the input stream doesn't have a stream buffer, construct an
    /// end-of-stream iterator.
    istreambuf_iterator(istream_type& is) noexcept : pBuf(is.rdbuf()) {}
    /// Construct an iterator for the given stream buffer
    istreambuf_iterator(streambuf_type* s) noexcept : pBuf(s) {}
    /// Copy constructor
    istreambuf_iterator(const istreambuf_iterator&) noexcept = default;

    /// Obtains a copy of the current character.
    /// The behaviour is undefined if the iterator is an end-of-file iterator.
    int_type operator*() const { return pBuf->sgetc(); }

    /// Advances the iterator by calling pBuf->sbumpc() where pBuf is the 
    /// stored pointer to the stream buffer.
    /// If sbumpc() returns eof(), the iterator becomes an end-of-stream iterator.
    istreambuf_iterator& operator++()
    {
      // Operator || is short-circuiting: if the first operand is true, the second operand is not evaluated.
      if ((pBuf == nullptr) || (traits_type::eq_int_type(traits_type::eof(), pBuf->sbumpc())))
      {
        pBuf = nullptr;
      }
      return *this;
    }

    // Returns true if both *this and it are valid, or both are invalid, false otherwise.
    bool equal(const istreambuf_iterator& it) const
    {
      return ((pBuf != nullptr) && (it.pBuf != nullptr)) || ((pBuf == nullptr) && (it.pBuf == nullptr));
    }

  protected:
    streambuf_type* pBuf;
  };

  /// Returns lhs.equal(rhs)
  template<class CharT, class Traits>
  bool operator==(const istreambuf_iterator<CharT, Traits>& lhs, const istreambuf_iterator<CharT, Traits>& rhs)
  {
    return lhs.equal(rhs);
  }

  /// Returns !lhs.equal(rhs)
  template< class CharT, class Traits >
  bool operator!=(const istreambuf_iterator<CharT, Traits>& lhs, const istreambuf_iterator<CharT, Traits>& rhs)
  {
    return !lhs.equal(rhs);
  }

  // ---------------------------------------------------
  /// A single-pass input iterator that reads successive objects of type T from the 
  /// std::basic_istream object for which it was constructed, by calling the 
  /// appropriate operator>>
  /// Default Traits type is defined in Ios_Fwd.h
  // ---------------------------------------------------
  template <class T, class CharT, class Traits, class Distance>
  class istream_iterator
  {
  public:
    using value_type = T;
    using difference_type = Distance;
    using pointer = const T*;
    using reference = const T&;
    using char_type = CharT;
    using traits_type = Traits;
    using istream_type = util::basic_istream<char_type, traits_type>;

    constexpr istream_iterator() : pistr(nullptr), val() {}
    istream_iterator(istream_type& str) : pistr(&str), val() { getval(); }

    reference operator*() const noexcept { return val; }
    pointer operator->() const noexcept { return &val; }
    istream_iterator& operator++()
    {
      getval();
      return *this;
    }

    istream_iterator operator++(int)
    {
      istream_iterator tmp = *this;
      getval();
      return tmp;
    }

  protected:
    void getval()
    {
      if (pistr != nullptr)
      {
        if (!(*pistr >> val))
        {
          pistr = nullptr;
        }
      }
    }

    istream_type* pistr;
    value_type val;
  };

} // namespace util

#endif // UTIL_ITERATOR_H