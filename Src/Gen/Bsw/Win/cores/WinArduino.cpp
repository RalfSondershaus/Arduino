/*
  main.cpp - Main loop for Arduino sketches
  Copyright (c) 2005-2013 Arduino Team.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
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
