#include <Arduino.h>
#include <Timer/Timer.h>
#include <Scheduler/Scheduler.h>

/// Blink the LED on pin 13 (LED on board). Each time, run() is called,
/// the LED is toggled.
class RunableBlink : public Runable
{
protected:
    unsigned int unOutput = HIGH;
public:
    /// Initialization
    virtual void init(void)
    {
        // Most Arduinos have an on-board LED you can control. On Mega, it is attached to digital pin 13.
        pinMode(13, OUTPUT);
    }
    /// Main execution function
    virtual void run(void)
    {
        digitalWrite(13, unOutput);
        if (unOutput == HIGH)
        {
            unOutput  = LOW;
        }
        else
        {
            unOutput = HIGH;
        }
    }
};

/// Blink the LED on pin 13 (LED on board). Each time, run() is called,
/// the LED is toggled.
class RunableBlinkPWM : public Runable
{
protected:
  unsigned int unIntensity;   ///< [0...255] intensity value
  bool bUp;             ///< upwards counting (true) or downwards counting (false)
  static const unsigned int aunRatioIntensities[256u] =
  {
    /* 0	 */	  1u,
    /* 1	 */	  1u,
    /* 2	 */	  1u,
    /* 3	 */	  1u,
    /* 4	 */	  1u,
    /* 5	 */	  1u,
    /* 6	 */	  1u,
    /* 7	 */	  1u,
    /* 8	 */	  1u,
    /* 9	 */	  1u,
    /* 10	 */	  1u,
    /* 11	 */	  1u,
    /* 12	 */	  1u,
    /* 13	 */	  1u,
    /* 14	 */	  1u,
    /* 15	 */	  1u,
    /* 16	 */	  1u,
    /* 17	 */	  1u,
    /* 18	 */	  1u,
    /* 19	 */	  2u,
    /* 20	 */	  2u,
    /* 21	 */	  2u,
    /* 22	 */	  2u,
    /* 23	 */	  2u,
    /* 24	 */	  2u,
    /* 25	 */	  2u,
    /* 26	 */	  2u,
    /* 27	 */	  2u,
    /* 28	 */	  2u,
    /* 29	 */	  2u,
    /* 30	 */	  2u,
    /* 31	 */	  2u,
    /* 32	 */	  2u,
    /* 33	 */	  2u,
    /* 34	 */	  2u,
    /* 35	 */	  2u,
    /* 36	 */	  2u,
    /* 37	 */	  2u,
    /* 38	 */	  2u,
    /* 39	 */	  2u,
    /* 40	 */	  2u,
    /* 41	 */	  2u,
    /* 42	 */	  2u,
    /* 43	 */	  3u,
    /* 44	 */	  3u,
    /* 45	 */	  3u,
    /* 46	 */	  3u,
    /* 47	 */	  3u,
    /* 48	 */	  3u,
    /* 49	 */	  3u,
    /* 50	 */	  3u,
    /* 51	 */	  3u,
    /* 52	 */	  3u,
    /* 53	 */	  3u,
    /* 54	 */	  3u,
    /* 55	 */	  3u,
    /* 56	 */	  3u,
    /* 57	 */	  3u,
    /* 58	 */	  4u,
    /* 59	 */	  4u,
    /* 60	 */	  4u,
    /* 61	 */	  4u,
    /* 62	 */	  4u,
    /* 63	 */	  4u,
    /* 64	 */	  4u,
    /* 65	 */	  4u,
    /* 66	 */	  4u,
    /* 67	 */	  4u,
    /* 68	 */	  4u,
    /* 69	 */	  4u,
    /* 70	 */	  5u,
    /* 71	 */	  5u,
    /* 72	 */	  5u,
    /* 73	 */	  5u,
    /* 74	 */	  5u,
    /* 75	 */	  5u,
    /* 76	 */	  5u,
    /* 77	 */	  5u,
    /* 78	 */	  5u,
    /* 79	 */	  6u,
    /* 80	 */	  6u,
    /* 81	 */	  6u,
    /* 82	 */	  6u,
    /* 83	 */	  6u,
    /* 84	 */	  6u,
    /* 85	 */	  6u,
    /* 86	 */	  6u,
    /* 87	 */	  7u,
    /* 88	 */	  7u,
    /* 89	 */	  7u,
    /* 90	 */	  7u,
    /* 91	 */	  7u,
    /* 92	 */	  7u,
    /* 93	 */	  8u,
    /* 94	 */	  8u,
    /* 95	 */	  8u,
    /* 96	 */	  8u,
    /* 97	 */	  8u,
    /* 98	 */   8u,
    /* 99	 */   9u,
    /* 100 */		9u,
    /* 101 */		9u,
    /* 102 */		9u,
    /* 103 */		9u,
    /* 104 */		10u,
    /* 105 */		10u,
    /* 106 */		10u,
    /* 107 */		10u,
    /* 108 */		10u,
    /* 109 */		11u,
    /* 110 */		11u,
    /* 111 */		11u,
    /* 112 */		11u,
    /* 113 */		12u,
    /* 114 */		12u,
    /* 115 */		12u,
    /* 116 */		12u,
    /* 117 */		13u,
    /* 118 */		13u,
    /* 119 */		13u,
    /* 120 */		14u,
    /* 121 */		14u,
    /* 122 */		14u,
    /* 123 */		14u,
    /* 124 */		15u,
    /* 125 */		15u,
    /* 126 */		15u,
    /* 127 */		16u,
    /* 128 */		16u,
    /* 129 */		16u,
    /* 130 */		17u,
    /* 131 */		17u,
    /* 132 */		18u,
    /* 133 */		18u,
    /* 134 */		18u,
    /* 135 */		19u,
    /* 136 */		19u,
    /* 137 */		20u,
    /* 138 */		20u,
    /* 139 */		21u,
    /* 140 */		21u,
    /* 141 */		21u,
    /* 142 */		22u,
    /* 143 */		22u,
    /* 144 */		23u,
    /* 145 */		23u,
    /* 146 */		24u,
    /* 147 */		24u,
    /* 148 */		25u,
    /* 149 */		25u,
    /* 150 */		26u,
    /* 151 */		27u,
    /* 152 */		27u,
    /* 153 */		28u,
    /* 154 */		28u,
    /* 155 */		29u,
    /* 156 */		30u,
    /* 157 */		30u,
    /* 158 */		31u,
    /* 159 */		32u,
    /* 160 */		32u,
    /* 161 */		33u,
    /* 162 */		34u,
    /* 163 */		35u,
    /* 164 */		35u,
    /* 165 */		36u,
    /* 166 */		37u,
    /* 167 */		38u,
    /* 168 */		39u,
    /* 169 */		39u,
    /* 170 */		40u,
    /* 171 */		41u,
    /* 172 */		42u,
    /* 173 */		43u,
    /* 174 */		44u,
    /* 175 */		45u,
    /* 176 */		46u,
    /* 177 */		47u,
    /* 178 */		48u,
    /* 179 */		49u,
    /* 180 */		50u,
    /* 181 */		51u,
    /* 182 */		52u,
    /* 183 */		53u,
    /* 184 */		55u,
    /* 185 */		56u,
    /* 186 */		57u,
    /* 187 */		58u,
    /* 188 */		59u,
    /* 189 */		61u,
    /* 190 */		62u,
    /* 191 */		63u,
    /* 192 */		65u,
    /* 193 */		66u,
    /* 194 */		68u,
    /* 195 */		69u,
    /* 196 */		71u,
    /* 197 */		72u,
    /* 198 */		74u,
    /* 199 */		76u,
    /* 200 */		77u,
    /* 201 */		79u,
    /* 202 */		81u,
    /* 203 */		82u,
    /* 204 */		84u,
    /* 205 */		86u,
    /* 206 */		88u,
    /* 207 */		90u,
    /* 208 */		92u,
    /* 209 */		94u,
    /* 210 */		96u,
    /* 211 */		98u,
    /* 212 */		100u,
    /* 213 */		102u,
    /* 214 */		105u,
    /* 215 */		107u,
    /* 216 */		109u,
    /* 217 */		112u,
    /* 218 */		114u,
    /* 219 */		117u,
    /* 220 */		119u,
    /* 221 */		122u,
    /* 222 */		124u,
    /* 223 */		127u,
    /* 224 */		130u,
    /* 225 */		133u,
    /* 226 */		136u,
    /* 227 */		139u,
    /* 228 */		142u,
    /* 229 */		145u,
    /* 230 */		148u,
    /* 231 */		151u,
    /* 232 */		155u,
    /* 233 */		158u,
    /* 234 */		162u,
    /* 235 */		165u,
    /* 236 */		169u,
    /* 237 */		172u,
    /* 238 */		176u,
    /* 239 */		180u,
    /* 240 */		184u,
    /* 241 */		188u,
    /* 242 */		192u,
    /* 243 */		196u,
    /* 244 */		201u,
    /* 245 */		205u,
    /* 246 */		210u,
    /* 247 */		214u,
    /* 248 */		219u,
    /* 249 */		224u,
    /* 250 */		229u,
    /* 251 */		234u,
    /* 252 */		239u,
    /* 253 */		244u,
    /* 254 */		250u,
    /* 255 */		255u
  };

public:
  /// default constructor
  RunableBlinkPWM() : unIntensity(0), bUp(true)
  {
  }

  /// Initialization
  virtual void init(void)
  {
    // Most Arduinos have an on-board LED you can control. On Mega, it is attached to digital pin 13.
    pinMode(13, OUTPUT);
  }
  /// Main execution function
  virtual void run(void)
  {
    analogWrite(13, RunableBlinkPWM::aunRatioIntensities[unIntensity]);
    
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

static const unsigned int RunableBlinkPWM::aunRatioIntensities[256u];

/// Print information to serial output
class RunableSerial : public Runable
{
public:
  /// Initialization
  virtual void init(void)
  {
  }
  /// Main execution function
  virtual void run(void)
  {
    Serial.println(0u - 65535u, BIN);
  }
};

RunableBlink     rBlink;
RunableBlinkPWM  rBlinkPWM;
RunableSerial    rSerial;

Scheduler SchM;

void setup()
{
    Serial.begin(9600);
    SchM.add(  0u,   1u, &rBlinkPWM);
    //SchM.add(  0u, 1000u, &rBlink);
    //SchM.add(100u, 2000u, &rSerial);
    SchM.init();
}

void loop()
{
    SchM.schedule();
}
