/**
  * @file Algorithm.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Algorithms
  *
  * @copyright Copyright 2022 Ralf Sondershaus
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
  * See <https://www.gnu.org/licenses/>.
  */

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Math.h>

namespace util
{
  // ------------------------------------------------------------------------------
  /// Returns true if lowerlimit <= v < upperlimit.
  // ------------------------------------------------------------------------------
  template<typename T>
  bool is_in_interval(const T v, const T lowerlimit, const T upperlimit)
  {
    return ((v >= lowerlimit) && (v < upperlimit));
  }

  // ------------------------------------------------------------------------------
  /// Returns an interator to the element that equals v.
  /// Returns last if the element is not found.
  /// Performs a linear search.
  // ------------------------------------------------------------------------------
  template<class InputIt, class T>
  InputIt find(InputIt first, InputIt last, const T& v)
  {
    while (first != last)
    {
      if (*first == v)
      {
        break;
      }
      first++;
    }
    return first;
  }

  // ------------------------------------------------------------------------------
  /// Copies the elements in the range, defined by [first, last), to another range 
  /// beginning at d_first (copy destination range).
  // ------------------------------------------------------------------------------
  template< class InputIt, class OutputIt >
  OutputIt copy(InputIt s_first, InputIt last, OutputIt d_first)
  {
    while (first != last)
    {
      *d_first++ = *s_first++;
    }

    return d_first;
  }

  // ------------------------------------------------------------------------------
  /// Copies exactly count values from the range beginning at first to the range 
  /// beginning at d_first.
  /// 
  /// Returns iterator in the destination range, pointing past the last element 
  /// copied if count > 0 or d_first otherwise.
  // ------------------------------------------------------------------------------
  template< class InputIt, class Size, class OutputIt >
  OutputIt copy_n(InputIt s_first, Size count, OutputIt d_first)
  {
    while (count > 0U)
    {
      *d_first++ = *s_first++;
      count--;
    }

    return d_first;
  }

  // ------------------------------------------------------------------------------
  /// Assigns the given value to the elements in the range [first, last)
  // ------------------------------------------------------------------------------
  template< class ForwardIt, class T >
  void fill(ForwardIt d_first, ForwardIt d_last, const T& value)
  {
    while (d_first != d_last)
    {
      *d_first++ = value;
    }
  }

  // ------------------------------------------------------------------------------
  /// Assigns the given value to the first count elements in the range beginning at 
  /// first if count > 0. Does nothing otherwise.
  // ------------------------------------------------------------------------------
  template< class OutputIt, class Size, class T >
  OutputIt fill_n(OutputIt d_first, Size count, const T& value)
  {
    while (count > 0U)
    {
      *d_first++ = value;
      count--;
    }

    return d_first;
  }

  // ------------------------------------------------------------------------------
  /// Copies count bytes from the object pointed to by src to the object pointed to 
  /// by dest. Both objects are reinterpreted as arrays of unsigned char.
  /// Returns dest.
  // ------------------------------------------------------------------------------
  void* memcpy(void* dest, const void* src, size_t count);

  template<typename T, typename... U>
  T make_number(U...);

  template<uint16, uint8, uint8>
  uint16 make_number(uint8 hi, uint8 lo)
  {
    return static_cast<uint16>((static_cast<uint16>(hi) << 8U) | static_cast<uint16>(lo));
  }
  
  // ------------------------------------------------------------------------------
  /// Returns the smaller of the given values.
  // ------------------------------------------------------------------------------
  template<class T>
  const T& min(const T& a, const T& b) 
  { 
    return (a < b) ? a : b; 
  }

  // ------------------------------------------------------------------------------
  /// Returns s1 + s2 and limits the sum to max value of T
  // ------------------------------------------------------------------------------
  template<typename T>
  T add_no_overflow(T s1, T s2)
  {
    T ret = s1 + s2;

    if (ret < s1)
    {
      // overflow
      ret = platform::numeric_limits<T>::max_();
    }

    return ret;
  }
  // ------------------------------------------------------------------------------
  /// Discretizes a ramp from ystart to yend over time dt with given timesteps.
  ///
  ///
  /// yend                     ----x
  ///                     ----x    |
  ///                ----x    |    |
  ///           ----x    |    |    |
  /// ystart   x    |    |    |    |
  ///          0                   dt
  ///               |----|
  ///              timestep
  // ------------------------------------------------------------------------------
  template<class T>
  class ramp
  {
  public:
    typedef T base_type;

