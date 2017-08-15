// LIB : init
// send 20 bytes maximum (5 float = 5*4 bytes)
float array1[5] = {1,0,0,0,0};
float array2[5] = {2,0,0,0,0};
float array3[5] = {3,0,0,0,0}; 
bool received[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int pwm_values[3] = {0,0,0};
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

  // TO BE DELETED : blink while not connected
  if(Bean.getConnectionState() == false){
    Bean.setLed(255,255,255);
    Bean.sleep(500);
    Bean.setLed(0,0,0);
    Bean.sleep(500);
  }

  // LIB : send data to central device (scratch 1) + read data from central device and store on receive array (scratch 2)
  if(Bean.getConnectionState() == true){

    // USER : set values before sending
    array1[1] = Bean.getAccelerationX();
    array1[2] = Bean.getAccelerationY();
    array1[3] = Bean .getAccelerationZ();

    
    // LIB : write on scratch characteristic 1
    Bean.setScratchData(1, (const unsigned char *) array1, sizeof(array1));
    Bean.setScratchData(1, (const unsigned char *) array2, sizeof(array2));
    Bean.setScratchData(1, (const unsigned char *) array3, sizeof(array3));


    // LIB : read on scratch characteristic 2
    ScratchData scratch = Bean.readScratchData(2);
    storeScratch(&scratch);


    // USER : use values received from computer
    if(pwm == 0){
      Bean.setLed(received[8]*255, received[8]*255, received[8]*255);
    }
    if(pwm == 1){
      Bean.setLed(pwm_values[0], pwm_values[1], pwm_values[2]);
    }

    
  }
}


// LIB : stores values from scratch in 'received' and 'pwm_values' arrays
void storeScratch(ScratchData *scratch) {
  if(scratch->data[0] == 0){
    if(scratch->length <= 13){
      for(int i=1; i<scratch->length; i++){
        received[i-1] = scratch->data[i];
      }
    }
  }
  if(scratch->data[0] == 1){
    if(scratch->length == 16){
      for(int i=1; i<4; i++){
        pwm_values[i-1] = (int)scratch->data[i*4];
      }
    }
    if(pwm_values[0] == 0 && pwm_values[1] == 0 && pwm_values[2] == 0){
      pwm = 0;
    }
    else{
      pwm = 1;
    }
  }
}




