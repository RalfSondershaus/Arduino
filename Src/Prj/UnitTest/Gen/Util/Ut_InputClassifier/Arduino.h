/**
 * @file Arduino.h
 * @author Ralf Sondershaus
 *
 * @descr Stub file for Google Test for Gen/Uil/InputClassifier.h
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ARDUINO_H__
#define ARDUINO_H__

/// Return current time [us]
unsigned long micros(); 
/// Return current time [ms]
unsigned long millis();
/// read analog input
int analogRead(int);

#endif // ARDUINO_H__
