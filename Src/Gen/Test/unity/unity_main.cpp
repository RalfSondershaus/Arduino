/**
  * @file unity_main.cpp
  * @author Ralf Sondershaus
  *
  * @brief Defines hook functions for loop, setup, and print to stdout
  * 
  * @copyright Copyright 2024 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#ifdef ARDUINO
#include <Arduino.h>
#endif

// -----------------------------------------------------------------------
// Prototypes that are defined in each Test.cpp
// -----------------------------------------------------------------------
bool test_loop();
void test_setup();


// -----------------------------------------------------------------------
/// Arduino interface with methods loop and setup.
/// Arduino.h defines loop and setup as C functions, so we need to do
/// the same here.
// -----------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------
/// Arduinos loop() method.
/// Is called on Arduino (ARDUINO) or on Windows (WIN32).
// -----------------------------------------------------------------------
#ifdef WIN32
bool loop(void)
{
  return test_loop();
}
#else
void loop(void)
{
  (void) test_loop();
}
#endif

// -----------------------------------------------------------------------
/// Arduinos setup() method.
/// Is called on Arduino (ARDUINO) or on Windows (WIN32).
// -----------------------------------------------------------------------
void setup(void)
{
#ifdef ARDUINO
  Serial.begin(19200);
#endif
  test_setup();
}

#ifdef ARDUINO
// -----------------------------------------------------------------------
/// Called by Unity to print a single character (see unity_config.h)
// -----------------------------------------------------------------------
int arduino_putchar(int a)
{
  Serial.print((char)a);
  return 0;
}

// -----------------------------------------------------------------------
/// Called by Unity to flush the output
// -----------------------------------------------------------------------
void arduino_flush(void)
{
  Serial.flush();
}
#endif // ARDUINO

#ifdef __cplusplus
}
#endif
