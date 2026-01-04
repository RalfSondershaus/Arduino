/**
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SRC_PRJ_UNITTEST_GEN_RTE_UT_RTE_B_H_
#define SRC_PRJ_UNITTEST_GEN_RTE_UT_RTE_B_H_

#include <Std_Types.h>

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

#endif  // SRC_PRJ_UNITTEST_GEN_RTE_UT_RTE_B_H_