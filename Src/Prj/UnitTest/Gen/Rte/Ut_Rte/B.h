#pragma once

#include <Std_Types.h>
#include <iostream>

/// Another class with a runable function
class B
{
public:
  uint32 ulCalls;
  uint32 ulCallsEvent;
  B() : ulCalls{ 0 } {}
  virtual ~B() {}
  void func(void) { ulCalls++; }
  void evnt(void) { ulCallsEvent++; }
};
