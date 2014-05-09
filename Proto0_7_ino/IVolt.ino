

// Subrutine to measure inputvoltagePin (multiple times for normalization)
int measureIvolt(){
   // read multiple values and sort them in to array
   int sortedValues[NUM_READS];
   for(int i=0;i<NUM_READS;i++){
     int value = analogRead(InputVoltagePin);
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
   ivoltBuffer[ivoltBufferPos] = medelV;
   ivoltBufferPos = ivoltBufferPos + 1;
   if (ivoltBufferPos>IVOLT_BUF_L){
     ivoltBufferPos = 1;
   }
   
   // Return average of tempBuffer
   int returnval = 0;
   for(int i=1;i<=IVOLT_BUF_L;i++){
     returnval = returnval + ivoltBuffer[i];
   }
   returnval = returnval/(IVOLT_BUF_L);
   return returnval;
}


