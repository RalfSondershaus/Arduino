/**
 * @file Ut_Signal/CalM.cpp
 *
 * @author Ralf Sondershaus
 *
 * @brief  Access to calibration parameters
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

#include <Cal/CalM.h>
#include <Rte/Rte.h>

#include "CalM_config.h"

namespace cal
{
  // -----------------------------------------------
  /// Constructor
  // -----------------------------------------------
  CalM::CalM() 
    : signals{ CAL_SIGNAL_ARRAY }
    , input_classifiers{ CAL_INPUT_CLASSIFIER_CFG }
  {}

  // -----------------------------------------------
  /// Init runable
  // -----------------------------------------------
  void CalM::init()
  {
    valid = true;
  }

  // -----------------------------------------------
  /// Runable 10 ms
  // -----------------------------------------------
  void CalM::cycle100()
  {
  }

} // namespace cal
