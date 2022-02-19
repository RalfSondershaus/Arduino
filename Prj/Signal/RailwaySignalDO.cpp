/**
 * Class RailwaySignalDO (digital output).
 */

#include <RailwaySignalDO.h>

#define RAILWAYSIGNALDO_INVALID_PORT      (-1)

static inline bool isPortValid(int p)
{
  return (p != RAILWAYSIGNALDO_INVALID_PORT);
}

/// Constructor
RailwaySignalDO::RailwaySignalDO(const IfcSignalTarget& tgt) : mIfcSignalTarget(tgt)
  , nPortRed1(RAILWAYSIGNALDO_INVALID_PORT)
  , nPortRed2(RAILWAYSIGNALDO_INVALID_PORT)
  , nPortGreen(RAILWAYSIGNALDO_INVALID_PORT)
  , nPortYellow(RAILWAYSIGNALDO_INVALID_PORT)
  , nPortWhite(RAILWAYSIGNALDO_INVALID_PORT)
{}

/// Constructor
RailwaySignalDO::RailwaySignalDO(const IfcSignalTarget& tgt, int nPRed1, int nPRed2, int nPGreen, int nPYellow, int nPWhite)
  : mIfcSignalTarget(tgt)
  , nPortRed1(nPRed1)
  , nPortRed2(nPRed2)
  , nPortGreen(nPGreen)
  , nPortYellow(nPYellow)
  , nPortWhite(nPWhite)
{}

/// Set output values
void RailwaySignalDO::update() const
{
  if (isPortValid(nPortRed1))   { digitalWrite(nPortRed1,   (mIfcSignalTarget.getRed1()    > 0) ? (HIGH) : (LOW)); }
  if (isPortValid(nPortRed2))   { digitalWrite(nPortRed2,   (mIfcSignalTarget.getRed2()    > 0) ? (HIGH) : (LOW)); }
  if (isPortValid(nPortGreen))  { digitalWrite(nPortGreen,  (mIfcSignalTarget.getGreen1()  > 0) ? (HIGH) : (LOW)); }
  if (isPortValid(nPortYellow)) { digitalWrite(nPortYellow, (mIfcSignalTarget.getYellow1() > 0) ? (HIGH) : (LOW)); }
  if (isPortValid(nPortWhite))  { digitalWrite(nPortWhite,  (mIfcSignalTarget.getWhite1()  > 0) ? (HIGH) : (LOW)); }
}

/// initialize output ports
void RailwaySignalDO::init()
{
  if (isPortValid(nPortRed1))   { pinMode(nPortRed1  , OUTPUT); }
  if (isPortValid(nPortRed2))   { pinMode(nPortRed2  , OUTPUT); }
  if (isPortValid(nPortGreen))  { pinMode(nPortGreen , OUTPUT); }
  if (isPortValid(nPortYellow)) { pinMode(nPortYellow, OUTPUT); }
  if (isPortValid(nPortWhite))  { pinMode(nPortWhite , OUTPUT); }
}

// EOF