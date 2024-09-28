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

#include <Arduino.h>
#include <Util/Array.h>

// we need chrono if millis is based on system tick instead of stub variables
#if CFG_STUB_MILLIS == CFG_STUB_OFF
#include <chrono>
#endif

namespace stubs
{
  util::array<uint8, NPINS> pinMode;
  util::array<uint8, NPINS> digitalWrite;
  util::array<int, NPINS> analogWrite;
  util::array<int, NPINS> analogRead;
#if CFG_STUB_MILLIS == CFG_STUB_ON
  unsigned long micros;
  unsigned long millis;
#endif
}

// ------------------------------------------------------------------
/// Initialize internals
// ------------------------------------------------------------------
void init(void)
{
  util::fill(stubs::pinMode.begin(), stubs::pinMode.end(), 0);
  util::fill(stubs::digitalWrite.begin(), stubs::digitalWrite.end(), 0);
  util::fill(stubs::analogWrite.begin(), stubs::analogWrite.end(), 0);
  util::fill(stubs::analogRead.begin(), stubs::analogRead.end(), 0);
  stubs::micros = 0;
  stubs::millis = 0;
}

// ------------------------------------------------------------------
/// Set mode of pin ucPin
// ------------------------------------------------------------------
void pinMode(uint8_t ucPin, uint8_t ucMode)
{
  stubs::pinMode[ucPin] = ucMode;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
void digitalWrite(uint8_t ucPin, uint8_t ucVal)
{
  stubs::digitalWrite[ucPin] = ucVal;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
int digitalRead(uint8_t ucPin)
{
  return 0;
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
int analogRead(uint8_t ucPin)
{
  return stubs::analogRead[ucPin];
}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
void analogReference(uint8_t mode)
{}

// ------------------------------------------------------------------
/// 
// ------------------------------------------------------------------
void analogWrite(uint8_t ucPin, int nVal)
{
  stubs::analogWrite[ucPin] = nVal;
}

// ------------------------------------------------------------------
/// [ms] Return time
// ------------------------------------------------------------------
unsigned long millis()
{
#if CFG_STUB_MILLIS == CFG_STUB_ON
  return stubs::millis;
#else
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).count();
#endif
}

// ------------------------------------------------------------------
/// [us] Return time
// ------------------------------------------------------------------
unsigned long micros()
{
#if CFG_STUB_MICROS == CFG_STUB_ON
  return stubs::micros;
#else
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now()).count();
#endif
}
