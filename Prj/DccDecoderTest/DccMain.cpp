/// main.cpp
///
/// Test program for DccDecoder and associated classes
///

#include <../DccDecoder/PeriodSM.h>
#include <Arduino.h>

using namespace Dcc;

bool test1()
{
  DccInterpreter DccInterp;
  HalfBitStateMachine PSM(DccInterp);
  PSM.execute(64);
  if (PSM.getNrCalls(1) != 1) return false;
  PSM.execute(64);
  if (PSM.getNrCalls(2) != 1) return false;
  PSM.execute(64);
  PSM.execute(64);
  PSM.execute(64);
  PSM.execute(64);
  PSM.execute(64);
  return true;
}

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  Serial.print("test1: "); if (test1()) Serial.print("PASSED"); else Serial.print("FAILED");
  Serial.println();
}