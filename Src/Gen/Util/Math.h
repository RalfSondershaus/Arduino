/**
  * @file UtilMath.h
  */
#ifndef UTIL_MATH_H
#define UTIL_MATH_H

#include <Util/Array.h>

namespace util
{
  namespace math
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
    inline T ceilt(const T dividend, const T divisor)
    {
      return static_cast<T>((dividend + (divisor - static_cast<T>(1))) / divisor);
    }

    // -----------------------------------------------------------------------------
    /// Returns the smallest value that is greater than (dividend1 * dividend2) / divisor.
    // -----------------------------------------------------------------------------
    template<typename T>
    inline T muldiv_ceil(const T dividend1, const T dividend2, const T divisor)
    {
      //#error "Template muldiv_ceil not supported for given type T."
      return static_cast<T>(0);
    }

    // -----------------------------------------------------------------------------
    /// Returns the smallest uint16 that is greater than (dividend1 * dividend2) / divisor.
    /// Partial specialization of base class for important use case T = uint16.
    // -----------------------------------------------------------------------------
    template<>
    inline uint16 muldiv_ceil<uint16>(const uint16 dividend1, const uint16 dividend2, const uint16 divisor)
    {
      uint32_t t = static_cast<uint32_t>(dividend1) * static_cast<uint32_t>(dividend2);
      t += static_cast<uint32_t>(divisor) - static_cast<uint32_t>(1);
      t /= static_cast<uint32_t>(divisor);
      return static_cast<uint16>(t);
    }

    // -----------------------------------------------------------------------------
    /// Returns the smallest uint32 that is greater than (dividend1 * dividend2) / divisor.
    /// Partial specialization of base class for important use case T = uint32.
    // -----------------------------------------------------------------------------
    template<>
    inline uint32 muldiv_ceil<uint32>(const uint32 dividend1, const uint32 dividend2, const uint32 divisor)
    {
      uint64_t t = static_cast<uint64_t>(dividend1) * static_cast<uint64_t>(dividend2);
      t += static_cast<uint64_t>(divisor) - static_cast<uint64_t>(1);
      t /= static_cast<uint64_t>(divisor);
      return static_cast<uint32>(t);
    }

    // -----------------------------------------------------------------------------
    /// Add with overflow check: returns true if an overflow is detected, false otherwise.
    /// Adds a and b and stores the result in r if no overflow is detected.
    /// Does not modify r if an overflow is detected.
    // -----------------------------------------------------------------------------
    template<typename T>
    bool add_overflow(T a, T b, T* r)
    {
      bool ret = false;

      if (((b > static_cast<T>(0)) && (a > (platform::numeric_limits<T>::max_() - b)))
       || ((b < static_cast<T>(0)) && (a < (platform::numeric_limits<T>::min_() - b))))
      {
        // overflow
        ret = true;
      }
      else
      {
        *r = a + b;
      }
      return ret;
    }

    // -----------------------------------------------------------------------------
    /// Substract with underflow check.
    /// Substracts b from a and stores the result in r.
    /// If b is positive and greater than a, r is set to zero (underflow).
    /// If b is negative and less than a, r is set to zero (crossing zero from negative numbers to positive numbers)
    // -----------------------------------------------------------------------------
    template<typename T>
    void sub_underflow_sat(T a, T b, T* r)
    {
      if (((b > static_cast<T>(0)) && (a < b))
       || ((b < static_cast<T>(0)) && (a > b)))
      {
        // underflow
        *r = 0;
      }
      else
      {
        *r = a - b;
      }
    }

