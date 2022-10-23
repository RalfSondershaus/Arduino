/**
 * Class DiagnoseCommander (diagnostic commands).
 */

#include <Arduino.h>
#include <DiagnoseCommander.h>

/// Baud rate for serial commands
#define SERIAL_BAUDRATE   9600

/// Constructor
DiagnoseCommander::DiagnoseCommander() 
  : mIfcPortDiagnose(*this)
  , mIfcPortSignalCommand(*this)
  , mCmd(eIfcDiagnosticCommand::NONE)
{}

/// Handle command C (External Control)
void DiagnoseCommander::cmdC()
{

}

/// Check command and call command handle function
void DiagnoseCommander::parseCommand(int nCmd)
{
  switch (nCmd)
  {
  case 'c': cmdC();  break;
  default: break;
  }
}

/// Called cyclicly
void DiagnoseCommander::update()
{
  static bool bEnterCmd = true;

  if (Serial.available())
  {
    int nRcv;
    nRcv = Serial.read();
    Serial.print("RECEIVED: ");
    Serial.print(nRcv, DEC);
    Serial.println();
    parseCommand(nRcv);
    bEnterCmd = true;
  }
  else
  {
    if (bEnterCmd)
    {
      Serial.print("Enter command: ");
      bEnterCmd = false;
    }
  }
}

/// Called at startup
void DiagnoseCommander::init()
{
  Serial.begin(SERIAL_BAUDRATE);
}

// EOF