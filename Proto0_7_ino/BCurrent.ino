// Subrutine to return Normalized Batterycurrent
int GetNormalizeBatteryCurrent(){
   int medelV = BatteryCurrent;
   // Put value in bcurBuffert and point to next pos in buffer.
   bcurBuffer[bcurBufferPos] = medelV;
   bcurBufferPos = bcurBufferPos + 1;
   if (tempBufferPos>BCUR_BUF_L-1){
     bcurBufferPos = 0;
   }
   
   // Return average of tempBuffer, remove 2 higest and 2 lowest values
   int returnval = 0;
   for(int i=2;i<(BCUR_BUF_L-2);i++){
     returnval = returnval + bcurBuffer[i];
   }
   returnval = returnval/(BCUR_BUF_L-4);
   return returnval;
}
