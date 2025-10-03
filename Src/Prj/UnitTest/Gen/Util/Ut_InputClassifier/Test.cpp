/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/InputClassifier.h
 */

#include <Util/Classifier.h>
#include <Hal/Timer.h>
#include <unity_adapt.h>

// ------------------------------------------------------------------------------
/// Test a configuration with 1 classifier with 5 classes.
/// Use mid values, and min and max values per class.
// ------------------------------------------------------------------------------
TEST(Ut_InputClassifier, Test_1_Classifier_5_Classes)
{
  constexpr int kNrClassifiers = 1;
  constexpr int kNrClasses = 5;

  using classifier_array_type = util::classifier_array<kNrClassifiers, kNrClasses>;
  using classifier_type = classifier_array_type::classifier_type;
  using time_type       = classifier_array_type::classifier_type::time_type;
  using classifier_cal_type = classifier_array_type::classifier_cal_type;
  using classifier_array_cal_type = classifier_array_type::classifier_array_cal_type;
  
  constexpr int kInvalidIdx = classifier_array_type::kInvalidIndex;

  classifier_array_type classifier;
  const int pin = 13;
  const time_type debounce = 10U;// [10 ms] debounce

  typedef struct
  {
    time_type ms; // [ms] current time
    int nAdc;     // current AD value for pin
    uint8 ucCls;  // expected class
  } step_type;

#define V2P(v) classifier_type::convert_input(v)
  const classifier_cal_type cfg_limits = 
  {
      debounce
    , { V2P(  0), V2P(104), V2P(204), V2P(304), V2P(404) } // Lower limit for class x [0, 255]
    , { V2P(100), V2P(200), V2P(300), V2P(400), V2P(1023) } // Upper limit for class x [0, 255]
  };
#undef V2P
  const classifier_array_cal_type cfg =
  {
      pin, &cfg_limits
  };

  const step_type aSteps[] =
  {
    /* mid values */
      { 0 * 50,  50, kInvalidIdx }
    , { 1 * 50,  50, kInvalidIdx }
    , { 2 * 50,  50, 0 }
    , { 3 * 50, 150, kInvalidIdx }
    , { 4 * 50, 150, kInvalidIdx }
    , { 5 * 50, 150, 1 }
    , { 6 * 50, 250, kInvalidIdx }
    , { 7 * 50, 250, kInvalidIdx }
    , { 8 * 50, 250, 2 }
    , { 9 * 50, 350, kInvalidIdx }
    , {10 * 50, 350, kInvalidIdx }
    , {11 * 50, 350, 3 }
    , {12 * 50, 700, kInvalidIdx }
    , {13 * 50, 700, kInvalidIdx }
    , {14 * 50, 700, 4 }
    /* min values */
    , {16 * 50,   0, kInvalidIdx }
    , {17 * 50,   0, kInvalidIdx }
    , {18 * 50,   0, 0 }
    , {19 * 50, 104, kInvalidIdx }
    , {20 * 50, 104, kInvalidIdx }
    , {21 * 50, 104, 1 }
    , {22 * 50, 204, kInvalidIdx }
    , {23 * 50, 204, kInvalidIdx }
    , {24 * 50, 204, 2 }
    , {25 * 50, 304, kInvalidIdx }
    , {26 * 50, 304, kInvalidIdx }
    , {27 * 50, 304, 3 }
    , {28 * 50, 404, kInvalidIdx }
    , {29 * 50, 404, kInvalidIdx }
    , {30 * 50, 404, 4 }
    /* max values */
    , {31 * 50, 100, kInvalidIdx }
    , {32 * 50, 100, kInvalidIdx }
    , {33 * 50, 100, 0 }
    , {34 * 50, 200, kInvalidIdx }
    , {35 * 50, 200, kInvalidIdx }
    , {36 * 50, 200, 1 }
    , {37 * 50, 300, kInvalidIdx }
    , {38 * 50, 300, kInvalidIdx }
    , {39 * 50, 300, 2 }
    , {40 * 50, 400, kInvalidIdx }
    , {41 * 50, 400, kInvalidIdx }
    , {42 * 50, 400, 3 }
    , {43 * 50,1023, kInvalidIdx }
    , {44 * 50,1023, kInvalidIdx }
    , {45 * 50,1023, 4 }
  };
  size_t nStep;
  
  classifier.set_config(&cfg);

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    hal::stubs::analogRead[pin] = aSteps[nStep].nAdc;
    hal::stubs::millis = aSteps[nStep].ms;
    classifier.run(); 
    EXPECT_EQ(classifier.classified_value(0), aSteps[nStep].ucCls);
  }
}

