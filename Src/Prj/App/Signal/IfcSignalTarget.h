/**
 * @file Rte_Cfg_Prj.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Interface for signal target intensities.
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

#ifndef IFC_SIGNAL_TARGET_H__
#define IFC_SIGNAL_TARGET_H__

#include <Std_Types.h>
#include <Rte/Rte_Type_Ifc.h>

// -----------------------------------------------------------------------------------
/// Interface for signal target intensities
// -----------------------------------------------------------------------------------
class ifc_signal_target : public rte::ifc_base
{
public:
  /// [%] target intensity
  typedef uint8 tIntensity1;
  /// [% / 10 ms] speed to reach a target intensity
  typedef uint8 tSpeed1_10;

  /// Default constructor
  ifc_signal_target() = default;
  /// Return target intensity for red 1.
  virtual tIntensity1 getRed1() const = 0;
  /// Return target intensity for red 2.
  virtual tIntensity1 getRed2() const = 0;
  /// Return target intensity for green.
  virtual tIntensity1 getGreen1() const = 0;
  /// Return target intensity for white.
  virtual tIntensity1 getWhite1() const = 0;
  /// Return target intensity for yellow.
  virtual tIntensity1 getYellow1() const = 0;
  /// Return target increment value (speed)
  virtual tSpeed1_10 getTargetInc() const = 0;
  /// Return target decrement value (speed)
  virtual tSpeed1_10 getTargetDec() const = 0;
};

#endif // IFC_SIGNAL_TARGET_H__
