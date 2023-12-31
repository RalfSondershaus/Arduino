/**
  * @file String.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines a simple string class
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

#ifndef UTIL_STRING_H
#define UTIL_STRING_H

#include <Std_Types.h>
#include <string.h> // strchr, strstr, memmove
#include <stdlib.h> // strtol
#include <stdio.h>  // EOF
#include <errno.h>  // errno
#include <Util/Algorithm.h>

namespace util
{
  using size_t = ::size_t;

  // ---------------------------------------------------
  /// The char_traits class is a traits class template that abstracts basic character and string operations 
  /// for a given character type. The defined operation set is such that generic algorithms almost always can 
  /// be implemented in terms of it. It is thus possible to use such algorithms with almost any possible 
  /// character or string type, just by supplying a customized char_traits class.
  // ---------------------------------------------------
  template<class CharT>
  class char_traits
  {};

  // ---------------------------------------------------
  /// Standard specialization for char.
  // ---------------------------------------------------
  template<>
  class char_traits<char>
  {
  public:
    typedef char char_type;
    typedef int int_type;

    static void assign(char_type& c1, const char_type& c2) noexcept { c1 = c2; }
    static constexpr bool eq(char_type a, char_type b) noexcept { return static_cast<unsigned char>(a) == static_cast<unsigned char>(b); }
    static constexpr bool lt(char_type a, char_type b) noexcept { return static_cast<unsigned char>(a) < static_cast<unsigned char>(b); }

    /// Copies count characters from the character string pointed to by src to the character string pointed to by dest.
    /// Performs correctly even if the ranges[src, src + count) and [dest, dest + count) overlap
    static char_type* move(char_type* dest, const char_type* src, util::size_t count) { memmove(dest, src, count); }

    /// copies a character sequence
    static char_type* copy(char_type* dest, const char_type* src, util::size_t count) { memcpy(dest, src, count); }

    /// Compares the first count characters of the character strings s1 and s2. The comparison is done lexicographically.
    /// If count is zero, strings are considered equal.
    static int compare(const char_type* s1, const char_type* s2, util::size_t count) { return strncmp(s1, s2, count); }

    /// returns the length of a character sequence
    static util::size_t length(const char_type* s) { return strlen(s); }

    /// finds a character in a character sequence
    static const char_type* find(const char_type* ptr, util::size_t count, const char_type& ch) { reinterpret_cast<const char_type*>(memchr(ptr, ch, count)); }

    /// converts int_type to equivalent char_type.
    /// Converts c to char_type. If there is no equivalent char_type value (such as when c is a 
    /// copy of the eof() value), the result is unspecified. We implement: to_char_type(eof()) -> 0xFF.
    static constexpr char_type to_char_type(int_type c) noexcept { return static_cast<char_type>(c); }

    /// Converts c to int_type.
    static constexpr int_type to_int_type(char_type c) noexcept { return static_cast<int_type>(c); }

    /// Checks whether two values of type int_type are equal.
    static constexpr bool eq_int_type(int_type c1, int_type c2) noexcept { return c1 == c2; }

    /// Returns a value not equivalent to any valid value of type char_type.
    static constexpr int_type eof() noexcept { return EOF; }

    /// Given e, produce a suitable value that is not equivalent to eof.
    static constexpr int_type not_eof(int_type e) noexcept { return (e != eof()) ? (e) : (!eof()); }
  };

#if 0
  template<class Traits>
  util::size_t string_find(const Traits::char_type* str1, util::size_t count1, util::size_t pos, const Traits::char_type* str2, util::size_t count2);

  template<>
  util::size_t string_find<char_traits<char> >(const char_traits<char>::char_type* str1, util::size_t count1, util::size_t pos, const char_traits<char>::char_type* str2, util::size_t count2)
  {

  }
#endif

  // ---------------------------------------------------
  /// A simple string class with a fixed size.
  /// 
  /// The find() functions work with standard C library functions such as strstr.
  /// Standard C library functions are based on char type. So the template
  /// parameter CharT is currently limited to char or unsigned char.
  // ---------------------------------------------------
  template<int Size, class CharT, class Traits = util::char_traits<CharT> >
  class basic_string
  {
  public:
    using char_type = CharT;
    using traits_type = Traits;
    using pointer = char_type*;
    using const_pointer = const char_type*;
    typedef char_type& reference;
    typedef const char_type& const_reference;
    typedef char_type* iterator;
    typedef const_pointer const_iterator;

    /// The string class
    //typedef basic_string<Size, CharT> this_type;

    /// The maximal size of the string
    static constexpr util::size_t MaxLength = Size;

    /// This is a special value equal to the maximum value representable by the type util::size_t
    static constexpr util::size_t npos = static_cast<util::size_t>(-1);

  protected:
    /// array of elements
    char_type elements[Size + 1U];

    /// iterator one past the last element. If no elements, points to begin.
    iterator it_end;

    /// Returns the number of remaining bytes until string is full
    util::size_t remaining_size() const { return max_size() - size(); }

  public:
    /// Construct
    basic_string() { clear(); }
    basic_string(const_reference v) { assign(1, v); }
    basic_string(const_pointer s) { assign(s); }

    /// Copy construct
    template<int Size2>
    basic_string(const basic_string<Size2, CharT>& s) { assign(s); }

    /// destruct
    ~basic_string() = default;

    /// Replaces the contents of the string with a copy of s
    /// or with the contents of the string with those of null-terminated character string
    template<int Size2>
    basic_string& operator=(const basic_string<Size2, CharT>& s) { return assign(s); }
    basic_string& operator=(const_pointer s) { return assign(s); }

    /// Replaces the contents with count copies of character ch.
    /// If count is greater than max_size(), the contents 
    /// is replaced with max_size() copies of character ch.
    basic_string& assign(util::size_t count, char_type ch)
    {
      // limit count to the available space
      count = util::min(count, max_size());
      it_end = util::fill_n(begin(), count, ch);
      return *this;
    }

    /// Replaces the contents with a copy of str. Equivalent to *this = str;
    template<int Size2>
    basic_string& assign(const basic_string<Size2, CharT>& s)
    {
      util::size_t count = util::min(s.size(), max_size());
      it_end = util::copy_n(s.begin(), count, begin());
      return *this;
    }

    /// Replaces the contents with copies of the characters in the range [s, s + count). 
    /// This range can contain null characters.
    basic_string& assign(const_pointer s, util::size_t count)
    {
      if (s != nullptr)
      {
        // limit count to the available space
        count = util::min(count, max_size());
        auto it = begin();
        while (count > 0U)
        {
          *it++ = *s++;
          count--;
        }
        it_end = it;
      }
      return *this;
    }

    /// Replaces the contents with those of null-terminated character string pointed to by s.
    basic_string& assign(const_pointer s)
    {
      if (s != nullptr)
      {
        auto it = begin();
        util::size_t count = max_size();
        while ((*s != static_cast<char_type>(0)) && (count > 0U))
        {
          *it++ = *s++;
          count--;
        }
        it_end = it;
      }
      return *this;
    }

    /// Element access

    /// Returns reference to the first character in the string
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }

    /// Returns reference to the last character in the string
    reference back() { return (size() > 0U) ? *(it_end - 1) : (*it_end); }
    const_reference back() const { return (size() > 0U) ? *(it_end - 1) : (*it_end); }

    /// Returns a reference to the character at specified location pos. Bounds checking is performed.
    /// If pos is out of bounds, a reference to the first element is returned.
    reference at(util::size_t pos) { return (check_boundary(pos)) ? elements[pos] : elements[0]; }
    const_reference at(util::size_t pos) const { return (check_boundary(pos)) ? elements[pos] : elements[0]; }

    /// Returns a reference to the character at specified location pos. No bounds checking is performed.
    /// Bounds can be checked with check_boundary(pos) before calling this function.
    reference operator[](util::size_t pos) { return elements[pos]; }
    const_reference operator[](util::size_t pos) const { return elements[pos]; }

    /// Returns a pointer to the underlying array serving as character storage.
    /// The returned array is not required to be null-terminated.
    const_pointer data() const noexcept { return begin(); }

    /// Returns a pointer to a null-terminated character array with data equivalent to those stored in the string.
    const_pointer c_str() const noexcept
    { 
      *it_end = static_cast<char_type>(0); 
      return begin(); 
    }

    /// Return true if the index is valid (within boundaries)
    bool check_boundary(util::size_t pos) const { return (pos >= static_cast<util::size_t>(0)) && (pos < size()); }

    /// Iterators
    
    /// returns an iterator to the beginning
    iterator begin() { return &elements[0]; }
    const_iterator begin() const { return &elements[0]; }

    /// returns an iterator to the end
    iterator end() { return it_end; }
    const_iterator end() const { return it_end; }

    /// Capacity

    /// Checks if the string has no characters
    bool empty() const { return begin() == end(); }

    /// Returns the number of characters
    util::size_t size() const { return end() - begin(); }
    util::size_t length() const { return size(); }

    /// Returns the maximum number of elements the string is able to hold
    constexpr util::size_t max_size() const { return Size; }

    /// Returns the number of characters that can be held in currently allocated storage
    constexpr util::size_t capacity() { return max_size(); }

    /// Modifiers

    /// clears the contents
    void clear() { it_end = &elements[0]; }

    /// Appends count copies of character ch.
    basic_string& append(util::size_t count, CharT ch)
    {
      // limit count to the available space
      count = util::min(count, remaining_size());
      it_end = util::fill_n(end(), count, ch);
      return *this;
    }

    /// Appends string str.
    template<int Size2>
    basic_string& append(const basic_string<Size2, CharT>& str)
    {
      util::size_t count = util::min(str.size(), remaining_size());
      it_end = util::copy_n(str.begin(), count, end());
      return *this;
    }

    /// Appends a substring[pos, pos + count) of str
    template<int Size2>
    basic_string& append(const basic_string<Size2, CharT>& str, util::size_t pos, util::size_t count)
    {
      if (pos < str.size())
      {
        // do not copy elements past the last element of str
        // and do not copy elements past the last element of this
        count = util::min(count, str.size() - pos);
        count = util::min(count, remaining_size());
        it_end = util::copy_n(str.begin() + pos, count, end());
      }
      return *this;
    }

    /// Appends characters in the range [s, s + count). This range can contain null characters.
    basic_string& append(const_pointer s, util::size_t count)
    {
      if (s != nullptr)
      {
        // limit count to the available space
        count = util::min(count, remaining_size());
        auto it = end();
        while (count > 0U)
        {
          *it++ = *s++;
          count--;
        }
        it_end = it;
      }
      return *this;
    }

    /// Appends the null-terminated character string pointed to by s, as if by append(s, Traits::length(s))
    basic_string& append(const_pointer s)
    {
      if (s != nullptr)
      {
        auto it = end();
        util::size_t count = remaining_size();
        while ((*s != static_cast<char_type>(0)) && (count > 0U))
        {
          *it++ = *s++;
          count--;
        }
        it_end = it;
      }
      return *this;
    }

    /// Appends characters in the range[first, last)
    template< class InputIt >
    basic_string& append(InputIt first, InputIt last)
    {
      while ((first != last) && (size() < max_size()))
      {
        *it_end++ = *first++;
      }
      return *this;
    }

    /// Appends string str, character ch, or a null-terminated character string
    template<int Size2>
    basic_string& operator+=(const basic_string<Size2, CharT>& str) { return append(str); }
    basic_string& operator+=(char_type ch) { return append(1, ch); }
    basic_string& operator+=(const_pointer s) { return append(s); }

    /// Search

    /// Finds the first substring equal to str.
    /// Returns position of the first character of the found substring or npos if no such substring is found
    template<int Size2>
    util::size_t find(const basic_string<Size2, CharT>& str, util::size_t pos = 0) const noexcept
    {
      util::size_t ret = npos;

      if (pos < size())
      {
        const_pointer pSrc = c_str() + pos;
        const_pointer pStr = str.c_str();
        const_pointer pPos = strstr(pSrc, pStr); // note: strstr works with char*
        // const_pointer pPos = string_find<traits_type>(pSrc, size() - pos, pos, pStr, traits_type::length(pStr));
        if (pPos != nullptr)
        {
          ret = (pPos - pSrc) + pos;
        }
      }

      return ret;
    }

    /// Finds the first substring equal to the character string pointed to by s.
    /// The length of the string is determined by the first null character.
    /// The search start at position pos.
    util::size_t find(const_pointer s, util::size_t pos = 0) const
    {
      util::size_t ret = npos;

      if (pos < size())
      {
        const char* pSrc = static_cast<const char*>(c_str()) + pos;
        const char* pStr = static_cast<const char*>(s);
        const char* pPos = strstr(pSrc, pStr);
        if (pPos != nullptr)
        {
          ret = (pPos - pSrc) + pos;
        }
      }

      return ret;
    }

    /// Finds the first character ch
    /// The search start at position pos.
    util::size_t find(CharT ch, util::size_t pos = 0) const noexcept
    {
      util::size_t ret = npos;

      if (pos < size())
      {
        const char* pSrc = static_cast<const char*>(c_str()) + pos;
        const char* pPos = strchr(pSrc, static_cast<int>(ch));
        if (pPos != nullptr)
        {
          ret = (pPos - pSrc) + pos;
        }
      }

      return ret;
    }

    /// Operations
    
    /// Compares two strings
    template<int Size2>
    int compare(const basic_string<Size2, char_type>& str) const noexcept { return strcmp(c_str(), str.c_str()); }

    /// Compares this string to the null-terminated character sequence beginning at the character pointed to by s
    int compare(const_pointer s) const noexcept { return strcmp(c_str(), s); }
  };

#if 0
  // ---------------------------------------------------
  /// A simple string stream class with a fixed size.
  // ---------------------------------------------------
  template<int Size, class CharT, class Traits = util::char_traits<CharT> >
  class basic_istringstream
  {
  public:
    /// the character type
    typedef CharT char_type;

    /// this class type
    typedef basic_istringstream<char_type, Size> this_type;

    /// Helper.
    /// Normally defined in header <ios>.
    /// The type std::streamsize is an implementation-defined signed integral type used 
    /// to represent the number of characters transferred in an I/O operation or the size 
    /// of an I/O buffer. It is used as a signed counterpart of std::size_t, similar to 
    /// the POSIX type ssize_t.
    typedef int streamsize;

    /// the string type
    using string_type = basic_string<char_type, Size>;

    /// The maximal size of the string
    static constexpr util::size_t MaxLength = Size;

  protected:
    /// a string holds the data
    string_type my_str;

  public:
    /// construct
    basic_istringstream() {}
    explicit basic_istringstream(const string_type& str) { my_str = str; }
    explicit basic_istringstream(const_pointer s) { my_str = s; }

    /// destruct
    ~basic_istringstream() = default;

    // In C++ standard, the following member functions are inherited from std::basic_istream

    // Unformatted input

    /// extracts characters
    int_type get();
    basic_istringstream& get(char_type& ch);
    basic_istringstream& get(char_type* s, streamsize count);
    basic_istringstream& get(char_type* s, streamsize count, char_type delim);

      reads the next character without extracting it
      peek

      unextracts a character
      unget

      puts a character into input stream
      putback

      extracts characters until the given character is found
      getline

      extracts and discards characters until the given character is found
      ignore

      extracts blocks of characters
      read

      extracts already available blocks of characters
      readsome

      returns number of characters extracted by last unformatted input operation
      gcount

   //Positioning

      returns the input position indicator
      tellg

      sets the input position indicator
      seekg

  };
#endif

  // ---------------------------------------------------
  /// Interprets a signed integer value in the string str
  /// @param pos    address of an integer to store the number of characters processed
  // ---------------------------------------------------
  template<int Size>
  int stoi(const util::basic_string<Size, char>& str, size_t* pos = nullptr, int base = 10, int * err = nullptr)
  {
    char* endpos;
    const char* startpos = str.c_str();
    int ret = static_cast<int>(strtol(startpos, &endpos, base));
    if (pos != nullptr)
    {
      *pos = endpos - startpos;
    }
    if (err)
    {
      *err = errno;
    }
    return ret;
  }

  /// Interprets a signed integer value in the string str
  /// @param pos    address of an integer to store the number of characters processed
  template<int Size>
  long stol(const util::basic_string<Size, char>& str, size_t* pos = nullptr, int base = 10)
  {
    char* endpos;
    const char* startpos = str.c_str();
    long ret = strtol(startpos, &endpos, base);
    if (pos != nullptr)
    {
      *pos = endpos - startpos;
    }
    return ret;
  }

  /// Interprets an unsigned integer value in the string str
  /// @param pos    address of an integer to store the number of characters processed
  template<int Size>
  unsigned long stoul(const util::basic_string<Size, char>& str, size_t* pos = nullptr, int base = 10)
  {
    char* endpos;
    const char* startpos = str.c_str();
    unsigned long ret = strtoul(startpos, &endpos, base);
    if (pos != nullptr)
    {
      *pos = endpos - startpos;
    }
    return ret;
  }
} // namespace util

// ---------------------------------------------------
/// Returns a string containing characters from lhs followed by the characters from rhs.
// ---------------------------------------------------
template<int Size, class CharT>
util::basic_string<Size, CharT> operator+(const util::basic_string<Size, CharT>& lhs, const util::basic_string<Size, CharT>& rhs)
{
  std::basic_string<Size, CharT> str = lhs;
  str += rhs;
  return str;
}

// ---------------------------------------------------
/// Compares the contents of a string with another string.
/// All comparisons are done via the compare() member function.
/// Two strings are equal if both the size of lhs and rhs are equal and each character in lhs has 
/// equivalent character in rhs at the same position.
// ---------------------------------------------------
template< int Size1, int Size2, class CharT >
bool operator==(const util::basic_string<Size1, CharT>& lhs, const util::basic_string<Size2, CharT>& rhs) noexcept 
{ 
  return lhs.compare(rhs) == 0; 
}

template< int Size, class CharT >
bool operator==(const util::basic_string<Size, CharT>& lhs, const CharT* rhs)
{
  return lhs.compare(rhs) == 0;
}

#endif // UTIL_STRING_H