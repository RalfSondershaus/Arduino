/**
 * @file FreeRTOSDemo.cpp
 *
 * @author Ralf Sondershaus
 *
 * Demo for FreeRTOS on Arduino
 */

#include <Arduino.h>

/* FreeRTOS include files. */
#include "FreeRTOS.h"
#include "task.h"

#define mainLED_BLINK_PRIORITY    ( tskIDLE_PRIORITY + 2 )

/// [ms] 
#define BLINK_LED_PERIOD  500
/// The LED port
static const int nLedPin = 13;

// --------------------------------------------------------------------------------------------
/// Toggles nLedPin
// --------------------------------------------------------------------------------------------
static void toggleLED(void)
{
  static bool bON = false;
  if (bON)
  {
    digitalWrite(nLedPin, HIGH);
    bON = false;
  }
  else
  {
    digitalWrite(nLedPin, LOW);
    bON = true;
  }
}

extern "C"
{
  // --------------------------------------------------------------------------------------------
  /// Hook function to handle a stack overflow
  // --------------------------------------------------------------------------------------------
  void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
  {
    /* When stack overflow happens, trap instead of attempting to recover.
    Read input arguments to learn about the offending task. */
    for (;; )
    {
      /* Doesn't do anything yet. */
    }
  }

  // --------------------------------------------------------------------------------------------
  /// Hook function for idle state
  // --------------------------------------------------------------------------------------------
  void vApplicationIdleHook(void)
  {
    /* Doesn't do anything yet. */
  }

  // --------------------------------------------------------------------------------------------
  /// A task
  // --------------------------------------------------------------------------------------------
  static void vBlinkOnboardUserLED(void *pvParameters)
  {
    /* The parameters are not used. */
    (void)pvParameters;

    /* Cycle forever, blink onboard user LED at a certain frequency. */
    for (;;)
    {
      toggleLED();

      vTaskDelay(pdMS_TO_TICKS(BLINK_LED_PERIOD));
    }

  }

} // extern "C"

// --------------------------------------------------------------------------------------------
/// Arduino's setup() function
// --------------------------------------------------------------------------------------------
void setup()
{
  pinMode(nLedPin, OUTPUT);

  xTaskCreate(vBlinkOnboardUserLED, "Check", configMINIMAL_STACK_SIZE, NULL, mainLED_BLINK_PRIORITY, NULL);
}

// --------------------------------------------------------------------------------------------
/// Arduino's loop() function
// --------------------------------------------------------------------------------------------
void loop()
{
  vTaskStartScheduler();
}
