
// Subrutine to output latest value to serialport
// No parameters is needed, outputting global variables.
void SendSerial()
{
  sensorValue = measureIvolt();                           //  Measure input voltage:   
  InputVoltage = map(sensorValue, 0, 1023, 0, 440);       // map it to the range of the analog out: 
  
  sensorValue = analogRead(BatteryVoltagePin);            //  Measure Battery voltage:
//  sensorValue = measureBatteryvoltage();            //  Measure Battery voltage:
  BatteryVoltage = map(sensorValue, 0, 1023, 0, 4400);     // map it to the range of the analog out:
  
  BatteryTempNorm = GetNormalizedBatteryTemp();             // Measure temp and normalize.

  BatteryCurrentNorm = GetNormalizeBatteryCurrent();        // Normalize and get batteryCurrent
  
  // print the results to the serial monitor:
  Serial.print("$1;1;");
  Serial.print(time);
  Serial.print(";");
  Serial.print(BatteryVoltage);
  Serial.print(";");
  Serial.print(BatteryCurrentNorm);
  Serial.print(";");
  Serial.print(BatteryTempNorm);
  Serial.print(";");
  Serial.print(InputVoltage);
  Serial.print(";");
  Serial.print(pwm);
  Serial.println(";0");  
  time++;
  
 }
