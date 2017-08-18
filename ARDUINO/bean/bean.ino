/*
 * Send values to the computer:
 * The user must place int or float values into "array1", "array2" or "array3" at indexes 1, 2, 3 and 4 (3x4=12 values in total)
 *
 * Read values from the computer:
 * The computer can send 12 boolean values to the Simblee stored in array "received" at indexes between 0 and 11
 * These values can be used to set the value of a pin or an LED
 *
 * Read PWM values from the computer:
 * The computer can send 3 int values to the Simblee stored in array "pwm_values" at indexes 1, 2 and 3
 * These values can be used to control the brightness of an LED
 *
 * PWM indicator:
 * The boolean "pwm" equals 1 if the computer is sending PWM values, 0 otherwise
 *
 * Connection inticator:
 * The boolean "connected" equals 1 if the Simblee is connected to a computer (central device), 0 otherwise
 *
 * Anything in sections "LIB" must remain unchanged
 * Anything in section "USER" is an example of the use of user arrays (array1, array2, array3, received, pwm_values) and can be changed
 * The user has 3 sections :
 *  - one for initializing his variables in setup function
 *  - one at the beginning of loop before "array1", "array2" and "array3" are sent
 *  - one at the end of loop after "array1", "array2" and "array3" were sent and after "received" and "pwm_values" were filled
 */





/*************************************************************************************************
 * LIB : initialisation
 *************************************************************************************************/

// max of 20 bytes accepted (5x1 int of 4 bytes)
float array1[5] = {1,0,0,0,0};
float array2[5] = {2,0,0,0,0};
float array3[5] = {3,0,0,0,0}; 
bool received[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int pwm_values[3] = {0,0,0};
bool pwm = 0;
bool connected = 0;






/*************************************************************************************************
 * USER : initialisation of variables and pins
 *************************************************************************************************/
 const int pause = 1000;
 const int pin = 5;



void setup() {
  pinMode(pin, OUTPUT);

  
/*************************************************************************************************
 * LIB : initialisation of BLE advertisement and custom UUID
 *************************************************************************************************/
  Bean.setBeanName("1234");
}


void loop() {
/*************************************************************************************************
 * USER : set values to send to computer
 *************************************************************************************************/
 if(connected){
    array1[1] = Bean.getAccelerationX() + 500;
    array1[2] = Bean.getAccelerationY() + 500;
    array1[3] = Bean .getAccelerationZ() + 500;
 }
  if(!connected){
    Bean.setLed(255,255,255);
    delay(pause);
    Bean.setLed(0,0,0);
    delay(pause);
  }




/*************************************************************************************************
 * LIB
 * 
 * Scratch Service UUID: a495ff20-c5b1-4b44-b512-1370f02d74de
 * 
 * Scratch Characteritics UUID:
 * a495ff21-c5b1-4b44-b512-1370f02d74de (scratch 1)
 * a495ff22-c5b1-4b44-b512-1370f02d74de (scratch 2)
 * a495ff23-c5b1-4b44-b512-1370f02d74de (scratch 3)
 * a495ff24-c5b1-4b44-b512-1370f02d74de (scratch 4)
 * a495ff25-c5b1-4b44-b512-1370f02d74de (scratch 5)
 *************************************************************************************************/




/*************************************************************************************************
 * LIB: send array1, array2 and array3 to computer
 * Send data to central device (scratch 1) + read data from central device and store on receive array (scratch 2)
 *************************************************************************************************/
  if(Bean.getConnectionState() == true){
    connected = 1;
    array1[0] = 1;
    array2[0] = 2;
    array3[0] = 3;
    
    // write on scratch characteristic 1
    Bean.setScratchData(1, (const unsigned char *) array1, sizeof(array1));
    Bean.setScratchData(1, (const unsigned char *) array2, sizeof(array2));
    Bean.setScratchData(1, (const unsigned char *) array3, sizeof(array3));

    // read on scratch characteristic 2
    ScratchData scratch = Bean.readScratchData(2);
    storeScratch(&scratch);
  }
  if(Bean.getConnectionState() == false){
    connected = 0;
  }
    


/*************************************************************************************************
 * USER : use the values received from the computer to set the LED
 *************************************************************************************************/
  if(connected){
    if(pwm == 0){
      Bean.setLed(received[8]*255, received[8]*255, received[8]*255);
    }
    if(pwm == 1){
      Bean.setLed(pwm_values[0], pwm_values[1], pwm_values[2]);
    } 
  }

  
}







/*************************************************************************************************
 * LIB : store the values received from the computer into array "received" and array "pwm_values"
 *************************************************************************************************/
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




