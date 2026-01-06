/**
 * @file Signal/Sig.cpp
 *
 * @brief Defines loop and setup.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Rte/Rte.h>

// --------------------------------------------------------------------------------------------
/// Arduino's setup() function
// --------------------------------------------------------------------------------------------
void setup()
{
  hal::serial::begin(115200);
  
  rte::start();
}

// --------------------------------------------------------------------------------------------
/// Arduino's loop() function
// --------------------------------------------------------------------------------------------
void loop()
{
  rte::exec();
}