// ------------------------------------------------------------------------------
///
// ------------------------------------------------------------------------------
TEST(Ut_InputClassifier, Test_3_Classifiers_5_Classes)
{
  constexpr int kNrClassifiers = 3;
  constexpr int kNrClasses = 5;

  using classifier_array_type = util::classifier_array<kNrClassifiers, kNrClasses>;
  using classifier_type = classifier_array_type::classifier_type;
  using time_type       = classifier_array_type::classifier_type::time_type;
  using classifier_cal_type = classifier_array_type::classifier_cal_type;
  using classifier_array_cal_type = classifier_array_type::classifier_array_cal_type;
  
  constexpr int kInvalidIdx = classifier_array_type::kInvalidIndex;

  classifier_array_type classifier;
  const uint8 pins[] = { 13U, 14U, 15U };
  constexpr uint8 debounce = 10;// [10 ms] debounce

  typedef struct
  {
    time_type ms; // [ms] current time
    int anAdc[3];     // current AD value for pin
    uint8 aucCls[3];  // expected class
  } step_type;

#define V2P(v) classifier_type::convert_input(v)
  const classifier_cal_type cfg_limits1 = 
  {
      debounce
    , { /* struct util::array */ { V2P(0), V2P(101), V2P(201), V2P(301), V2P(401) } } // Lower limit for class x [0, 255]
    , { /* struct util::array */ { V2P(100), V2P(200), V2P(300), V2P(400), V2P(500) } } // Upper limit for class x [0, 255]
  };
  const classifier_cal_type cfg_limits2 = 
  {
      debounce
    , { { V2P(101), V2P(201), V2P(301), V2P(401), V2P(501) } } // Lower limit for class x [0, 255]
    , { { V2P(200), V2P(300), V2P(400), V2P(500), V2P(600) } } // Upper limit for class x [0, 255]
  };
  const classifier_cal_type cfg_limits3 = 
  {
      debounce
    , { { V2P(201), V2P(301), V2P(401), V2P(501), V2P(601) } } // Lower limit for class x [0, 255]
    , { { V2P(300), V2P(400), V2P(500), V2P(600), V2P(700) } } // Upper limit for class x [0, 255]
  };
  const classifier_array_cal_type cfg =
  { /* struct input_classifier_cfg */
    { /* struct util::array */
      { /* array */
        { pins[0], &cfg_limits1 },
        { pins[1], &cfg_limits2 },
        { pins[2], &cfg_limits3 },
      }
    }
  };
#undef V2P

  const step_type aSteps[] =
  {
      {   0, {   0,   0,   0 }, { kInvalidIdx, kInvalidIdx, kInvalidIdx } }
    , {  50, {  10,  10,  10 }, { kInvalidIdx, kInvalidIdx, kInvalidIdx } }
    , { 100, {   5,   5,   5 }, { 0          , kInvalidIdx, kInvalidIdx } }
    , { 150, { 150, 150, 150 }, { kInvalidIdx, kInvalidIdx, kInvalidIdx } }
    , { 200, { 140, 140, 140 }, { kInvalidIdx, kInvalidIdx, kInvalidIdx } }
    , { 250, { 140, 140, 140 }, { 1          , 0          , kInvalidIdx } }
    , { 300, { 240, 240, 240 }, { kInvalidIdx, kInvalidIdx, kInvalidIdx } }
    , { 350, { 230, 230, 230 }, { kInvalidIdx, kInvalidIdx, kInvalidIdx } }
    , { 400, { 230, 230, 230 }, { 2          , 1          , 0           } }
  };
  size_t nStep;

  classifier.set_config(&cfg);

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    for (int i = 0; i < 3; i++)
    {
      hal::stubs::analogRead[pins[i]] = aSteps[nStep].anAdc[i];
      hal::stubs::millis = aSteps[nStep].ms;
      classifier.run();
      EXPECT_EQ(classifier.classified_value(i), aSteps[nStep].aucCls[i]);
    }
  }
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_setup(void)
{
}

bool test_loop(void)
{
  UNITY_BEGIN();

  RUN_TEST(Test_1_Classifier_5_Classes);
  RUN_TEST(Test_3_Classifiers_5_Classes);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
