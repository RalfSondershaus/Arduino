/**
 * @file Signal/InputClassifier.h
 *
 * @author Ralf Sondershaus
 *
 * @brief The classifier interface.
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

#ifndef SIGNAL_INPUT_CLASSIFIER_H_
#define SIGNAL_INPUT_CLASSIFIER_H_

#include "Cfg_Prj.h"
#include <Rte/Rte_Type.h>
#include <Cal/CalM_Type.h>
#include <Util/Array.h>
#include <Util/Classifier.h>

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// Read AD values, classify and write classified values onto the RTE.
  // -----------------------------------------------------------------------------------
  class InputClassifier
  {
  public:
    using classifier_array_type = util::classifier_array<cfg::kNrClassifiers, cfg::kNrClassifierClasses>;
    using cal_const_pointer = const cal::classifier_array_cal_type *;
    using classified_values_array_type = classifier_array_type::classified_values_array_type;
    
  protected:
    classifier_array_type classifiers;

  public:
    /// Construct
    InputClassifier() {}

    // Runables
    /// @brief Init runable
    /// @note Average run time 10 usec @ATmega2560 @16 MHz
    void init();
    /// @brief Cyclic runable
    /// @note Average run time 85 usec @ATmega2560 @16 MHz
    void cycle();

    /**
     * @brief CS Interface: call this function when coding parameters have been changed.
     */
    rte::ret_type set_config(uint8 classifier_pos);
  };
} // namespace signal

#endif // SIGNAL_INPUT_CLASSIFIER_H_
