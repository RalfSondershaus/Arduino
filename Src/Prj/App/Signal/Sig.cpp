/**
 * @file Signal/Sig.cpp
 *
 * @brief Defines loop and setup.
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

#include <Rte/Rte.h>

// --------------------------------------------------------------------------------------------
/// Arduino's setup() function
// --------------------------------------------------------------------------------------------
void setup()
{
  rte::start();
}

// --------------------------------------------------------------------------------------------
/// Arduino's loop() function
// --------------------------------------------------------------------------------------------
void loop()
{
  rte::exec();
}
