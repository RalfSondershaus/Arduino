/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/InputClassifier.h
 */

#include <Util/Classifier.h>
#include <unity_adapt.h>

static unsigned long Arduino_Stub_MicrosReturnValue;
static unsigned long Arduino_Stub_MillisReturnValue;
static int Arduino_Stub_AnalogReadReturnValue[255];

/// Return current time [us]
unsigned long micros() { return Arduino_Stub_MicrosReturnValue; }
/// Return current time [ms]
unsigned long millis() { return Arduino_Stub_MillisReturnValue; }
/// read analog input
int analogRead(int pin) { return Arduino_Stub_AnalogReadReturnValue[pin]; }

// ------------------------------------------------------------------------------
/// Test a configuration with 1 classifier with 5 classes.
/// Use mid values, and min and max values per class.
// ------------------------------------------------------------------------------
TEST(Ut_InputClassifier, Test_1_Classifier_5_Classes)
{
  constexpr int kNrClassifiers = 1;
  constexpr int kNrClasses = 5;

  typedef util::input_classifier<kNrClassifiers, kNrClasses> input_classifier_type;
  using cal_type        = input_classifier_type::input_classifier_cal_type;
  using classifier_type = input_classifier_type::classifier_type;
  using time_type       = input_classifier_type::classifier_type::time_type;

  input_classifier_type classifier;
  const int pin = 13;
  const time_type debounce = 10U;// [10 ms] debounce

  typedef struct
  {
    time_type ms; // [ms] current time
    int nAdc;     // current AD value for pin
    uint8 ucCls;  // expected class
  } step_type;

#define V2P(v) classifier_type::convertInput(v)
  const cal_type cfg =
  {
      pin
    , debounce
    , V2P(  0), V2P(104), V2P(204), V2P(304), V2P(404)  // Lower limit for class x [0, 255]
    , V2P(100), V2P(200), V2P(300), V2P(400), V2P(1023) // Upper limit for class x [0, 255]
  };
#undef V2P

  const step_type aSteps[] =
  {
    /* mid values */
      { 0 * 50,  50, input_classifier_type::kInvalidIndex }
    , { 1 * 50,  50, input_classifier_type::kInvalidIndex }
    , { 2 * 50,  50, 0 }
    , { 3 * 50, 150, input_classifier_type::kInvalidIndex }
    , { 4 * 50, 150, input_classifier_type::kInvalidIndex }
    , { 5 * 50, 150, 1 }
    , { 6 * 50, 250, input_classifier_type::kInvalidIndex }
    , { 7 * 50, 250, input_classifier_type::kInvalidIndex }
    , { 8 * 50, 250, 2 }
    , { 9 * 50, 350, input_classifier_type::kInvalidIndex }
    , {10 * 50, 350, input_classifier_type::kInvalidIndex }
    , {11 * 50, 350, 3 }
    , {12 * 50, 700, input_classifier_type::kInvalidIndex }
    , {13 * 50, 700, input_classifier_type::kInvalidIndex }
    , {14 * 50, 700, 4 }
    /* min values */
    , {16 * 50,   0, input_classifier_type::kInvalidIndex }
    , {17 * 50,   0, input_classifier_type::kInvalidIndex }
    , {18 * 50,   0, 0 }
    , {19 * 50, 104, input_classifier_type::kInvalidIndex }
    , {20 * 50, 104, input_classifier_type::kInvalidIndex }
    , {21 * 50, 104, 1 }
    , {22 * 50, 204, input_classifier_type::kInvalidIndex }
    , {23 * 50, 204, input_classifier_type::kInvalidIndex }
    , {24 * 50, 204, 2 }
    , {25 * 50, 304, input_classifier_type::kInvalidIndex }
    , {26 * 50, 304, input_classifier_type::kInvalidIndex }
    , {27 * 50, 304, 3 }
    , {28 * 50, 404, input_classifier_type::kInvalidIndex }
    , {29 * 50, 404, input_classifier_type::kInvalidIndex }
    , {30 * 50, 404, 4 }
    /* max values */
    , {31 * 50, 100, input_classifier_type::kInvalidIndex }
    , {32 * 50, 100, input_classifier_type::kInvalidIndex }
    , {33 * 50, 100, 0 }
    , {34 * 50, 200, input_classifier_type::kInvalidIndex }
    , {35 * 50, 200, input_classifier_type::kInvalidIndex }
    , {36 * 50, 200, 1 }
    , {37 * 50, 300, input_classifier_type::kInvalidIndex }
    , {38 * 50, 300, input_classifier_type::kInvalidIndex }
    , {39 * 50, 300, 2 }
    , {40 * 50, 400, input_classifier_type::kInvalidIndex }
    , {41 * 50, 400, input_classifier_type::kInvalidIndex }
    , {42 * 50, 400, 3 }
    , {43 * 50,1023, input_classifier_type::kInvalidIndex }
    , {44 * 50,1023, input_classifier_type::kInvalidIndex }
    , {45 * 50,1023, 4 }
  };
  int nStep;
  
  classifier.set_config(&cfg);

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    Arduino_Stub_AnalogReadReturnValue[pin] = aSteps[nStep].nAdc;
    Arduino_Stub_MillisReturnValue = aSteps[nStep].ms;
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

  typedef util::input_classifier<kNrClassifiers, kNrClasses> input_classifier_type;
  using cal_type        = input_classifier_type::input_classifier_cal_type;
  using classifier_type = input_classifier_type::classifier_type;
  using time_type       = input_classifier_type::classifier_type::time_type;

  input_classifier_type classifier;
  const uint8 pins[] = { 13U, 14U, 15U };
  const uint8 debounce[] = { 10U, 10U, 10U };// [10 ms] debounce

  typedef struct
  {
    time_type ms; // [ms] current time
    int anAdc[3];     // current AD value for pin
    uint8 aucCls[3];  // expected class
  } step_type;

#define V2P(v) classifier_type::convertInput(v)
  const cal_type cfg =
  { /* struct input_classifier_cfg */
    { /* struct util::array */
      { /* array */
        { pins[0]
          , { /* struct classifier_limits */
            debounce[0]
            , { /* struct util::array */ { V2P(0), V2P(101), V2P(201), V2P(301), V2P(401) } } // Lower limit for class x [0, 1023]
            , { /* struct util::array */ { V2P(100), V2P(200), V2P(300), V2P(400), V2P(500) } } // Upper limit for class x [0, 1023]
            }
        }
      , { pins[1]
          , {
            debounce[1]
            , { { V2P(101), V2P(201), V2P(301), V2P(401), V2P(501) } } // Lower limit for class x [0, 1023]
            , { { V2P(200), V2P(300), V2P(400), V2P(500), V2P(600) } } // Upper limit for class x [0, 1023]
            }
        }
      , { pins[2]
          , {  debounce[2]
          , { { V2P(201), V2P(301), V2P(401), V2P(501), V2P(601) } } // Lower limit for class x [0, 1023]
          , { { V2P(300), V2P(400), V2P(500), V2P(600), V2P(700) } } // Upper limit for class x [0, 1023]
           }
        }
      }
    }
  };
#undef V2P

  const step_type aSteps[] =
  {
      {   0, {   0,   0,   0 }, { input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex } }
    , {  50, {  10,  10,  10 }, { input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex } }
    , { 100, {   5,   5,   5 }, { 0                                   , input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex } }
    , { 150, { 150, 150, 150 }, { input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex } }
    , { 200, { 140, 140, 140 }, { input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex } }
    , { 250, { 140, 140, 140 }, { 1                                   , 0                                   , input_classifier_type::kInvalidIndex } }
    , { 300, { 240, 240, 240 }, { input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex } }
    , { 350, { 230, 230, 230 }, { input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex, input_classifier_type::kInvalidIndex } }
    , { 400, { 230, 230, 230 }, { 2                                   , 1                                   , 0                                    } }
  };
  int nStep;

  classifier.set_config(&cfg);

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    for (int i = 0; i < 3; i++)
    {
      Arduino_Stub_AnalogReadReturnValue[pins[i]] = aSteps[nStep].anAdc[i];
      Arduino_Stub_MillisReturnValue = aSteps[nStep].ms;
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
