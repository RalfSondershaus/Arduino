#ifndef ISR_DCC_H
#define ISR_DCC_H

// ---------------------------------------------------
/// Interrupt service routine
// ---------------------------------------------------
void ISR_Dcc(void);

/// For debugging: get number of calls per state
unsigned int Dcc_GetNrCalls(unsigned int unState);
unsigned int Dcc_GetNrOne();
unsigned int Dcc_GetNrZero();
unsigned int Dcc_GetByte(unsigned int idx);

#endif // ISR_DCC_H