/**
  * @file unity_cfg.h
  * @author Ralf Sondershaus
  *
  * @brief Defines macros to configure Unity for Arduino
  * 
  * @copyright Copyright 2024 Ralf Sondershaus
  *
 * SPDX-License-Identifier: Apache-2.0
  */

#ifndef UNITY_CFG_H
#define UNITY_CFG_H

// defined in unity_main.cpp
int arduino_putchar(int a);
void arduino_flush(void);

#define UNITY_OUTPUT_CHAR(a)   (void)arduino_putchar(a)
#define UNITY_OUTPUT_FLUSH()   arduino_flush()

#endif // UNITY_CFG_H
