/// FireFlicker.cpp
///
/// Implementation of flickering fire
///
/// Implements
/// - setup
/// - loop

#include <Arduino.h>

// ----------------------------------------------------------
/// Define settings for Fire Flicker
// ----------------------------------------------------------
template<int NrLamps>
struct FireFlickerSetting
{
  /// (Constant) array of Arduino output ports
  const int anPinOutput[NrLamps];
};

// ----------------------------------------------------------
/// Class to flicker a fire that consists of NrLamps lamps.
// ----------------------------------------------------------
template<int NrLamps>
class FireFlicker
{
protected:
  /// Time stamp when to switch the state of the output pin
  unsigned long aulTime[NrLamps];
  /// State of output pin: either LOW or HIGH
  int anValue[NrLamps];
  /// Reference to our settings (defined outside and handed over by constructor)
  FireFlickerSetting<NrLamps>& setting;
  /// Constructor that shall not be used
  FireFlicker() {}
public:
  /// Constructor with settings
  FireFlicker(FireFlickerSetting<NrLamps>& settng) : setting(settng) {}
  /// Destructor has nothing to do
  ~FireFlicker() {}
  /// Init function, to be called from Arduino's setup function
  void init()
  {
    int i;

    randomSeed(analogRead(0));

    for (i = 0; i < NrLamps; i++)
    {
      pinMode(setting.anPinOutput[i], OUTPUT);
      anValue[i] = HIGH;
      aulTime[i] = millis() + random(500);
    }
  }
  /// Loop function, to be called from Arduino's loop function
  void loop(void)
  {
    int i;
    unsigned long ulTimeCurrent;

    ulTimeCurrent = millis();

    for (i = 0; i < NrLamps; i++)
    {
      if (ulTimeCurrent > aulTime[i])
      {
        if (anValue[i] == HIGH)
        {
          anValue[i] = LOW;
          aulTime[i] = millis() + random(500);
        }
        else
        {
          anValue[i] = HIGH;
          aulTime[i] = millis() + random(2000);
        }
        digitalWrite(setting.anPinOutput[i], anValue[i]);
      }
    }
  }
};

// ----------------------------------------------------------
/// variables and constants
// ----------------------------------------------------------
FireFlickerSetting<3> MyFireFlickerSetting = { { 2, 3, 4 } };
FireFlicker<3> MyFireFlicker(MyFireFlickerSetting);

// ----------------------------------------------------------
/// setup is called once at start up
// ----------------------------------------------------------
void setup()
{
  MyFireFlicker.init();
}

// ----------------------------------------------------------
/// loop
// ----------------------------------------------------------
void loop()
{
  MyFireFlicker.loop();
}