    // -----------------------------------------------------------------------------
    /// Multiply with overflow check: returns true if an overflow is detected, false otherwise.
    /// Multiplies a and b and stores the result in r if no overflow is detected.
    /// Does not modify r if an overflow is detected.
    // -----------------------------------------------------------------------------
    template<typename T>
    bool mul_overflow(T a, T b, T* r)
    {
      bool ret = false;

      if (a > static_cast<T>(0))
      {
        if (b > static_cast<T>(0))
        {
          // a positive, b positive
          if (a > platform::numeric_limits<T>::max_() / b)
          {
            ret = true;
          }
        }
        else
        {
          // a positive, b nonpositive
          if (b < platform::numeric_limits<T>::min_() / a)
          {
            ret = true;
          }
        }
      }
      else
      {
        if (b > static_cast<T>(0))
        {
          // a nonpositive, b positive
          if (a < platform::numeric_limits<T>::min_() / b)
          {
            ret = true;
          }
        }
        else
        {
          // a nonpositive, b nonpositive
          if ((a != static_cast<T>(0)) && (b < (platform::numeric_limits<T>::max_() / a)))
          {
            ret = true;
          }
        }
      }

      // multiply only if there is no overflow
      if (ret == false)
      {
        *r = a * b;
      }

      return ret;
    }

    // -----------------------------------------------------------------------------
    /// Multiply-add with overflow check: returns true if an overflow is detected, false otherwise.
    /// Calculates (a * b) + c and stores the result in r if no overflow is detected.
    /// Does not modify r if an overflow is detected.
    // -----------------------------------------------------------------------------
    template<typename T>
    bool muladd_overflow(T a, T b, T c, T* r)
    {
      bool ret = false;

      if (util::math::mul_overflow(a, b, r))
      {
        ret = true;
      }
      else
      {
        if (util::math::add_overflow(*r, c, r))
        {
          ret = true;
        }
      }

      return ret;
    }

    // ----------------------------------------------------------------------------
    /// - class vec: A N-dimensional vector
    // ----------------------------------------------------------------------------
    template<int N, typename T>
    class vec
    {
    protected:
      typedef util::array<T, N> array_type;

      array_type c = { 0 };

    public:

      typedef T base_type;
      typedef size_t size_type;
      typedef vec<N,T> This;

      static constexpr size_type kDim = static_cast<size_type>(N);

      /// Construct and destrcut
      vec() = default;
      vec(const base_type coords[N])
      {
        for (size_type i = 0; i < kDim; i++)
        {
          c[i] = coords[i];
        }
      }
      vec(const This& v) { *this = v; }
      ~vec() = default;

      /// operator[]
      const base_type& operator[](size_type i) const { return c[i]; }
      base_type& operator[](size_type i) { return c[i]; }

      /// Set operations
      void set(size_type i, base_type v) { c[i] = v; }
      void zero() { c.fill(static_cast<base_type>(0)); }

      /// dot product
      T operator|(const This& v) const 
      { 
        T sum = static_cast<base_type>(0);
        for (size_type i = static_cast<size_type>(0); i < kDim; i++)
        {
          sum += c[i] * v[i];
        }
        return sum;
      }

      /// copy assignment
      This& operator=(const This& v) { c = v.c; return *this; }
      /// move assignment
      This& operator=(const This&& v) { c = v.c; return *this; }

      /// operators for addition, substraction, scaling
      This& operator+=(const This& rhs) { for (size_type i = static_cast<size_type>(0); i < kDim; i++) { c[i] += rhs.c[i]; } return *this; }
      This& operator-=(const This& rhs) { for (size_type i = static_cast<size_type>(0); i < kDim; i++) { c[i] -= rhs.c[i]; } return *this; }
      This& operator*=(const base_type f) { for (size_type i = static_cast<size_type>(0); i < kDim; i++) { c[i] *= f; } return *this; }
      friend This operator+(This lhs, const This& rhs) { lhs += rhs; return lhs; }
      friend This operator-(This lhs, const This& rhs) { lhs -= rhs; return lhs; }
      friend This operator*(const base_type f, This rhs) { rhs *= f; return rhs; }
    };

    // ----------------------------------------------------------------------------
    /// partial specialization of vec for important use case N = 2
    // ----------------------------------------------------------------------------
    template<typename T>
    class vec<2, T>
    {
    protected:
      typedef util::array<T, 2> array_type;

      array_type c = { 0 };

    public:

      typedef T base_type;
      typedef size_t size_type;
      typedef vec<2, T> This;

      vec() = default;
      vec(base_type x, base_type y) { c[0] = x; c[1] = y; }
      vec(const base_type coords[2]) { c = coords; }
      vec(const This& v) { c = v.c; }
      ~vec() = default;

