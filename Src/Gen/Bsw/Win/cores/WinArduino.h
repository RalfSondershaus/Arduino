/**
 * @file WinArduino.h
 * @author Ralf Sondershaus
 *
 * @brief Arduino interface to run on Win32
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

#ifndef WINARDUINO_H_
#define WINARDUINO_H_

#include <Std_Types.h>

#ifdef __cplusplus
extern "C"{
#endif

#define INPUT 0x0
#define OUTPUT 0x1

void init(void);

void pinMode(uint8_t, uint8_t);
void digitalWrite(uint8_t, uint8_t);
int digitalRead(uint8_t);
int analogRead(uint8_t);
void analogReference(uint8_t mode);
void analogWrite(uint8_t, int);

uint32 millis(void);
uint32 micros(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);

void attachInterrupt(uint8_t, void (*)(void), int mode);
void detachInterrupt(uint8_t);

// MEGA version
#define digitalPinHasPWM(p)         (((p) >= 2 && (p) <= 13) || ((p) >= 44 && (p)<= 46))

void setup(void);
bool loop(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // WINARDUINO_H_
