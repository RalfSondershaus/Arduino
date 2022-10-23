/**
  * @file UtilMath.h
  */
#ifndef UTIL_MATH_H
#define UTIL_MATH_H

namespace Util
{
  // -----------------------------------------------------------------------------
  /// Returns smallest integral value that is greater than or equal to 
  /// dividend / divisor. Precondition: divisor > 0<BR>
  /// Returns <BR>
  /// <CODE>
  /// dividend + (divisor - 1) <BR>
  /// ------------------------ <BR>
  ///        divisor           <BR>
  /// </CODE>
  // -----------------------------------------------------------------------------
  template<typename T>
  T ceilT(const T dividend, const T divisor)
  {
    return static_cast<T>((dividend + (divisor - static_cast<T>(1))) / divisor);
  }
} // namespace Util

#endif // UTIL_MATH_H