      const base_type& operator[](size_type i) const { return c[i]; }
      base_type& operator[](size_type i) { return c[i]; }
      const base_type& x() const { return c[0]; }
      const base_type& y() const { return c[1]; }
      base_type& x() { return c[0]; }
      base_type& y() { return c[1]; }
      void set(size_type i, base_type v) { c[i] = v; }
      void set(base_type x, base_type y) { c[0] = x; c[1] = y; }
      void set(const base_type coords[2]) { c = coords; }
      void zero() { c[0] = c[1] = static_cast<base_type>(0); }

      /// dot product
      T operator|(const This& v) const { return (c[0] * v.c[0]) + (c[1] * v.c[1]); }

      /// copy assignment
      This& operator=(const This& v) { c[0] = v.c[0]; c[1] = v.c[1]; return *this; }
      /// move assignment
      This& operator=(const This&& v) { c[0] = v.c[0]; c[1] = v.c[1]; return *this; }

      /// addition, substraction, scaling
      This& operator+=(const This& rhs) { c[0] += rhs.c[0]; c[1] += rhs.c[1]; return *this; }
      This& operator-=(const This& rhs) { c[0] -= rhs.c[0]; c[1] -= rhs.c[1]; return *this; }
      This& operator*=(const base_type f) { c[0] *= f; c[1] *= f; return *this; }
      friend This operator+(This lhs, const This& rhs) { lhs += rhs; return lhs; }
      friend This operator-(This lhs, const This& rhs) { lhs -= rhs; return lhs; }
      friend This operator*(const base_type f, This rhs) { rhs *= f; return rhs; }
    };

    // ----------------------------------------------------------------------------
    /// partial specialization of vec for important use case N = 3
    // ----------------------------------------------------------------------------
    template<typename T>
    class vec<3, T>
    {
    protected:
      typedef util::array<T, 3> array_type;

      array_type c = { 0 };

    public:

      typedef T base_type;
      typedef size_t size_type;
      typedef vec<3, T> This;

      vec() = default;
      vec(base_type x, base_type y, base_type z) { c[0] = x; c[1] = y; c[2] = z; }
      vec(const base_type coords[3]) { c = coords; }
      vec(const This& v) { c = v.c; }
      ~vec() = default;

      const base_type& operator[](size_type i) const { return c[i]; }
      base_type& operator[](size_type i) { return c[i]; }
      const base_type& x() const { return c[0]; }
      const base_type& y() const { return c[1]; }
      const base_type& z() const { return c[2]; }
      base_type& x() { return c[0]; }
      base_type& y() { return c[1]; }
      base_type& z() { return c[2]; }
      void set(size_type i, base_type v) { c[i] = v; }
      void set(base_type x, base_type y, base_type z) { c[0] = x; c[1] = y; c[2] = z; }
      void set(const base_type coords[3]) { c = coords; }
      void zero() { c.fill(static_cast<base_type>(0)); }

      /// dot product
      T operator|(const This& v) const { return (c[0] * v.c[0]) + (c[1] * v.c[1]) + (c[2] * v.c[2]); }

      /// copy assignment
      This& operator=(const This& v) { c[0] = v.c[0]; c[1] = v.c[1]; c[2] = v.c[2]; return *this; }
      /// move assignment
      This& operator=(const This&& v) { c[0] = v.c[0]; c[1] = v.c[1]; c[2] = v.c[2]; return *this; }

      /// addition, substraction, scaling
      This& operator+=(const This& rhs) { c[0] += rhs.c[0]; c[1] += rhs.c[1]; c[2] += rhs.c[2]; return *this; }
      This& operator-=(const This& rhs) { c[0] -= rhs.c[0]; c[1] -= rhs.c[1]; c[2] -= rhs.c[2]; return *this; }
      This& operator*=(const base_type f) { c[0] *= f; c[1] *= f; c[2] *= f; return *this; }
      friend This operator+(This lhs, const This& rhs) { lhs += rhs; return lhs; }
      friend This operator-(This lhs, const This& rhs) { lhs -= rhs; return lhs; }
      friend This operator*(const base_type f, This rhs) { rhs *= f; return rhs; }
    };
  } // namespace math
} // namespace Util

#endif // UTIL_MATH_H