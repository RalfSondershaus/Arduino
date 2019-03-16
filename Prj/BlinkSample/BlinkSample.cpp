#include <Arduino.h>
#include <Timer/Timer.h>
#include <Scheduler/Scheduler.h>

/// Blink the LED on pin 13 (LED on board).
class RunableBlinkPWM : public Runable
{
protected:
  const int nLedPin = 13;
  unsigned int unIntensity;   ///< [0...255] intensity value
  bool bUp;                   ///< upwards counting (true) or downwards counting (false)
  static const unsigned int aunIntensity2Pwm[256];  ///< for each brightness a corresponding PWM value. Necessary for ratio-based dim ramps.

public:
  /// default constructor
  RunableBlinkPWM() : unIntensity(0), bUp(true)
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
    analogWrite(nLedPin, RunableBlinkPWM::aunIntensity2Pwm[unIntensity]);
    
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

static const unsigned int RunableBlinkPWM::aunIntensity2Pwm[256u] =
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

RunableBlinkPWM  rBlinkPWM;

Scheduler SchM;

void setup()
{
    SchM.add(  0u,   1u, &rBlinkPWM);
    SchM.init();
}

void loop()
{
    SchM.schedule();
}
