/*
 * Control signals on a model railroad based on input AD values
 */


#define GREEN_LOW    (605-10)
#define GREEN_HIGH   (605+10)
#define YELLOW_LOW   (399-10)
#define YELLOW_HIGH  (399+10)
#define RED_LOW      (192-10)
#define RED_HIGH     (192+10)
#define WHITE_LOW      (147-10)
#define WHITE_HIGH     (147+10)

/// Print analogue input values for ADC A0, A1, ..., A5
void printAnalog(void)
{
  int nSensorValue;

  nSensorValue = analogRead(A0);  // G=605, Y=396, R=191, W=147
  Serial.print("A0=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A1);  // G=605, Y=NA, R=191
  Serial.print(" A1=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A2);  // G=603, Y=396, R=191
  Serial.print(" A2=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A3);  // G=604, Y=397, R=193
  Serial.print(" A3=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A4);  // G=605, Y=397, R=192
  Serial.print(" A4=");
  Serial.print(nSensorValue);
  nSensorValue = analogRead(A5);  // G=605, Y=---, R=192
  Serial.print(" A5=");
  Serial.print(nSensorValue);
  Serial.println();
}

static const int nPorts[] = { 13,12,11,10,9,8,7,6,5,4,3,2, 22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52 };
static const int nPortsNr = sizeof(nPorts) / sizeof(int);

void activateOutputsOneAfterOther_Init(void)
{
  int nIdx;
  
  for (nIdx = 0; nIdx < nPortsNr; nIdx++)
  {
    pinMode(nPorts[nIdx], OUTPUT);
    digitalWrite(nPorts[nIdx], LOW );
  }
}

/// Outputs are specified by nPorts.
/// Activate first output port. By pressing button connected to A0, activate next output port.
/// Walk through output ports is possible.
void activateOutputsOneAfterOther(void)
{
  int nSensorValue;
  static int nIdx = 0;
  static boolean bPressed = false;
  
  nSensorValue = analogRead(A0);  // G=605, Y=399, R=192
  digitalWrite(nPorts[nIdx], HIGH);
  Serial.print(nSensorValue);
  Serial.print(" ");
  Serial.println(nPorts[nIdx]);  
  
  if (nSensorValue < 700)
  {
    if (bPressed == false)
    {
      bPressed = true;
      digitalWrite(nPorts[nIdx], LOW);
      if (nIdx < nPortsNr)
      {
        nIdx++;
      }
      if (nIdx >= nPortsNr)
      {
        nIdx = 0;
      }
    }
  }
  else
  {
    bPressed = false;
  }
}

/// Activate output ports 0 ... 4
void activateOutputs0_4(void)
{
  int nIdx;
  
  for (nIdx = 0; nIdx < 5; nIdx++)
  {
    digitalWrite(nPorts[nIdx], HIGH);
  }
}

class Button
{
  protected:
    int nPin;        ///< AD channel, e.g. A0
    int nLow;        ///< Lower limit for pressed [0, 1023]
    int nHigh;       ///< Upper limit for pressed [0, 1023]
    int nAdcInRange; ///< Number of samples with nLow <= sample <= nHigh
    int nNrTicks;    ///< Maximal number of samples until button is considered as pressed
  public:
  /// Default constructor, initializes to values that are not plausible
  Button() : nPin(A0), nLow(1023), nHigh(0), nAdcInRange(0), nNrTicks(100)
  {}
  /// Constructor
  /// nPin: Input ADC pin, e.g. A0
  /// nLow: Lower Limit for pressed [0, 1023]
  /// nHigh: Upper Limit for pressed [0, 1023]
  Button(int pin, int low, int high) : nPin(pin), nLow(low), nHigh(high), nAdcInRange(0), nNrTicks(100)
  {}
  /// Destructor: does nothing at the moment
  ~Button()
  {}
  /// Set input channel for ADC
  void setAdcPin(int pin)    { nPin = pin; }
  /// Set lower limit
  void setAdcLow(int low)    { nLow = low; }
  /// Set upper limit
  void setAdcHigh(int high)    { nHigh = high; }
  /// Set maximal number of samples until AD value is considered to be correct
  void setNrTicks(int t)    { nNrTicks = t; }
  /// Measure one value
  void measure()
  {
    int nAdc;
    nAdc = analogRead(nPin);
    if ((nAdc >= nLow) && (nAdc <= nHigh))
    {
      if (nAdcInRange < nNrTicks)
      {
        nAdcInRange++;
      }
    }
    else
    {
      nAdcInRange = 0;
    }
  }
  /// Reset to a new measurement
  void reset()
  {
    nAdcInRange = 0;
  }
  /// Return true if the button was pressed
  boolean pressed()
  {
    boolean bRet = false;
    
    if (nAdcInRange >= nNrTicks)
    {
      bRet = true;
    }
    
    return bRet;
  }
};

class AusfahrSignal
{
  protected:
    Button ButtonRed;
    Button ButtonGreen;
    Button ButtonYellow;
    Button ButtonWhite;
    int nOutRed1;
    int nOutRed2;
    int nOutGreen;
    int nOutYellow;
    int nOutWhite;

  public:
    
    AusfahrSignal() : 
      nOutRed1    (-1)
    , nOutRed2    (-1)
    , nOutGreen   (-1)
    , nOutYellow  (-1)
    , nOutWhite   (-1)
    , ButtonRed   (A0, RED_LOW   , RED_HIGH   )
    , ButtonGreen (A0, GREEN_LOW , GREEN_HIGH )
    , ButtonYellow(A0, YELLOW_LOW, YELLOW_HIGH)
    , ButtonWhite (A0, WHITE_LOW , WHITE_HIGH )
    {}
    AusfahrSignal(int adcPin, int outRed1, int outRed2, int outGreen, int outYellow, int outWhite) : 
      nOutRed1    (outRed1)
    , nOutRed2    (outRed2)
    , nOutGreen   (outGreen)
    , nOutYellow  (outYellow)
    , nOutWhite   (outWhite) 
    , ButtonRed   (adcPin, RED_LOW   , RED_HIGH   )
    , ButtonGreen (adcPin, GREEN_LOW , GREEN_HIGH )
    , ButtonYellow(adcPin, YELLOW_LOW, YELLOW_HIGH)
    , ButtonWhite (adcPin, WHITE_LOW , WHITE_HIGH )
    {}
    ~AusfahrSignal()
    {}
    void hp0()
    {
      if (nOutRed1   >= 0) { digitalWrite(nOutRed1  , HIGH); }
      if (nOutRed2   >= 0) { digitalWrite(nOutRed2  , HIGH); }
      if (nOutGreen  >= 0) { digitalWrite(nOutGreen , LOW ); }
      if (nOutYellow >= 0) { digitalWrite(nOutYellow, LOW ); }
      if (nOutWhite  >= 0) { digitalWrite(nOutWhite , LOW ); }
    }
    void hp1()
    {
      if (nOutRed1   >= 0) { digitalWrite(nOutRed1  , LOW ); }
      if (nOutRed2   >= 0) { digitalWrite(nOutRed2  , LOW ); }
      if (nOutGreen  >= 0) { digitalWrite(nOutGreen , HIGH); }
      if (nOutYellow >= 0) { digitalWrite(nOutYellow, LOW ); }
      if (nOutWhite  >= 0) { digitalWrite(nOutWhite , LOW ); }
    }
    void hp2()
    {
      if (nOutRed1   >= 0) { digitalWrite(nOutRed1  , LOW ); }
      if (nOutRed2   >= 0) { digitalWrite(nOutRed2  , LOW ); }
      if (nOutGreen  >= 0) { digitalWrite(nOutGreen , HIGH); }
      if (nOutYellow >= 0) { digitalWrite(nOutYellow, HIGH); }
      if (nOutWhite  >= 0) { digitalWrite(nOutWhite , LOW ); }
    }
    void hp3()
    {
      if (nOutRed1   >= 0) { digitalWrite(nOutRed1  , HIGH); }
      if (nOutRed2   >= 0) { digitalWrite(nOutRed2  , HIGH); }
      if (nOutGreen  >= 0) { digitalWrite(nOutGreen , LOW ); }
      if (nOutYellow >= 0) { digitalWrite(nOutYellow, LOW ); }
      if (nOutWhite  >= 0) { digitalWrite(nOutWhite , HIGH); }
    }
    void activateAll()
    {
      if (nOutRed1   >= 0) { digitalWrite(nOutRed1  , HIGH); }
      if (nOutRed2   >= 0) { digitalWrite(nOutRed2  , HIGH); }
      if (nOutGreen  >= 0) { digitalWrite(nOutGreen , HIGH); }
      if (nOutYellow >= 0) { digitalWrite(nOutYellow, HIGH); }
      if (nOutWhite  >= 0) { digitalWrite(nOutWhite , HIGH); }
    }
    void init()
    {
      if (nOutRed1   >= 0) { pinMode(nOutRed1  , OUTPUT); }
      if (nOutRed2   >= 0) { pinMode(nOutRed2  , OUTPUT); }
      if (nOutGreen  >= 0) { pinMode(nOutGreen , OUTPUT); }
      if (nOutYellow >= 0) { pinMode(nOutYellow, OUTPUT); }
      if (nOutWhite  >= 0) { pinMode(nOutWhite , OUTPUT); }
      hp0();
      ButtonRed.reset();
      ButtonWhite.reset();
      ButtonGreen.reset();
      ButtonYellow.reset();
    }
    void measure()
    {
      ButtonGreen.measure();
      ButtonYellow.measure();
      ButtonRed.measure();
      ButtonWhite.measure();
      
      if (ButtonRed.pressed())
      {
        hp0();
        ButtonRed.reset();
      }
      else if (ButtonWhite.pressed())
      {
        hp3();
        ButtonWhite.reset();
      }
      else if (ButtonGreen.pressed())
      {
        hp1();
        ButtonGreen.reset();
      }
      else if (ButtonYellow.pressed())
      {
        hp2();
        ButtonYellow.reset();
      }
      else
      {}
    }
};

/*
AusfahrSignal S2 (A0, 13, 12, 11, 10,  9);
AusfahrSignal S4 (A1,  8,  7,  6,  5,  4);
AusfahrSignal S6 (A2,  2, 22, 24,  3, 52);
AusfahrSignal S8 (A3, 28, 30, 26, 32, 50);
AusfahrSignal S10(A4, 40, 44, 46, 42, 48);
*/

//                    R1  R1   G   Y   W
AusfahrSignal S2 (A4, 13, 12, 11, 10,  9);
AusfahrSignal S4 (A3,  8,  7,  6,  5,  4);
AusfahrSignal S6 (A2, 40, 42, 44, 46, 48);
AusfahrSignal S8 (A1,  3, 24,  2, 22, -1);
AusfahrSignal S10(A0, 30, 32, 28, 26, -1);
AusfahrSignal S12(A5, 52, -1, 50, -1, -1);

// the setup function runs once when you press reset or power the board
void setup()
{
  S2.init();
  S4.init();
  S6.init();
  S8.init();
  S10.init();
  S12.init();
  
  //activateOutputsOneAfterOther_Init();

  Serial.begin(9600);
}
// the loop function runs over and over again forever
void loop() 
{

  //printAnalog();
  //activateOutputsOneAfterOther();
  //activateOutputs0_4();
  
  if (true)
  {
    S2.measure();  
    S4.measure();  
    S6.measure();  
    S8.measure();  
    S10.measure(); 
    S12.measure();  
  }
  
  if (false)
  {
    S2.activateAll();  
    S4.activateAll();  
    S6.activateAll();  
    S8.activateAll();  
    S10.activateAll(); 
    S12.activateAll();  
  }
}
