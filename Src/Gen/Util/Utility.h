/**
  * @file Utility.h
  *
  * @author Ralf Sondershaus
  *
  */

#ifndef UTIL_UTILITY_H
#define UTIL_UTILITY_H

#include <Platform_Types.h>

namespace util
{
  /** 
   * remove_reference
   */ 
  template<typename T> struct remove_reference      { using type = T; };
  template<typename T> struct remove_reference<T&>  { using type = T; };
  template<typename T> struct remove_reference<T&&> { using type = T; };

  /** 
   * forward: conditional cast. 
   * 
   * Converts a value to an lvalue if initial argument is an lvalue.
   * Converts a value to an rvalue if initial argument is an rvalue.
   */
  template<typename T>
  constexpr T&& forward(T& t) noexcept
  {
    return static_cast<typename util::remove_reference<T>::type&&>(t); 
  }
  template<typename T>
  constexpr T&& forward(T&& t) noexcept
  {
    return static_cast<typename util::remove_reference<T>::type&&>(t);
  }

  /**
   * move: converts a value to an rvalue.
   */
  template<typename T>
  constexpr typename util::remove_reference<T>::type&& move(T&& t) noexcept
  { 
    return static_cast<typename util::remove_reference<T>::type&&>(t); 
  }
} // namespace util

#endif // UTIL_UTILITY_H