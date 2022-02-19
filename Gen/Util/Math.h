/**
  * @file UtilMath.h
  */
#ifndef UTIL_MATH_H
#define UTIL_MATH_H

namespace Util
{
  // -----------------------------------------------------------------------------
  /// Returns smallest integral value that is greater than or equal to 
  /// dividend / divisor.<BR>
  /// Returns <BR>
  /// <CODE>
  /// dividend + (divisor - 1)   dividend - 1     <BR>
  /// ------------------------ = ------------ + 1 <BR>
  ///        divisor                divisor       <BR>
  /// </CODE>
  // -----------------------------------------------------------------------------
  template<typename T>
  T ceilT(const T dividend, const T divisor)
  {
    return (T)((dividend - (T)1) / divisor + (T)1);
  }
} // namespace Util

#endif // UTIL_MATH_H