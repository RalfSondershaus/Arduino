/**
  * @file String.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines string classes such as char_traits and basic_string.
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
#include <Platform_Limits.h>
#include <Util/Algorithm.h>
#include <Util/Math.h>
#include <Util/Ios_Type.h> // streamoff
#include <Util/ctype.h> // isspace

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
    using char_type = char;
    using int_type = int;
    using off_type = streamoff;
    using pos_type = streampos;
    //using state_type = util::mbstate_t;

    static void assign(char_type& c1, const char_type& c2) noexcept { c1 = c2; }
    static constexpr bool eq(char_type a, char_type b) noexcept { return static_cast<unsigned char>(a) == static_cast<unsigned char>(b); }
    static constexpr bool lt(char_type a, char_type b) noexcept { return static_cast<unsigned char>(a) < static_cast<unsigned char>(b); }

    /// Copies count characters from src to dest.
    /// Performs correctly even if the ranges[src, src + count) and [dest, dest + count) overlap
    static char_type* move(char_type* dest, const char_type* src, size_t count) { return static_cast<char_type*>(::memmove(dest, src, count * sizeof(char_type))); }

    /// copies a character sequence
    static char_type* copy(char_type* dest, const char_type* src, size_t count) { return static_cast<char_type*>(::memcpy(dest, src, count * sizeof(char_type))); }

    /// Compares the first count characters of the character strings s1 and s2. The comparison is done lexicographically.
    /// If count is zero, strings are considered equal.
    static int compare(const char_type* s1, const char_type* s2, size_t count) { return ::strncmp(s1, s2, count); }

    /// returns the length of a character sequence
    static size_t length(const char_type* s) { return strlen(s); }

    /// finds a character in a character sequence
    static const char_type* find(const char_type* ptr, size_t count, const char_type& ch) { return static_cast<const char_type*>(memchr(ptr, ch, count)); }

    /// Converts int_type to equivalent char_type.
    static constexpr char_type to_char_type(int_type c) noexcept { return static_cast<char_type>(c); }

    /// Converts char_type to int_type.
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
  size_t string_find(const Traits::value_type* str1, size_t count1, size_t pos, const Traits::value_type* str2, size_t count2);

  template<>
  size_t string_find<char_traits<char> >(const char_traits<char>::value_type* str1, size_t count1, size_t pos, const char_traits<char>::value_type* str2, size_t count2)
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
  template<int Size, class CharT, class Traits = char_traits<CharT> >
  class basic_string
  {
  public:
    using traits_type = Traits;
    using value_type = CharT;                 // acc. to standard, this is named value_type instead of char_type
    using size_type = size_t;                 // should be std::allocator_traits<Allocator>::size_type 
    using pointer = value_type*;              // should be std::allocator_traits<Allocator>::pointer
    using const_pointer = const value_type*;  // should be std::allocator_traits<Allocator>::const_pointer
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type* iterator;
    typedef const_pointer const_iterator;

    /// The maximal size of the string
    static constexpr size_t MaxLength = Size;

    /// This is a special value equal to the maximum value representable by the type size_t
    static constexpr size_t npos = static_cast<size_t>(-1);

  protected:
    /// array of elements
    value_type elements[Size + 1U];

    /// iterator one past the last element. If no elements, points to begin.
    iterator it_end;

    /// Returns the number of remaining bytes until string is full
    size_t remaining_size() const { return max_size() - size(); }

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

  public:
    /// Construct
    basic_string() { clear(); }
    basic_string(const_reference v) { assign(1, v); }
    basic_string(const_pointer s) { assign(s); }
    template<int Size2>
    basic_string(const basic_string<Size2, value_type>& s) { assign(s); }

    /// Copy construct
    basic_string(const basic_string& s) { assign(s); }

    /// destruct
    ~basic_string() = default;

    /// Replaces the contents of the string with a copy of s
    /// or with the contents of the string with those of null-terminated character string.
    template<int Size2>
    basic_string& operator=(const basic_string<Size2, value_type>& s) { return assign(s); }
    basic_string& operator=(const basic_string& s) { return assign(s); }
    basic_string& operator=(const_pointer s) { return assign(s); }

    /// Replaces the contents with count copies of character ch.
    /// If count is greater than max_size(), the contents 
    /// is replaced with max_size() copies of character ch.
    basic_string& assign(size_t count, value_type ch)
    {
      // limit count to the available space
      count = ::util::min_(count, max_size());
      it_end = ::util::fill_n(begin(), count, ch);
      return *this;
    }

    /// Replaces the contents with a copy of s. Equivalent to *this = str;
    basic_string& assign(const basic_string& s)
    {
      it_end = ::util::copy_n(s.begin(), s.size(), begin());
      return *this;
    }

    /// Replaces the contents with a copy of s. Equivalent to *this = s;
    template<int Size2>
    basic_string& assign(const basic_string<Size2, value_type>& s)
    {
      size_t count = ::util::min_(s.size(), max_size());
      it_end = ::util::copy_n(s.begin(), count, begin());
      return *this;
    }

    /// Replaces the contents with copies of the characters in the range [s, s + count). 
    /// This range can contain null characters.
    basic_string& assign(const_pointer s, size_t count)
    {
      if (s != nullptr)
      {
        // limit count to the available space
        count = util::min_(count, max_size());
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
        size_t count = max_size();
        while ((*s != static_cast<value_type>(0)) && (count > 0U))
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
    reference at(size_t pos) { return (check_boundary(pos)) ? elements[pos] : elements[0]; }
    const_reference at(size_t pos) const { return (check_boundary(pos)) ? elements[pos] : elements[0]; }

    /// Returns a reference to the character at specified location pos. No bounds checking is performed.
    /// Bounds can be checked with check_boundary(pos) before calling this function.
    reference operator[](size_t pos) { return elements[pos]; }
    const_reference operator[](size_t pos) const { return elements[pos]; }

    /// Returns a pointer to the underlying array serving as character storage.
    /// The returned array is not required to be null-terminated.
    const_pointer data() const noexcept { return begin(); }

    /// Returns a pointer to a null-terminated character array with data equivalent to those stored in the string.
    const_pointer c_str() const noexcept
    { 
      *it_end = static_cast<value_type>(0); 
      return begin(); 
    }

    /// Return true if the index is valid (within boundaries)
    bool check_boundary(size_t pos) const { return (pos >= static_cast<size_t>(0)) && (pos < size()); }

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
    size_t size() const { return end() - begin(); }
    size_t length() const { return size(); }

    /// Returns the maximum number of elements the string is able to hold
    constexpr size_t max_size() const { return Size; }

    /// Returns the number of characters that can be held in currently allocated storage
    constexpr size_t capacity() { return max_size(); }

    /// Modifiers

    /// clears the contents
    void clear() { it_end = &elements[0]; }

    /// Appends count copies of character ch.
    basic_string& append(size_t count, CharT ch)
    {
      // limit count to the available space
      count = util::min_(count, remaining_size());
      it_end = util::fill_n(end(), count, ch);
      return *this;
    }

    /// Appends string str.
    template<int Size2>
    basic_string& append(const basic_string<Size2, CharT>& str)
    {
      size_t count = util::min_(str.size(), remaining_size());
      it_end = util::copy_n(str.begin(), count, end());
      return *this;
    }

    /// Appends a substring[pos, pos + count) of str
    template<int Size2>
    basic_string& append(const basic_string<Size2, CharT>& str, size_t pos, size_t count)
    {
      if (pos < str.size())
      {
        // do not copy elements past the last element of str
        // and do not copy elements past the last element of this
        count = util::min_(count, str.size() - pos);
        count = util::min_(count, remaining_size());
        it_end = util::copy_n(str.begin() + pos, count, end());
      }
      return *this;
    }

    /// Appends characters in the range [s, s + count). This range can contain null characters.
    basic_string& append(const_pointer s, size_t count)
    {
      if (s != nullptr)
      {
        // limit count to the available space
        count = util::min_(count, remaining_size());
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
        size_t count = remaining_size();
        while ((*s != static_cast<value_type>(0)) && (count > 0U))
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

    /// Non-standard functions, TBD shall be replaced by ostringstream
    basic_string& append(int val)
    {
      char s[10];
      const int base = 10;
      return append(::itoa(val, s, base));
    }
    basic_string& append(uint8 val)
    {
      return append(static_cast<int>(val));
    }
    basic_string& append(uint16 val)
    {
      return append(static_cast<int>(val));
    }
    basic_string& append(uint32 val)
    {
      return append(static_cast<int>(val));
    }

    /// Appends string str, character ch, or a null-terminated character string
    template<int Size2>
    basic_string& operator+=(const basic_string<Size2, CharT>& str) { return append(str); }
    basic_string& operator+=(value_type ch) { return append(1, ch); }
    basic_string& operator+=(const_pointer s) { return append(s); }

    /// Search

    /// Finds the first substring equal to str.
    /// Returns position of the first character of the found substring or npos if no such substring is found
    template<int Size2>
    size_t find(const basic_string<Size2, CharT>& str, size_t pos = 0) const noexcept
    {
      return find(str.c_str(), pos);
    }

    /// Finds the first substring equal to the character string pointed to by s.
    /// The length of the string is determined by the first null character.
    /// The search start at position pos.
    size_t find(const_pointer s, size_t pos = 0) const noexcept
    {
      size_t ret = npos;

      if (traits_type::length(s) == static_cast<size_t>(0))
      {
        ret = 0U;
      }

      if (pos < size())
      {
        const_pointer pSrc = c_str() + pos; // points to the character of the first character match (if exists)
        const_pointer pPvt = pSrc;         // points to the character of a substring search
        const_pointer pStr = s;            // points to the character of a substring search
        while ((*pSrc != static_cast<value_type>(0)) && (*pStr != static_cast<value_type>(0)) && (*pPvt != static_cast<value_type>(0)))
        {
          if (traits_type::eq(*pPvt, *pStr))
          {
            // characters are equal, check next character
            pStr++;
            pPvt++;
          }
          else
          {
            // characters are not equal, re-start search beginning at character where search started
            pStr = s;
            pSrc++;
            pPvt = pSrc;
          }
        }
        if ((*pStr == static_cast<value_type>(0)) && (pSrc != pPvt))
        {
          ret = pSrc - c_str();
        }
      }


      return ret;
    }

    /// Finds the first occurance of character ch. The search starts at position pos.
    size_t find(value_type ch, size_t pos = 0) const noexcept
    {
      size_t ret = npos;

      if (pos < size())
      {
        const_pointer p = traits_type::find(c_str() + pos, size() - pos, ch);
        if (p != nullptr)
        {
          ret = p - c_str();
        }
      }

      return ret;
    }

    /// Operations
    
    /// Compares two strings
    template<int Size2>
    int compare(const basic_string<Size2, value_type>& str) const noexcept { return traits_compare(data(), length(), str.data(), str.length()); }

    /// Compares this string to the null-terminated character sequence beginning at the character pointed to by s
    int compare(const_pointer s) const noexcept { return traits_compare(data(), length(), s, traits_type::length(s)); }

  };

  // ---------------------------------------------------
  /// Error codes for string function such as strtoi64
  // ---------------------------------------------------
  typedef enum
  {
    ERR_NONE = 0,   ///< no error
    ERR_RANGE       ///< out of range
  } tStringError;

  // ---------------------------------------------------
  /// Interprets a signed integer value in the string str.
  /// (optional) plus or minus sign
  /// (optional) prefix 0 for octal base (applies only when base is 8 or 0)
  /// (optional) prefix 0x or 0X for hexadecimal base (applies only when base is 16 or 0)
  // ---------------------------------------------------
  template<typename T>
  T strtoi(const char* str, char** endptr, int base, tStringError * err = nullptr)
  {
    bool negative = false;
    bool foundDigit = false;
    T value = static_cast<T>(0);
    tStringError strerr = tStringError::ERR_NONE;

    // Set endptr if provided, will be updated later if a valid digit is found
    if (endptr != nullptr)
    {
      *endptr = (char*)str;
    }

    // Skip leading whitespace
    while ((*str != 0) && isspace(*str))
    {
      str++;
    }

    // Handle sign
    if (*str == '-')
    {
      negative = true;
      str++;
    }
    else if (*str == '+')
    {
      str++;
    }
    else
    {
      // nothing to do
    }

    // Determine base if not specified
    if (base == 0)
    {
      if (*str == '0')
      {
        if ((str[1] == 'x') || (str[1] == 'X'))
        {
          base = 16;
          str += 2;
        }
        else
        {
          base = 8;
          str++;
        }
      }
      else
      {
        base = 10;
      }
    }

    // Skip a leading 0x or 0X
    if (base == 16)
    {
      if (*str == '0')
      {
        if ((str[1] == 'x') || (str[1] == 'X'))
        {
          str += 2;
        }
      }
    }

    // Convert digits to value
    while (true)
    {
      char digit;

      if ((*str >= '0') && (*str <= '9'))
      {
        digit = *str - '0';
      }
      else if ((*str >= 'A') && (*str <= 'Z'))
      {
        digit = (*str - 'A') + 10;
      }
      else if ((*str >= 'a') && (*str <= 'z'))
      {
        digit = (*str - 'a') + 10;
      }
      else
      {
        // Invalid character, stop conversion
        if ((!foundDigit) && (base == 8))
        {
          // special case: base is 0 or 8 and string is "0":
          // ensure that endptr is set later on
          foundDigit = true;
        }
        break;
      }

      if (digit >= base)
      {
        // Digit out of range for base, stop conversion
        break;
      }

      // Apply sign. Sign is applied here to ensure overflow check for signed integer values
      if (negative)
      {
        digit = -digit;
      }

      foundDigit = true;
      if (util::math::muladd_overflow(value, static_cast<T>(base), static_cast<T>(digit), &value))
      {
        // overflow, stop conversion and report error
        strerr = tStringError::ERR_RANGE;
        break;
      }
      str++;
    }

    // Set endptr if provided, if a valid digit was found and if there was no error
    if (endptr != nullptr)
    {
      if (foundDigit && (strerr == ERR_NONE))
      {
        *endptr = (char*)str;
      }
    }

    // Set err if provided
    if (err != nullptr)
    {
      *err = strerr;
    }

    return value;
  }

  // ---------------------------------------------------
  /// Interprets an unsigned integer value in the string str.
  /// (optional) plus or minus sign
  /// (optional) prefix 0 for octal base (applies only when base is 8 or 0)
  /// (optional) prefix 0x or 0X for hexadecimal base (applies only when base is 16 or 0)
  // ---------------------------------------------------
  template<typename T>
  T strtoui(const char* str, char** endptr, int base, tStringError* err = nullptr)
  {
    bool foundDigit = false;
    T value = static_cast<T>(0);
    tStringError strerr = tStringError::ERR_NONE;

    // Set endptr if provided, will be updated later if a valid digit is found
    if (endptr != nullptr)
    {
      *endptr = (char*)str;
    }

    // Skip leading whitespace
    while ((*str != 0) && isspace(*str))
    {
      str++;
    }

    // Determine base if not specified
    if (base == 0)
    {
      if (*str == '0')
      {
        if ((str[1] == 'x') || (str[1] == 'X'))
        {
          base = 16;
          str += 2;
        }
        else
        {
          base = 8;
          str++;
        }
      }
      else
      {
        base = 10;
      }
    }

    // Skip a leading 0x or 0X
    if (base == 16)
    {
      if (*str == '0')
      {
        if ((str[1] == 'x') || (str[1] == 'X'))
        {
          str += 2;
        }
      }
    }

    // Convert digits to value
    while (true)
    {
      char digit;

      if ((*str >= '0') && (*str <= '9'))
      {
        digit = *str - '0';
      }
      else if ((*str >= 'A') && (*str <= 'Z'))
      {
        digit = (*str - 'A') + 10;
      }
      else if ((*str >= 'a') && (*str <= 'z'))
      {
        digit = (*str - 'a') + 10;
      }
      else
      {
        // Invalid character, stop conversion
        if ((!foundDigit) && (base == 8))
        {
          // special case: base is 0 or 8 and string is "0":
          // ensure that endptr is set later on
          foundDigit = true;
        }
        break;
      }

      if (digit >= base)
      {
        // Digit out of range for base, stop conversion
        break;
      }

      foundDigit = true;

      if (util::math::muladd_overflow(value, static_cast<T>(base), static_cast<T>(digit), &value))
      {
        // overflow, stop conversion and report error
        strerr = tStringError::ERR_RANGE;
        break;
      }

      str++;
    }

    // Set endptr if provided, if a valid digit was found and if there was no error
    if (endptr != nullptr)
    {
      if (foundDigit && (strerr == ERR_NONE))
      {
        *endptr = (char*)str;
      }
    }

    // Set err if provided
    if (err != nullptr)
    {
      *err = strerr;
    }

    return value;
  }

  // ---------------------------------------------------
  /// Interprets a signed integer value in the string str.
  /// If str is empty or does not have the expected form, no conversion is performed.
  /// If no conversion is performed, pos is set to 0.
  /// If the converted value falls out of range of corresponding return type, a range error occurs (setting errno to ERANGE)
  /// 
  /// @param str    the string to be interpreted
  /// @param pos    if not null, address of an integer to store the number of characters processed
  /// @param base   base of the interpreted integer value
  // ---------------------------------------------------
  template<typename T, int Size>
  T stoi(const basic_string<Size, char>& str, size_t* pos = nullptr, int base = 10)
  {
    char* endpos;
    const char* startpos = str.c_str();

    T v = strtoi<T>(startpos, &endpos, base);

    if (startpos == endpos)
    {
      // something went wrong (str is empty or does not have the expected form)
      v = 0;
    }

    // Set pos if provided
    if (pos != nullptr)
    {
      *pos = endpos - startpos;
    }

    return v;
  }

  // ---------------------------------------------------
  /// Interprets an unsigned integer value in the string str
  /// 
  /// @param str    the string to be interpreted
  /// @param pos    if not null, address of an integer to store the number of characters processed
  /// @param base   base of the interpreted integer value
  // ---------------------------------------------------
  template<typename T, int Size>
  T stoui(const basic_string<Size, char>& str, size_t* pos = nullptr, int base = 10)
  {
    char* endpos;
    const char* startpos = str.c_str();

    T v = strtoui<T>(startpos, &endpos, base);

    if (startpos == endpos)
    {
      // something went wrong (str is empty or does not have the expected form)
      v = 0;
    }

    // Set pos if provided
    if (pos != nullptr)
    {
      *pos = endpos - startpos;
    }

    return v;
  }

  // ---------------------------------------------------
  /// Converts a signed integer to a string
  // ---------------------------------------------------
  template<int Size, class CharT>
  void to_string(int value, util::basic_string<Size, CharT>& str)
  {
    int base = 10;
    str = ::itoa(value, str.begin(), base);
  }

} // namespace util

// ---------------------------------------------------
/// Returns a string containing characters from lhs followed by the characters from rhs.
// ---------------------------------------------------
template<int Size, class CharT>
util::basic_string<Size, CharT> operator+(const util::basic_string<Size, CharT>& lhs, const util::basic_string<Size, CharT>& rhs)
{
  util::basic_string<Size, CharT> str = lhs;
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