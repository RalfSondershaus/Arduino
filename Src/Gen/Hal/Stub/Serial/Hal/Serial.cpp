/**
 * @file Hal/Stub/Serial/Hal/Serial.cpp
 *
 * @brief 
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Std_Types.h>
#include <Hal/Serial.h>

namespace hal
{
  namespace serial 
  {
    namespace stubs
    {
        unsigned long baudrate = 0;
        unsigned int available = 0;
        int read = 0;
    }
  }
} // namespace com
