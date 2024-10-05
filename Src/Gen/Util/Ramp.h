/**
  * @file Ramp.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Defines a class to calculate (dim) ramps
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

#ifndef UTIL_RAMP_H_
#define UTIL_RAMP_H_

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Math.h>

namespace util
{
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
    /// If target value is reached before timestep is elapsed, slope is set to maximum value.
    /// 
    /// @param slope [./ms]
    /// @param timestep [ms]
    void update_delta_from_slope(base_type slope, base_type timestep)
    {
      //delta = slope * timestep
      if (math::mul_overflow(slope, timestep, &delta))
      {
        // overflow detected, fallback is maximal value for delta
        // (If target value is reached before timestep is elapsed, slope is set to maximum value.)
        delta = platform::numeric_limits<base_type>::max_();
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

    /// @brief Set to no ramp (initialize to default values, same as default constructor)
    void clear()
    {
      cur = 0;
      tgt = 0;
      delta = 0;
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
    /// @param slope Slope to reach the target value.
    /// @param timestep Cycle time for updates. Defines update rate per cyclic call.
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

#endif // UTIL_RAMP_H
