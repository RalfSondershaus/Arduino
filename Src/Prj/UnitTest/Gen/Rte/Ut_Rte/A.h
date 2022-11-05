#pragma once

#include <Std_Types.h>
#include <iostream>

class A
{
public:
  uint32 ulCalls;
  A() : ulCalls{ 0 } {}
  virtual ~A() {}
  void func(void) { ulCalls++; }
};
