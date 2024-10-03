/**
  * @file unity_cfg.h
  * @author Ralf Sondershaus
  *
  * @brief Defines macros to configure Unity for Arduino
  * 
  * @copyright Copyright 2024 Ralf Sondershaus
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
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */

#ifndef UNITY_CFG_H
#define UNITY_CFG_H

// defined in unity_main.cpp
int arduino_putchar(int a);
void arduino_flush(void);

#define UNITY_OUTPUT_CHAR(a)   (void)arduino_putchar(a)
#define UNITY_OUTPUT_FLUSH()   arduino_flush()

#endif // UNITY_CFG_H
