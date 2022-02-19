/**
 * Interface for signal target intensities.
 */

#ifndef IFC_SIGNAL_TARGET_H__
#define IFC_SIGNAL_TARGET_H__

#include <IfcBase.h>

// -----------------------------------------------------------------------------------
/// Interface for signal target intensities
// -----------------------------------------------------------------------------------
class IfcSignalTarget : public IfcBase
{
public:
  /// [%] target intensity
  typedef unsigned char tIntensity1;
  /// [% / 10 ms] speed to reach a target intensity
  typedef unsigned char tSpeed1_10;

  /// Default constructor
  IfcSignalTarget()
  {}
  /// Destructor: nothing to do
  ~IfcSignalTarget()
  {}
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
