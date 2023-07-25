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
