/**
 * @file BlinkSample2.cpp
 * @author Ralf Sondershaus
 *
 * @brief Code sample for a blinking LED with RTE support and gamma correction.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * See <https://www.gnu.org/licenses/>.
 */

#include <Std_Types.h>
#include <Rte/Rte.h>
#include <Arduino.h>

// --------------------------------------------------------------------------------------------
/// Intensity [0 ... 255] to Arduino PWM [0 ... 255]
// --------------------------------------------------------------------------------------------
static const uint8 aunIntensity2Pwm[256u] =
{
  /*          0    1    2    3    4    5    6    7    8    9  */
  /*   0 */   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
  /*  10 */   1,   1,   1,   1,   1,   1,   1,   1,   1,   2,
  /*  20 */   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
  /*  30 */   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
  /*  40 */   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
  /*  50 */   3,   3,   3,   3,   3,   3,   3,   3,   4,   4,
  /*  60 */   4,   4,   4,   4,   4,   4,   4,   4,   4,   4,
  /*  70 */   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,
  /*  80 */   6,   6,   6,   6,   6,   6,   6,   7,   7,   7,
  /*  90 */   7,   7,   7,   8,   8,   8,   8,   8,   8,   9,
  /* 100 */   9,   9,   9,   9,  10,  10,  10,  10,  10,  11,
  /* 110 */  11,  11,  11,  12,  12,  12,  12,  13,  13,  13,
  /* 120 */  14,  14,  14,  14,  15,  15,  15,  16,  16,  16,
  /* 130 */  17,  17,  18,  18,  18,  19,  19,  20,  20,  21,
  /* 140 */  21,  21,  22,  22,  23,  23,  24,  24,  25,  25,
  /* 150 */  26,  27,  27,  28,  28,  29,  30,  30,  31,  32,
  /* 160 */  32,  33,  34,  35,  35,  36,  37,  38,  39,  39,
  /* 170 */  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
  /* 180 */  50,  51,  52,  53,  55,  56,  57,  58,  59,  61,
  /* 190 */  62,  63,  65,  66,  68,  69,  71,  72,  74,  76,
  /* 200 */  77,  79,  81,  82,  84,  86,  88,  90,  92,  94,
  /* 210 */  96,  98, 100, 102, 105, 107, 109, 112, 114, 117,
  /* 220 */ 119, 122, 124, 127, 130, 133, 136, 139, 142, 145,
  /* 230 */ 148, 151, 155, 158, 162, 165, 169, 172, 176, 180,
  /* 240 */ 184, 188, 192, 196, 201, 205, 210, 214, 219, 224,
  /* 250 */ 229, 234, 239, 244, 250, 255
};

// --------------------------------------------------------------------------------------------
/// Return Arduino PWM [0 ... 255] for the given intensity [0 ... 255].
// --------------------------------------------------------------------------------------------
unsigned int int2Pwm(unsigned int unInt)
{
  return aunIntensity2Pwm[unInt];
}

// --------------------------------------------------------------------------------------------
/// Blink the LED on pin 7 (LED on board).
// --------------------------------------------------------------------------------------------
class Blinker
{
protected:
  const int nLedPin = 7;
  unsigned int unIntensity;   ///< [0...255] intensity value
  bool bUp;                   ///< upwards counting (true) or downwards counting (false)

public:
  /// default constructor
  Blinker() : unIntensity(0), bUp(true)
  {
  }

  /// Initialization
  virtual void init(void)
  {
    // Most Arduinos have an on-board LED you can control. On Mega, it is attached to digital pin 13.
    pinMode(nLedPin, OUTPUT);
  }

  /// Main execution function
  virtual void run(void)
  {
    analogWrite(nLedPin, int2Pwm(unIntensity));

    if (bUp)
    {
      if (unIntensity >= 255u)
      {
        bUp = false;
      }
    }
    else
    {
      if (unIntensity == 0)
      {
        bUp = true;
      }
    }

    if (bUp)
    {
      if (unIntensity < 255u)
      {
        unIntensity++;
      }
    }
    else
    {
      if (unIntensity > 0u)
      {
        unIntensity--;
      }
    }
  }
};

// --------------------------------------------------------------------------------------------
/// Variables
// --------------------------------------------------------------------------------------------
/// This class manages a blinking LED
Blinker theBlinker;

// --------------------------------------------------------------------------------------------
/// RTE configuration
// --------------------------------------------------------------------------------------------

/// The runables of instance theBlinker
Rte::TRunable<Blinker> rBlinker_Init(theBlinker, &Blinker::init);
Rte::TRunable<Blinker> rBlinker_Cyclic(theBlinker, &Blinker::run);

/// One init runable and one cyclic runable
static constexpr int NCR = 1;
static constexpr int NIR = 1;

/// The arrays to configure the RTE
static const Rte::RTE<NIR, NCR>::tCyclicCfgArray rcb_cfg_array = {
  {
    { 0U, 10000U, &rBlinker_Cyclic }
  }
};
static const Rte::RTE<NIR, NCR>::tInitArray init_array = { &rBlinker_Init };

/// The RTE
Rte::RTE<1, 1> rte(init_array, rcb_cfg_array);

// --------------------------------------------------------------------------------------------
/// Arduino's setup() function
// --------------------------------------------------------------------------------------------
void setup()
{
  rte.start();
}

// --------------------------------------------------------------------------------------------
/// Arduino's loop() function
// --------------------------------------------------------------------------------------------
void loop()
{
  rte.exec1();
}
