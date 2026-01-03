#pragma once

/**
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Std_Types.h>
#include <iostream>

class A
{
public:
  uint32 ulCallsCyc;
  uint32 ulCallsInit;
  A() : ulCallsCyc{ 0 }, ulCallsInit{ 0 }
  {}
  virtual ~A() {}
  void init(void) { ulCallsInit++; }
  void func(void) { ulCallsCyc++; }
};
