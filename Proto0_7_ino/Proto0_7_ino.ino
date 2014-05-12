#include <Event.h>
#include <Timer.h>
// Kolla på Input voltage (får spikar) och current (skala och få ännu mjukare)
// Ver 0.7 Proto
// Den hät versionen baseras på ver 0.6 för kommunikation mot logwieer 
// och atmel charger för laddningen.

// Setup constants for battery
// Battery Characteristics: FAST charge TERMINATION       
//***************************************************************************
// -dV = 20mV per cell,rounded up to closest one 
const int   I_FAST = 14;              // Define Fast Charge current in mA.
const int  NEG_dV  =  6;               // Voltage drop (10=1V)


// Setup Constatns for charger
const int   BaudRate = 9600;           // Sepeed for cerial comunication
const int   InputVoltagePin = 5;        // Pin used to measure Supply-voltage
const int   BatteryVoltagePin = 0;      // Pin used to measure battery-voltage
const int   BatteryCurrentPin = 1;      // Pin used to measure charge-current
const int   TemperaturPin = 4;          // Pin used to measure Batery-temperatur
const int   pwmPin = 9;                // Pin used for PWM-output controlling charge voltage/current (timer1 ??)
const int   ChargeLed = 13;            // Arduino Nano onboard LED Pin 13, Used for Charge indication.

// Setup variables
int  sensorValue = 0;      // Variable colecting the ADC value when measuring.
int  outputValue =0;        // Recalibrated ADC value
Timer t1;                  // Timer for SendSerial
int  ChargeStatus = 1;      // 1=Monitor, 2=FastCharge, 3=Triclesharge, 9=Error
int  last_min_temp;        // Battery temperatur last minute (for delta T calc)
int  last_min_volt;        // Battery voltage last minute (for delta V calc)
int  last_min = 0;         // Variable used to measure deltaT DeltaV last minute

// Variables set on serialport (May be changed to an structure)
int  time = 0;            // Variable to store time sinse chargestart
int  InputVoltage;        // Last value from Suply-Voltage measur.
int  BatteryVoltage;      // Last value from Batery-voltage measure.
int  BatteryCurrent;      // Last value from batery-current measure, unfilterd for pwm adj.
int  BatteryCurrentNorm;  // Normalized value of BatteryCurrent sent to serial

int  BatteryTemp;         // Last Battery temp reading
int  BatteryTempNorm;      // Normalized battery Temp
int  pwm;                 // pwm-value for charge

#define NUM_READS 30                    // Number times remeasuring ADC, for normalization.
// Setup temperature smoothing buffer
#define TEMP_BUF_L  7    // Number of values in the temperatur-buffer, higher=more smooth/slower responce
int tempBuffer[TEMP_BUF_L];  // Array of teperatur-values.r 
int tempBufferPos = 0; // Where in the temperature-buffer to next temperature measure


// Setup inputvoltage smoothing buffer
#define IVOLT_BUF_L  7    // Number of values in the inputvoltage-buffer, higher=more smooth/slower responce
int ivoltBuffer[IVOLT_BUF_L];  // Array of inputvoltage-values. 
int ivoltBufferPos = 0; // Where in the inputvoltage-buffer to next inputvoltage measure

// Setup batterycurrent smoothing buffer
#define BCUR_BUF_L  7    // Number of values in the batterycurrent-buffer, higher=more smooth/slower responce
int bcurBuffer[BCUR_BUF_L];  // Array of batterycurrent-values. 
int bcurBufferPos = 0; // Where in the batterycurrent-buffer to next batterycurrent measure


// Setup everyting before program starts
void setup() {

  // initialize serial communications at defined bps:
  Serial.begin(BaudRate);   
  
  // Setup timer to send data on serial every sec
  t1.every(1000,SendSerial);
  
  // Fill tempBuffer vith real values
   for(int i=0;i<TEMP_BUF_L;i++){
     measureTemp();
   }

  // Start pwm with xx% Duty
  pinMode(pwmPin, OUTPUT);   // sets the pin as output
  pwm = 0;
  analogWrite(pwmPin, pwm); 
//  Remove comment to set high pwm freq. Standard freq. is 490 of 980 HZ depending on pin.  
//  setPwmFrequency(9, 1);    // pin,prescaler   pin9 default freq 32k.
 setPwmFrequency(9, 8);    // pin,prescaler   pin9 default freq 3.9k.

}




void loop() {
  
  // Get temperature and calculate
  sensorValue = measureTemp();
  BatteryTemp = GetTempValue(sensorValue);
  last_min_temp = BatteryTemp;
  
  // Get Battery-voltage and calculate
  sensorValue = analogRead(BatteryVoltagePin);            
  // map it to the range of the analog out:
  BatteryVoltage = map(sensorValue, 0, 1023, 0, 440);  
  last_min_volt = BatteryVoltage;    // Store battery voltage att charge start

  time = 0;   // Reset charge-time.
  ChargeStatus = 2;    // Set ChargeStatus to FastCharge
  t1.oscillate(ChargeLed,250,HIGH);    // Set Fastcharge blink (250ms) 


  while (ChargeStatus == 2) // While charge status = Fast
  {
    do  // Loop until charge current == I_FAST, "soft start"
    {   //  Measure Battery Current:
        sensorValue = analogRead(BatteryCurrentPin);            
        // map it to the range of the analog out:
        BatteryCurrent = map(sensorValue, 0, 1023, 0, 1006);        
        if (BatteryCurrent < I_FAST)    // Increase pwm if current to low
        {
          if (pwm > 254) { pwm = 254; }  // Battery not connected or input voltage to low.
          pwm++;
          analogWrite(pwmPin, pwm);
        }
        else if (BatteryCurrent > I_FAST)    // Decrease pwm if current to high
        {
          if (pwm < 1) { pwm = 1; } // Output shortcut ??
          pwm--;
          analogWrite(pwmPin, pwm);
        } 
          t1.update();
    } while (BatteryCurrent != I_FAST);
    // I_FAST is set now.  

    if (time < last_min-60) // If 60sec has pased
    {
      last_min = time;
      //If charge voltage is falling,	change to trickle mode
      //  Measure Battery voltage
      sensorValue = analogRead(BatteryVoltagePin);            
      // map it to the range of the analog out:
      BatteryVoltage = map(sensorValue, 0, 1023, 0, 440);  
      if ( (last_min_volt - BatteryVoltage) > NEG_dV )
      {
          //Stop the PWM, flag FAST charge dV/dt
      	//termination and change charge mode to 
      	//"TRICKLE". 
  	analogWrite(pwmPin,0);
	ChargeStatus = 3;    // Set Chargestatus to Trickle
        t1.oscillate(ChargeLed,2000,HIGH);    // Fastcharge ended, set Tricklecharge blink (2000ms) 

      }
    }

  t1.update();

  }
 
}
