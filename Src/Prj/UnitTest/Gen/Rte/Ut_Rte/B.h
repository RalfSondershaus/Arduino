#pragma once

#include <Std_Types.h>
#include <iostream>

/// Another class with a runable function
class B
{
public:
  uint32 ulCallsCyc;
  uint32 ulCallsEvent;
  B() : ulCallsCyc{ 0 }, ulCallsEvent{ 0 } {}
  virtual ~B() {}
  void func(void) { ulCallsCyc++; }
  void evnt(void) { ulCallsEvent++; }
};
