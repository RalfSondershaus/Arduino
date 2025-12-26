/**
  * @file String_view.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines the string_view class.
  *
  * @copyright Copyright 2023 Ralf Sondershaus
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

#ifndef UTIL_STRING_VIEW_H
#define UTIL_STRING_VIEW_H

#include <Std_Types.h>
#include <Platform_Limits.h> // numeric_limits::max
#include <Util/Algorithm.h> // swap
#include <Util/Math.h>
#include <Util/Ios_Type.h> // streamoff
#include <Util/String.h>   // char_traits, size_t

namespace util
{
    /**
     * @class basic_string_view
     * @brief A lightweight, non-owning view of a contiguous sequence of characters.
     * 
     * @tparam CharT The character type (e.g., char, wchar_t)
     * @tparam Traits Character traits type (e.g., std::char_traits<CharT>)
     * 
     * @details
     * This class provides a read-only view into a character sequence without owning the underlying data.
     * It is similar to std::string_view from C++17 and offers efficient string operations without copying.
     * 
     * Key Features:
     * - Non-owning: Does not allocate or manage memory
     * - Lightweight: Contains only a pointer and size
     * - Efficient: No copying of character data
     * - Immutable: Provides only const access to the underlying data
     * 
     * @note The underlying character array must remain valid for the lifetime of the string_view.
     *       The view does not manage the lifetime of the pointed-to data.
     * 
     * @warning The character sequence is not required to be null-terminated.
     * 
     * @example
     * @code
     * // Create from string literal
     * basic_string_view<char> view1("Hello, World!");
     * 
     * // Create from pointer and length
     * const char* str = "Example";
     * basic_string_view<char> view2(str, 4); // "Exam"
     * 
     * // Substring operations
     * auto sub = view1.substr(7, 5); // "World"
     * 
     * // Search operations
     * size_t pos = view1.find("World"); // Returns 7
     * 
     * // Comparison
     * if (view1.starts_with("Hello")) {
     *     // Do something
     * }
     * 
     * // Iteration
     * for (auto ch : view1) {
     *     // Process each character
     * }
     * 
     * // Modify view boundaries
     * view1.remove_prefix(7); // Now view is "World!"
     * view1.remove_suffix(1); // Now view is "World"
     * @endcode
     */
  template<class CharT, class Traits = char_traits<CharT> >
  class basic_string_view
  {
  public:
    using traits_type     = Traits;
    using value_type      = CharT;        // acc. to standard, this is named value_type instead of char_type
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using iterator        = pointer;
    using const_iterator  = const_pointer;
    using size_type       = size_t;

    /// This is a special value equal to the maximum value representable by the type size_t
    static constexpr size_t npos = static_cast<size_t>(-1);

  protected:
    /// array of elements
    const_pointer pStr;
    /// number of characters in pStr
    size_type len;

    /// Compare p_left and p_right
    static int traits_compare(const_pointer p_left, size_type size_left, const_pointer p_right, size_type size_right)
    {
      int ret = traits_type::compare(p_left, p_right, util::min_(size_left, size_right));

      if (ret == 0U)
      {
        if (size_left < size_right) 
        {
          ret = -1;
        }
        else if (size_left > size_right) 
        {
          ret = 1;
        }
        else
        {
          // ret = 0;
        }
      }

      return ret;
    }

    /// Returns the smaller of count and size() - pos.
    /// Limits pos to [0, size()].
    /// If count == npos, count is set to size() (max length).
    size_type get_rcount(size_type pos, size_type count) const noexcept
    {
      size_type size_pos = (pos <= size()) ? (size() - pos) : (0);
      size_type rcount = (count == npos) ? size() : count;
      return util::min_(rcount, size_pos);
    }

  public:
    /// Construct
    constexpr basic_string_view() noexcept : pStr(nullptr), len(0) {}
    constexpr basic_string_view(const basic_string_view& v) noexcept = default;
    constexpr basic_string_view(const_pointer s, size_type n) { pStr = s; len = n; }
    constexpr basic_string_view(const_pointer s) { pStr = s; len = traits_type::length(s); }

    /// destruct
    ~basic_string_view() = default;

    /// Replaces the contents of the string with a copy of s
    /// or with the contents of the string with those of null-terminated character string.
    basic_string_view& operator=(const basic_string_view& s) 
    {
      pStr = s.pStr;
      len = s.len;
      return *this;
    }

    /// Iterators
    
    /// returns an iterator to the beginning (first character)
    constexpr const_iterator begin() const noexcept { return &pStr[0]; }
    constexpr const_iterator cbegin() const noexcept { return &pStr[0]; }

    /// returns an iterator to the end (one character past the last character)
    constexpr const_iterator end() const noexcept { return &pStr[len]; }
    constexpr const_iterator cend() const noexcept { return &pStr[len]; }

    /// Element access

    /// Returns a reference to the character at specified location pos. No bounds checking is performed.
    /// Bounds can be checked with check_boundary(pos) before calling this function.
    const_reference operator[](size_t pos) const { return pStr[pos]; }

    /// Returns a reference to the character at specified location pos. Bounds checking is performed.
    /// If pos is out of bounds, a reference to the first element is returned.
    const_reference at(size_t pos) const { return (check_boundary(pos)) ? pStr[pos] : pStr[0]; }

    /// Returns reference to the first character in the string
    const_reference front() const { return *begin(); }

    /// Returns reference to the last character in the string
    const_reference back() const { return at(size() - 1); }

    /// Returns a pointer to the character array.
    /// The returned array is not required to be null-terminated.
    const_pointer data() const noexcept { return begin(); }

    /// Return true if the index is valid (within boundaries)
    bool check_boundary(size_t pos) const { return pos < size(); }

    /// Capacity

    /// Checks if the view is empty
    bool empty() const { return len == 0; }

    /// Returns the number of characters
    size_t size() const { return len; }
    size_t length() const { return size(); }

    /// Returns the maximum number of elements the string is able to hold
    constexpr size_t max_size() const { return platform::numeric_limits<size_t>::max_(); }

    /// Modifiers

    /// Moves the start of the view forward by n characters.
    constexpr void remove_prefix(size_type n) { n = util::min_(size(), n); pStr += n; len -= n; }

    /// Moves the end of the view back by n characters.
    constexpr void remove_suffix(size_type n) { n = util::min_(size(), n); len -= n; }

    /// Exchanges the view with that of v.
    constexpr void swap(basic_string_view& v) noexcept 
    { 
      util::swap(pStr, v.pStr);
      util::swap(len, v.len);
    }

    // Operations

    /// Copies the substring [pos, pos + rcount) to the character array pointed to by dest, 
    /// where rcount is the smaller of count and size() - pos.
    /// @return The number of characters copied.
    size_type copy(pointer dest, size_type count, size_type pos = 0) const 
    { 
      const size_type rcount = get_rcount(pos, count);
      (void)traits_type::copy(dest, data() + pos, rcount);
      return rcount;
    }

    /// Returns a view of the substring [pos, pos + rlen), where rlen is the smaller of 
    /// count and size() - pos.
    constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const
    {
      const size_type rcount = get_rcount(pos, count);
      return basic_string_view(data() + pos, rcount);
    }

    /**
     * @brief Compares two character sequences lexicographically.
     * 
     * Performs a lexicographical comparison between this string view and the provided
     * character sequence using the character traits comparison function. The comparison
     * is performed character by character up to the length of the shorter sequence.
     * If all compared characters are equal, the result is determined by comparing the
     * lengths of the sequences.
     * 
     * @param v The string view to compare with this string view.
     * @return int Comparison result:
     *         - Negative value (typically -1) if this view is lexicographically less than v
     *         - Zero (0) if both views are equal in content and length
     *         - Positive value (typically 1) if this view is lexicographically greater than v
     * 
     * @note The comparison uses the traits_type::compare function for character comparison.
     * @note Time complexity: O(min(n, m)) where n and m are the sizes of the two views.
     * @note This function is constexpr and noexcept, making it suitable for compile-time
     *       evaluation and guaranteeing no exceptions will be thrown.
     * 
     * @par Example Usage:
     * @code
     * basic_string_view<char> sv1("apple");
     * basic_string_view<char> sv2("banana");
     * basic_string_view<char> sv3("apple");
     * basic_string_view<char> sv4("app");
     * 
     * int result1 = sv1.compare(sv2);  // Returns negative value (sv1 < sv2)
     * int result2 = sv1.compare(sv3);  // Returns 0 (sv1 == sv3)
     * int result3 = sv2.compare(sv1);  // Returns positive value (sv2 > sv1)
     * int result4 = sv1.compare(sv4);  // Returns positive value (sv1 > sv4, same prefix but sv1 is longer)
     * 
     * // Using comparison result in conditional statements
     * if (sv1.compare(sv2) < 0) {
     *     // sv1 comes before sv2 in lexicographical order
     * }
     * @endcode
     * 
     * @see compare(size_type, size_type, basic_string_view) for substring comparison
     * @see operator==, operator<, operator> for operator-based comparisons
     */
    constexpr int compare(basic_string_view v) const noexcept
    {
      int res = traits_type::compare(data(), v.data(), util::min_(size(), v.size()));

      if (res == 0)
      {
        if (size() < v.size())        { res = -1; }
        else if (size() == v.size())  { res = 0; }
        else                          { res = 1; }
      }
      return res;
    }
    constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const { return substr(pos1, count1).compare(v); }
    constexpr int compare(size_type pos1, size_type count1, basic_string_view v, size_type pos2, size_type count2) const { return substr(pos1, count1).compare(v.substr(pos2, count2)); }
    constexpr int compare(const_pointer s) const { return compare(basic_string_view(s)); }
    constexpr int compare(size_type pos1, size_type count1, const_pointer s ) const { return substr(pos1, count1).compare(basic_string_view(s)); }
    constexpr int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const { return substr(pos1, count1).compare(basic_string_view(s, count2)); }

    constexpr bool starts_with(basic_string_view sv) const noexcept { return basic_string_view(data(), util::min_(size(), sv.size())).compare(sv) == 0; }
    constexpr bool starts_with(value_type ch) const noexcept        { return !empty() && traits_type::eq(front(), ch); }
    constexpr bool starts_with(const_pointer s) const               { return starts_with(basic_string_view(s)); }

    constexpr bool ends_with(basic_string_view sv) const noexcept { return (size() >= sv.size()) && (compare(size() - sv.size(), npos, sv) == 0); }
    constexpr bool ends_with(value_type ch) const noexcept        { return !empty() && traits_type::eq(back(), ch); }
    constexpr bool ends_with(const_pointer s) const               { return ends_with(basic_string_view(s)); }

    /// Finds the first substring equal to the given character sequence.
    /// Finds the first occurence of v in this view, starting at position pos.
    constexpr size_type find(basic_string_view sv, size_type pos = 0) const noexcept
    {
      size_type ret = npos;

      if (sv.size() == 0)
      {
        ret = 0;
      }
      else if (pos < size())
      {
        const_iterator pSrc = begin() + pos; // points to the character of the first character match (if exists)
        const_iterator pPvt = pSrc;          // points to the character of a substring search (in this string view)
        const_iterator pSv  = sv.begin();     // points to the character of a substring search (in given character sequence)
        while ((pSrc < end()) && (pSv < sv.end()) && (pPvt < end()))
        {
          if (traits_type::eq(*pPvt, *pSv))
          {
            // characters are equal, check next character
            pSv++;
            pPvt++;
          }
          else
          {
            // characters are not equal, re-start search at the character where the search started
            pSv = sv.begin();
            pSrc++;
            pPvt = pSrc;
          }
        }
        if ((pSv == sv.end()) && (pSrc != pPvt))
        {
          ret = pSrc - begin();
        }
      }
      else
      {
        // nothing to do: ret is npos already
      }

      return ret;
    }

    constexpr size_type find(value_type ch, size_type pos = 0) const noexcept       { return find(basic_string_view(&ch, 1), pos); }
    constexpr size_type find(const_pointer s, size_type pos, size_type count) const { return find(basic_string_view(s, count), pos); }
    constexpr size_type find(const_pointer s, size_type pos = 0) const              { return find(basic_string_view(s), pos); }

    /// Finds the first character equal to any of the characters in the given character sequence.
    /// Finds the first occurrence of any of the characters of v in this view, starting at position pos.
    /// @return Position of the first occurrence of any character of the substring, or npos if no such character is found.
    constexpr size_type find_first_of(basic_string_view sv, size_type pos = 0) const noexcept
    {
      size_type ret = npos;
      bool found;

      if (sv.size() == 0)
      {
        ret = 0;
      }
      else if (pos < size())
      {
        const_iterator pSrc = begin() + pos; // points to the character of in this string view
        found = false;
        while ((!found) && (pSrc < end()))
        {
          // loop over characters of in given character array
          for (auto pSv = sv.begin(); pSv < sv.end(); pSv++)
          {
            if (traits_type::eq(*pSrc, *pSv))
            {
              found = true;
              ret = pSrc - begin();
              break;
            }
          }
          pSrc++;
        }
      }
      else
      {
        // nothing to do: ret is npos already
      }

      return ret;
    }

    constexpr size_type find_first_of(value_type ch, size_type pos = 0) const noexcept        
    {
      size_type ret = npos;
      if (pos < size())
      {
        const_pointer pSrc = begin() + pos;
        const_pointer pFound = traits_type::find(pSrc, size() - pos, ch);
        if (pFound != nullptr)
        {
          ret = pFound - begin();
        }
      }
      return ret;
    }
    constexpr size_type find_first_of(const_pointer s, size_type pos, size_type count) const  { return find_first_of(basic_string_view(s, count), pos); }
    constexpr size_type find_first_of(const_pointer s, size_type pos = 0) const               { return find_first_of(basic_string_view(s), pos); }

    /// Finds the first character not equal to any of the characters in the given character sequence.
    /// Finds the first character not equal to any of the characters of v in this view, starting at position pos.
    /// @return Position of the first character not equal to any of the characters in the given string, or std::string_view::npos if no such character is found.
    constexpr size_type find_first_not_of(basic_string_view sv, size_type pos = 0) const noexcept
    {
      size_type ret = npos;
      bool found; // first character found that is not equal to any of the characters in v

      if (sv.size() == 0)
      {
        ret = 0;
      }
      else if (pos < size())
      {
        const_iterator pSrc = begin() + pos; // points to the character in this string view
        found = false;
        while (pSrc < end())
        {
          // loop over characters of in given character array
          for (auto pSv = sv.begin(); pSv < sv.end(); pSv++)
          {
            if (traits_type::eq(*pSrc, *pSv))
            {
              found = true;
              break;
            }
          }
          if (found)
          {
            found = false;
            pSrc++;
          }
          else
          {
            ret = pSrc - begin();
            break;
          }
        }
      }
      else
      {
        // nothing to do: ret is npos already
      }

      return ret;
    }

    constexpr size_type find_first_not_of(value_type ch, size_type pos = 0) const noexcept        { return find_first_not_of(basic_string_view(&ch, 1), pos); }
    constexpr size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const  { return find_first_not_of(basic_string_view(s, count), pos); }
    constexpr size_type find_first_not_of(const_pointer s, size_type pos = 0) const               { return find_first_not_of(basic_string_view(s), pos); }
  };

  /// Basic_string_view typedef name for char
  using string_view = basic_string_view<char>;

} // namespace util

// ---------------------------------------------------
/// Compares two views.
/// All comparisons are done via the compare() member function.
/// Two views are equal if both the size of lhs and rhs are equal and each character in lhs has 
/// equivalent character in rhs at the same position.
// ---------------------------------------------------
template<class CharT, class Traits = util::char_traits<CharT> >
constexpr bool operator==(util::basic_string_view<CharT,Traits> lhs, util::basic_string_view<CharT,Traits> rhs) noexcept
{ 
  return lhs.compare(rhs) == 0; 
}

#if 0
// Currently, no support for this operator.
// GCC shows a warning: literal operator suffixes not preceded by '_' are reserved for future standardization [-Wliteral-suffix]
// See also https://github.com/tcbrindle/cpp17_headers/issues/10
constexpr util::basic_string_view<char> operator ""sv(const char* str, util::size_t len) noexcept
{
  return util::basic_string_view<char>(str, len);
}
#endif

#endif // UTIL_STRING_VIEW_H