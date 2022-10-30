#include "stdafx.h"
#include "CppUnitTest.h"
#include <PeriodSM.h>
#include <DccInterpreter.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dcc;

namespace DccDecoderMicrosoftTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
      static const unsigned int aSteps[][3] =
      {
          { 64u, HalfBitStateMachine::STATE_SHORT_INIT_1, 1u }
        , { 64u, HalfBitStateMachine::STATE_SHORT_INIT_2, 1u }
        , { 64u, HalfBitStateMachine::STATE_SHORT_INIT_1, 2u }
        , { 64u, HalfBitStateMachine::STATE_SHORT_INIT_2, 2u }
        , { 64u, HalfBitStateMachine::STATE_SHORT_INIT_1, 3u }
        , { 64u, HalfBitStateMachine::STATE_SHORT_INIT_2, 3u }
      };
      DccInterpreter DccInterp;
      HalfBitStateMachine PSM(DccInterp);
      int i;
      const int nSize = sizeof(aSteps) / (3 * sizeof(unsigned int));
      for (i = 0; i < nSize; i++)
      {
        PSM.execute(aSteps[i][0]);
        Assert::AreEqual(aSteps[i][2], PSM.getNrCalls(aSteps[i][1]));
      }
    }
    TEST_METHOD(TestMethod2)
    {
      static const unsigned int aSteps[][3] =
      {
          {  64u, HalfBitStateMachine::STATE_SHORT_INIT_1, 1u }
        , {  64u, HalfBitStateMachine::STATE_SHORT_INIT_2, 1u }
        , { 100u, HalfBitStateMachine::STATE_LONG_1      , 1u }
        , { 100u, HalfBitStateMachine::STATE_LONG_2      , 1u }
      };
      DccInterpreter DccInterp;
      HalfBitStateMachine PSM(DccInterp);
      int i;
      const int nSize = sizeof(aSteps) / (3 * sizeof(unsigned int));
      for (i = 0; i < nSize; i++)
      {
        PSM.execute(aSteps[i][0]);
        Assert::AreEqual(aSteps[i][2], PSM.getNrCalls(aSteps[i][1]));
      }
    }
    TEST_METHOD(TestMethod3)
    {
      static const unsigned int aSteps[][3] =
      {
          {  64u, HalfBitStateMachine::STATE_SHORT_INIT_1, 1u }
        , {  64u, HalfBitStateMachine::STATE_SHORT_INIT_2, 1u }
        , { 100u, HalfBitStateMachine::STATE_LONG_1, 1u }
        , { 100u, HalfBitStateMachine::STATE_LONG_2, 1u }
        , {  64u, HalfBitStateMachine::STATE_SHORT_1, 1u }
        , {  64u, HalfBitStateMachine::STATE_SHORT_2, 1u }
        , {  64u, HalfBitStateMachine::STATE_SHORT_1, 2u }
        , {  64u, HalfBitStateMachine::STATE_SHORT_2, 2u }
      };
      DccInterpreter DccInterp;
      HalfBitStateMachine PSM(DccInterp);
      int i;
      const int nSize = sizeof(aSteps) / (3 * sizeof(unsigned int));
      for (i = 0; i < nSize; i++)
      {
        PSM.execute(aSteps[i][0]);
        Assert::AreEqual(aSteps[i][2], PSM.getNrCalls(aSteps[i][1]));
      }
    }
    TEST_METHOD(TestMethod4)
    {
      static const unsigned int aSteps[] =
      {
             1,1,1,1,1, 1,1,1,1,1, 1
        , 0, 1,1,0,0,0, 0,0,1
        , 0, 1,0,0,0,0, 1,0,0
        , 0, 1,1,1,0,0, 0,0,0
        , 1
      };
      DccInterpreter DccInterp;
      int i;
      const int nSize = sizeof(aSteps) / sizeof(unsigned int);
      for (i = 0; i < nSize; i++)
      {
        if (aSteps[i] == 1)
        {
          DccInterp.one();
        }
        else
        {
          DccInterp.zero();
        }
      }
      Assert::AreEqual(1u, DccInterp.refPacketContainer().size());
    }
    TEST_METHOD(TestMethod5)
    {
      static const unsigned int aSteps[] =
      {
             1,1,1,1,1, 1,1,1,1,1, 1
        , 0, 1,1,0,0,0, 0,0,1
        , 0, 1,0,0,0,0, 1,0,0
        , 0, 1,1,1,0,0, 0,0,0
        , 1
        ,    1,1,1,1,1, 1,1,1,1,1, 1
        , 0, 1,1,0,0,0, 0,0,1
        , 0, 1,0,0,0,0, 1,0,0
        , 0, 1,1,1,0,0, 0,0,0
        , 1
      };
      DccInterpreter DccInterp;
      int i;
      const int nSize = sizeof(aSteps) / sizeof(unsigned int);
      for (i = 0; i < nSize; i++)
      {
        if (aSteps[i] == 1)
        {
          DccInterp.one();
        }
        else
        {
          DccInterp.zero();
        }
      }
      Assert::AreEqual(1u, DccInterp.refPacketContainer().size());
    }
    TEST_METHOD(TestMethod6)
    {
      static const unsigned int aSteps[] =
      {
             1,1,1,1,1, 1,1,1,1,1, 1
        , 0, 1,1,0,0,0, 0,0,1
        , 0, 1,0,0,0,0, 1,0,0
        , 0, 1,1,1,0,0, 0,0,0
        , 1
        ,    1,1,1,1,1, 1,1,1,1,1, 1
        , 0, 1,0,0,0,0, 0,0,1
        , 0, 1,0,0,0,0, 1,0,0
        , 0, 1,1,1,0,0, 0,0,0
        , 1
      };
      DccInterpreter DccInterp;
      int i;
      const int nSize = sizeof(aSteps) / sizeof(unsigned int);
      for (i = 0; i < nSize; i++)
      {
        if (aSteps[i] == 1)
        {
          DccInterp.one();
        }
        else
        {
          DccInterp.zero();
        }
      }
      Assert::AreEqual(2u, DccInterp.refPacketContainer().size());
    }

	};
}