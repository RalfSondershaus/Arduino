/**
 * @file Prj/Signal/Rte_Type_Prj.h
 *
 * @brief Defines project specific types for the RTE.
 *
 * @copyright Copyright 2022-2024 Ralf Sondershaus
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
  class LedRouter;
  class InputClassifier;
}

namespace rte
{
  /// Commands from inputs
  typedef uint8 cmd_type;

  constexpr cmd_type kInvalidCmd = platform::numeric_limits<cmd_type>::max_();

  /// The AD classified values (from buttons) are written into an array of this type
  using classified_values_array = util::array<cmd_type, cfg::kNrClassifiers>;
  using ad_values_array = util::array<uint16, cfg::kNrClassifiers>;
  using dcc_commands_array = util::array<cmd_type, cfg::kNrSignals>;

  /// The DCC values are written into an array of this type.
  /// For signals:
  /// Array index 0 corresponds to DCC address as defined by CV values.
  /// Array index 1 corresponds to DCC address + 1.
  /// Array index 2 corresponds to DCC address + 2, etc.
  using dcc_values_array = util::array<cmd_type, cfg::kNrSignals>;

  /// The target intensities are written into an array of this type
  using onboard_target_array = util::array<intensity8_255, cfg::kNrOnboardTargets>;
  using external_target_array = util::array<intensity8_255, cfg::kNrExternalTargets>;

  /// @brief use the target_type of calibration
  using target_type = cal::target_type;

  // -----------------------------------------------------------------------------------
  /// SR interface for classified values
  // -----------------------------------------------------------------------------------
  using Ifc_ClassifiedValues = rte::ifc_sr_array<classified_values_array>;
  using Ifc_ADValues = rte::ifc_sr_array<ad_values_array>;

  using Ifc_DccCommands = rte::ifc_sr_array<dcc_commands_array>;
  // -----------------------------------------------------------------------------------
  /// SR interface for onboard and external target duty cycles
  // -----------------------------------------------------------------------------------
  using Ifc_OnboardTargetDutyCycles = rte::ifc_sr_array<onboard_target_array>;
  using Ifc_ExternalTargetDutyCycles = rte::ifc_sr_array<external_target_array>;

  /// SR interface for DCC address (calculated from calibration data)
  using Ifc_Cal_DccAddress = rte::ifc_sr<uint16>;

  // -----------------------------------------------------------------------------------
  /// CS interface for calibration values
  // -----------------------------------------------------------------------------------
  using Ifc_Cal_Signal          = rte::ifc_cs<const cal::signal_cal_type *          , cal::CalM>;
  using Ifc_Cal_InputClassifier = rte::ifc_cs<const cal::classifier_array_cal_type *, cal::CalM>;
  using Ifc_Cal_Led             = rte::ifc_cs<const cal::led_cal_type *             , cal::CalM>;
  using Ifc_Cal_Base_CV         = rte::ifc_cs<const cal::base_cv_cal_type *         , cal::CalM>;
  using Ifc_Cal_Get_CV          = rte::ifc_cs<uint8                                 , cal::CalM>;

  using Ifc_Cal_Set_Signal          = rte::ifc_cs<ret_type, cal::CalM, uint8, const cal::signal_type&, bool>;
  using Ifc_Cal_Set_CV              = rte::ifc_cs<ret_type, cal::CalM, uint16, uint8>;
//using Ifc_Cal_Set_InputClassifier = rte::ifc_cs<ret_type, cal::CalM, uint8, const cal::input_classifier_single_type&, bool>;
  using Ifc_Cal_Set_Defaults        = rte::ifc_cs<ret_type, cal::CalM, void>;

  /**
   * @brief Inform classifier about new coding data.
   */
  using Ifc_Rte_UpdateConfigForClassifier = rte::ifc_cs<ret_type, signal::InputClassifier, uint8>;

  // -----------------------------------------------------------------------------------
  /// CS interface for commands
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_cs<cmd_type, signal::InputCommand, cal::input_type> Ifc_Rte_GetCommand;

  // -----------------------------------------------------------------------------------
  /// CS interface for LED intensity target values and speed target values
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_cs<ret_type, signal::LedRouter, const rte::target_type, const rte::intensity16, const rte::speed16_ms_t> Ifc_Rte_LedSetIntensityAndSpeed;

  // -----------------------------------------------------------------------------------
  /// CS interface type for LED intensity target values
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_cs<ret_type, signal::LedRouter, const rte::target_type, const rte::intensity16> Ifc_Rte_LedSetIntensity;

  // -----------------------------------------------------------------------------------
  /// CS interface type for LED speed target values
  // -----------------------------------------------------------------------------------
  typedef rte::ifc_cs<ret_type, signal::LedRouter, const rte::target_type, const rte::speed16_ms_t> Ifc_Rte_LedSetSpeed;

} // namespace rte

#endif // RTE_TYPE_PRJ_H_
