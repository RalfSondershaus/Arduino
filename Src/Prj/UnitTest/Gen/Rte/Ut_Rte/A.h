/**
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SRC_PRJ_UNITTEST_GEN_RTE_UT_RTE_A_H_
#define SRC_PRJ_UNITTEST_GEN_RTE_UT_RTE_A_H_

#include <Std_Types.h>

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

#endif  // SRC_PRJ_UNITTEST_GEN_RTE_UT_RTE_A_H_