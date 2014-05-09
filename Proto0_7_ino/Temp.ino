// Subrutine to return normalized tempwalue

int GetNormalizedBatteryTemp(){
  int iTemp;
  sensorValue = measureTemp();                            // Measure battery temperature
  iTemp = GetTempValue(sensorValue);                       // Map ADC-value to real temperature.
  return iTemp;
 }


// Subrutine to measure TemperaturPin (multiple times for normalization)
int measureTemp(){
   // read multiple values and sort them in to array
   int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){
     int value = analogRead(TemperaturPin);
          int j;
     if(value<sortedValues[0] || i==0){
        j=0; //insert at first position
     }
     else{
       for(j=1;j<i;j++){
          if(sortedValues[j-1]<=value && sortedValues[j]>=value){
            // j is insert position
            break;
          }
       }
     }
     for(int k=i;k>j;k--){
       // move all values higher than current reading up one position
       sortedValues[k]=sortedValues[k-1];
     }
     sortedValues[j]=value; //insert current reading
   }
   // Get the 10 center values and calculate average
   int medelV = 0;
   for(int i=NUM_READS/2-5;i<(NUM_READS/2+5);i++){
     medelV = medelV + sortedValues[i];
   }
   medelV = medelV/10;
   // Put value in tempBuffert and point to next pos in buffer.
   tempBuffer[tempBufferPos] = medelV;
   tempBufferPos = tempBufferPos + 1;
   if (tempBufferPos>TEMP_BUF_L){
     tempBufferPos = 1;
   }
   
   // Return average of tempBuffer, remove 2 higest and 2 lowest values
   int returnval = 0;
   for(int i=1;i<(TEMP_BUF_L);i++){
     returnval = returnval + tempBuffer[i];
   }
   returnval = returnval/(TEMP_BUF_L);
   return returnval;
}


// Subrutine to calculate temperature from ADC-value
// Using two tables one with value in deg C an one with ADC-value.
// Returing interpolated temp-value 
int GetTempValue(int adc_value){
   
  // Table rufly measured
  int temp_table[12] = {100,200,250,290,330,370,410,450,490,510,530,550 };
  int adc_table[12] =  {300,350,378,410,442,472,496,518,538,550,559,566 };  

  int ix;                // Loop variable
  int NO_TEMPS = 12;    // Number of values in the table
  long interp, interp3;  // Interpolated ADC and Temp-value
  
  // Find closest lower table possition
  for(ix = NO_TEMPS - 2; ix > -1; ix--)  {
  	if(adc_value > adc_table[ix])  {
   		break;
  	}
  }
  if(ix < 0)ix = 0;

// Interpolate and return value.
  interp = adc_table[ix + 1] - adc_table[ix];
  if(interp != 0)  {
    interp3 = (adc_value - adc_table[ix]);
    interp3 = (100 * interp3);
    interp = interp3 / interp;
  }
  return((temp_table[ix] + interp * (temp_table[ix+1] - temp_table[ix])/ 100));
}

