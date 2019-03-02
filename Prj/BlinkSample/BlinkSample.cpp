#include <Arduino.h>
#include "Timer.h"
#include "Scheduler.h"

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
        Serial.println(0u - 65300u, BIN);
    }
};

RunableBlink  rBlink;
RunableSerial rSerial;

Scheduler SchM;

void setup()
{
    Serial.begin(9600);
    SchM.add(  0u,  500u, &rBlink);
    SchM.add(100u, 2000u, &rSerial);
    SchM.init();
}

void loop()
{
    SchM.schedule();
}
