// ---------------------------------------------------
/// Defines interrupt service routine for DCC
// ---------------------------------------------------
#include <Arduino.h>
#include "ISR_Dcc.h"
#include "PeriodSM.h"
#include "DccInterpreter.h"

/// The state machine for short and long parts (-> detecting "1" and "0" and INVALID)
PeriodStateMachine periodSM;

unsigned long ulTimeStampPrev = 0;

// ---------------------------------------------------
/// Interrupt service routine
// ---------------------------------------------------
void ISR_Dcc(void)
{
  unsigned long ulTimeStamp = micros();
  unsigned long ulTimeDiff;

  if (ulTimeStampPrev > 0u)
  {
    // second call or beyond
    ulTimeDiff = ulTimeStamp - ulTimeStampPrev;
    periodSM.execute(ulTimeDiff);
  }

  ulTimeStampPrev = ulTimeStamp;
}

/// For debugging: get number of calls per state
unsigned int Dcc_GetNrCalls(unsigned int unState)
{
  return periodSM.getNrCalls(unState);
}

/// For debugging: get number of "1"
unsigned int Dcc_GetNrOne()
{
  return periodSM.getNrOne();
}

/// For debugging: get number of "0"
unsigned int Dcc_GetNrZero()
{
  return periodSM.getNrZero();
}

/// For debugging: get byte
unsigned int Dcc_GetByte(unsigned int idx)
{
  return periodSM.getByte(idx);
}
