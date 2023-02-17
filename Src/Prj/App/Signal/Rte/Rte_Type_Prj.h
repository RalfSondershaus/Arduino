/**
 * @file Prj/Signal/Rte_Type_Prj.h
 *
 * @brief Defines project specific types for RTE for project Signal.
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

#ifndef RTE_TYPE_PRJ_H_
#define RTE_TYPE_PRJ_H_

#include <Std_Types.h>
#include <Cfg_Prj.h>
#include <Platform_Limits.h>
#include <Cal/CalM_Type.h>
#include <Util/Array.h>

// forward declaration for servers
namespace cal
{
  class CalM;
}
namespace signal
{
  class InputCommand;
}

namespace rte
{
  /// Commands from inputs
  typedef uint8 cmd_type;

  constexpr cmd_type kInvalidCmd = platform::numeric_limits<cmd_type>::max_();

  /// The AD classified values are written into an array of this type
  typedef util::array<cmd_type, cfg::kNrClassifiers> classified_values_array;

  /// Target intensities for a signal
  typedef struct
  {
    util::array<intensity8_t, cfg::kNrSignalTargets> intensities;
    uint8 changeOverTime; ///< [10 ms] dim time if aspect changes
  } signal_intensity_type;

  /// Target intensities for each signal
  typedef util::array<signal_intensity_type, cfg::kNrSignals> signal_intensity_array_type;

  /// The target intensities are written into an array of this type
  typedef util::array<intensity8_t, cfg::kNrOnboardTargets> onboard_intensity_array;
  typedef util::array<intensity8_t, cfg::kNrExternalTargets> external_intensity_array;

  // -----------------------------------------------------------------------------------
  /// SR interface for classified values
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_sr_array<classified_values_array> Ifc_ClassifiedValues;

  // -----------------------------------------------------------------------------------
  /// SR interface for signal target intensities
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_sr_array< signal_intensity_array_type> Ifc_SignalTargetIntensities;

  // -----------------------------------------------------------------------------------
  /// SR interface for target intensities
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_sr_array<onboard_intensity_array> Ifc_OnboardTargetIntensities;
  typedef rte::ifc_sr_array<external_intensity_array> Ifc_ExternalTargetIntensities;

  // -----------------------------------------------------------------------------------
  /// CS interface for calibration values
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_cs<const cal::signal_cal_type *          , cal::CalM> Ifc_Cal_Signal;
  typedef rte::ifc_cs<const cal::input_classifier_cal_type *, cal::CalM> Ifc_Cal_InputClassifier;

  // -----------------------------------------------------------------------------------
  /// CS interface for commands
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_cs<cmd_type, signal::InputCommand, cal::input_type> Ifc_Rte_GetCommand;

} // namespace rte

#endif // RTE_TYPE_PRJ_H_