  protected:
    base_type cur;
    base_type tgt;
    base_type delta;

    /// Initialize internal structure: ramp starts at ystart and reaches yend after time dt with step size defined by timestep
    void update_delta(base_type ystart, base_type yend, base_type dt, base_type timestep)
    {
      base_type dy;

      if (yend > ystart)
      {
        dy = yend - ystart;
      }
      else
      {
        dy = ystart - yend;
      }

      if (dt > static_cast<base_type>(0))
      {
        delta = util::math::muldiv_ceil<T>(dy, timestep, dt);
      }
      else
      {
        delta = dy;
      }
    }

    /// Initialize internal structure: ramp uses the given slope which is adjusted by timestep.
    /// 
    /// @param slope [./ms]
    /// @param timestep [ms]
    void update_delta_from_slope(base_type slope, base_type timestep)
    {
        delta = slope * timestep;
    }
  public:

    /// Construct to no ramp.
    ramp() : cur(static_cast<base_type>(0)), tgt(static_cast<base_type>(0)), delta(static_cast<base_type>(0))
    {
    }
    /// Construct. Calls init().
    explicit ramp(base_type ystart, base_type yend, base_type dt, base_type timestep)
    {
      init(ystart, yend, dt, timestep);
    }

    /// Return target values and current value
    base_type get_tgt() const noexcept { return tgt; }
    base_type get_cur() const noexcept { return cur; }

    /// Initialize: ramp starts at ystart and reaches yend after time dt with step size defined by timestep
    void init(base_type ystart, base_type yend, base_type dt, base_type timestep)
    {
      cur = ystart;
      tgt = yend;
      update_delta(ystart, yend, dt, timestep);
    }

    /// Initialize: ramp starts at current value (cur) and reaches yend after time dt with step size defined by timestep
    void init(base_type yend, base_type dt, base_type timestep)
    {
      tgt = yend;
      update_delta(cur, yend, dt, timestep);
    }

    /// Initialize ramp to reach yend with the given slope (speed).
    /// 
    /// The step size is calculated from slope and timestep by division slope / timestep.
    /// 
    /// The unit of slope is "units per time" where the unit of "time" needs to match the unit of timestep.
    /// For example, if the unit of timestep is [ms], the unit of slope must be [./ms].
    /// 
    /// @param yend Target value
    /// @param slope Speed / slope to reach the target value. 
    /// @timestep Cycle time for updates. Defines update rate per cyclic call.
    void init_from_slope(base_type yend, base_type slope, base_type timestep)
    {
      tgt = yend;
      update_delta_from_slope(slope, timestep);
    }

    /// Set new target value without changing the speed to reach the target value. The target value
    /// will be approached by the current speed starting from current value.
    void set_tgt(base_type yend)
    {
      tgt = yend;
    }

    /// Set a new speed / slope to reach the target value.
    /// 
    /// The step size is calculated from slope and timestep by division slope / timestep.
    /// 
    /// The unit of slope is "units per time" where the unit of "time" needs to match the unit of timestep.
    /// For example, if the unit of timestep is [ms], the unit of slope must be [./ms].
    void set_slope(base_type slope, base_type timestep)
    {
      update_delta_from_slope(slope, timestep);
    }

    /// Makes a step and returns current ramp value
    base_type step()
    {
      if (tgt == cur)
      {
        // intentionally left empty: cur is at tgt
      }
      else if (tgt > cur)
      {
        if ((tgt - cur) > delta)
        {
          cur += delta;
        }
        else
        {
          cur = tgt;
        }
      }
      else
      {
        if ((cur - tgt) > delta)
        {
          cur -= delta;
        }
        else
        {
          cur = tgt;
        }
      }

      return cur;
    }
  };
} // namespace util

#endif // ALGORITHM_H_
