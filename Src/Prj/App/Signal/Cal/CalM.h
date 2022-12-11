/**
 * @file CalM.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Access to calibration parameters
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

#ifndef CAL_H_
#define CAL_H_

#include <Std_Types.h>
#include <Cal/CalM_Type.h>

namespace cal
{

  class CalM
  {
  public:
    /// coding data for signals
    signal_array signals;

    /// Init runable
    void init();
    /// Runable 100 ms
    void cycle100();

    /// Obsolete: reference to coding parameters
    const signal_array& ref_signal_array() const { return signals; }
  };

} // namespace cal

#endif // CAL_H_
