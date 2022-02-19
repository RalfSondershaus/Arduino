/**
 * Class RailwaySignalDO (digital output).
 */

#ifndef RAILWAYSIGNALDO_H__
#define RAILWAYSIGNALDO_H__

#include <Arduino.h>
#include <IfcSignalTarget.h>

// -----------------------------------------------------------------------------------
/// Set target intensities based on input commands for signals
// -----------------------------------------------------------------------------------
class RailwaySignalDO
{
public:
  /// type for intensities [%]
  typedef IfcSignalTarget::tIntensity1 tIntensity1;

  /// The (required) port for signal targets
  const IfcSignalTarget& mIfcSignalTarget;

protected:
  int nPortRed1;    ///< Output digital port
  int nPortRed2;    ///< Output digital port
  int nPortGreen;   ///< Output digital port
  int nPortYellow;  ///< Output digital port
  int nPortWhite;   ///< Output digital port

public:
  /// Constructor
  RailwaySignalDO(const IfcSignalTarget& tgt);
  /// Constructor
  RailwaySignalDO(const IfcSignalTarget& tgt, int nPRed1, int nPRed2, int nPGreen, int nPYellow, int nPWhite);
  /// Destructor
  ~RailwaySignalDO()
  {}
  /// Set output values
  void update() const;
  /// initialize output ports
  void init();
};

#endif // RAILWAYSIGNAL_H__
