// bean = 8 pins (2 analog + 6 digital)

// send 20 bytes maximum (5 float = 5*4 bytes)
float array1[5] = {1,10,11,12,13};
float array2[5] = {2,99,98,97,96};
bool received[9] = {0,0,0,0,0,0,0,0,0};
int pwm = 0;


void setup() {
  // custom address
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

  // blink while not connected
  if(Bean.getConnectionState() == false){
    Bean.setLed(255,255,255);
    Bean.sleep(500);
    Bean.setLed(0,0,0);
    Bean.sleep(500);
  }

  // send data to central device (scratch 1) + read data from central device and store on receive array (scratch 2)
  else{
    // write on scratch characteristic 1
    Bean.setScratchData(1, (const unsigned char *) array1, sizeof(array1));
    Bean.setScratchData(1, (const unsigned char *) array2, sizeof(array2));

    // read on scratch characteristic 2
    ScratchData scratch = Bean.readScratchData(2);
    storeScratch(&scratch);
  }
}


// stores values from scratch in received array + set LED to value specified by central device
void storeScratch(ScratchData *scratch) {
  if(scratch->data[0] == 0){
    if(scratch->length <= 10){
      for(int i=1; i<scratch->length; i++){
        received[i] = scratch->data[i];
      }
      if(pwm == 0){
        Bean.setLed(received[8]*255, received[8]*255, received[8]*255);
      }
    }
  }
  if(scratch->data[0] == 1){
    if(pwm == 1){
      Bean.setLed((int)scratch->data[4], (int)scratch->data[8], (int)scratch->data[12]);
    }
    if((int)scratch->data[4] == 0 && (int)scratch->data[8] == 0 && (int)scratch->data[12] == 0){
      pwm = 0;
    }
    else{
      pwm = 1;
    }
  }
}




