/*
 * @file WinArduino.cpp
 *
 * Copyright (c) 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
*/

#include <WinArduino.h>
#include <chrono>

// ------------------------------------------------------------------
/// Initialize internals
// ------------------------------------------------------------------
void init(void)
{
}

// ------------------------------------------------------------------
/// Set mode of pin ucPin
// ------------------------------------------------------------------
void pinMode(uint8_t ucPin, uint8_t ucMode)
{
    (void) ucPin;
    (void) ucMode;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
void digitalWrite(uint8_t ucPin, uint8_t ucVal)
{
    (void) ucPin;
    (void) ucVal;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
int digitalRead(uint8_t ucPin)
{
  (void) ucPin;
  return 0;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
int analogRead(uint8_t ucPin)
{
  (void) ucPin;
  return 0;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
void analogReference(uint8_t mode)
{
  (void) mode;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
void analogWrite(uint8_t ucPin, int nVal)
{
  (void) ucPin;
  (void) nVal;
}

// ------------------------------------------------------------------
/// [ms] Return time
// ------------------------------------------------------------------
uint32 millis()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

// ------------------------------------------------------------------
/// [us] Return time
// ------------------------------------------------------------------
uint32 micros()
{
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
