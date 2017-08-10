ScratchData lastScratch;
const char * res = "test";
float array1[5] = {1,10,11,12,13};
float array2[4] = {2,99,98,97};

void setup() {
  Bean.setBeanName("1234");
}

void loop() {

  // Scratch Service UUID: a495ff20-c5b1-4b44-b512-1370f02d74de
  
  // Scratch Characteritics UUID:
  // a495ff21-c5b1-4b44-b512-1370f02d74de
  // a495ff22-c5b1-4b44-b512-1370f02d74de
  // a495ff23-c5b1-4b44-b512-1370f02d74de
  // a495ff24-c5b1-4b44-b512-1370f02d74de
  // a495ff25-c5b1-4b44-b512-1370f02d74de
  
  
  if(Bean.getConnectionState() == false){
    Bean.setLed(123,255,67);
    Bean.sleep(500);
    Bean.setLed(0,0,0);
    Bean.sleep(500);
  }
  else{
    Bean.setLed(0,0,0);
    Bean.setScratchData(1, (const unsigned char *) array1, sizeof(array1)); // scratch characteritic 1
    Bean.setScratchData(1, (const unsigned char *) array2, sizeof(array2));


   
  ScratchData thisScratch = Bean.readScratchData(2);
  bool matched = compareScratch(&thisScratch, &lastScratch);
  lastScratch = thisScratch;

  if (!matched) {
    // Set the LED to green for 1 second if the data has changed
    Bean.setLed(0, 255, 0);
    Bean.sleep(1000);
    Bean.setLed(0, 0, 0);
  } else {
    // Otherwise, just sleep for one second and check again
    Bean.sleep(1000);
  }
  }
}


bool compareScratch(ScratchData *scratch1, ScratchData *scratch2) {
  if (scratch1->length != scratch2->length) {
    return false;
  }

  for (int i = 0; i < scratch1->length; i++) {
    if (scratch1->data[i] != scratch2->data[i]) {
      return false;
    }
  }
  
  return true;
}


