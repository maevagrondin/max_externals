/*
 * Address of board:
 * The user must define the address of the board in the macro ADDRESS (#define ADDRESS value)
 * The address for LightBlue Beans is the Serial Number (SN) written on the chip after the letters "EPC"
 * This address will enable the board to be recognized by the Max Object BLEManager
 * 
 * 
 * Send values to the computer:
 * The user must place int or float values into "array1", "array2" or "array3" at indexes 1, 2, 3 and 4 (3x4=12 values in total)
 * These values are already initialized with 0
 *
 *
 * Read values from the computer:
 * The computer can send 12 boolean values to the board stored in array "received" at indexes between 0 and 11
 * E.G These values can be used to set the value of a pin or an LED
 *
 *
 * Read PWM values from the computer:
 * The computer can send 3 int values to the board stored in array "pwm_values" at indexes 0, 1 and 2
 * These values can be used to control the brightness of an LED
 *
 *
 * PWM indicator:
 * The boolean "pwm" equals 1 if the computer is sending PWM values, 0 otherwise
 *
 *
 * Connection indicator:
 * The boolean "connected" equals 1 if the board is connected to a computer (central device), 0 otherwise
 * 
 * 
 * Authentication indicator:
 * The boolean "authenticated" equals 1 if the board is connected to the right Max Object (BLEManager)
 * The board will start sending its values to the central device only once the central device is authenticated
 * The board will start reading values from the central device only once the central device is authenticated
 * The board won't send any values if the central device is connected but not authenticated
 *
 *
 * Everything in sections "LIB" must remain unchanged
 * Everything in sections "USER" is an example of the use of indicators (connected, authenticated, pwm) and user arrays (array1, array2, array3, received, pwm_values) and can be changed
 * The user has 3 sections :
 *  - one for initializing his variables in setup function
 *  - one at the beginning of loop before "array1", "array2" and "array3" are sent
 *  - one at the end of loop after "array1", "array2" and "array3" were sent and after "received" and "pwm_values" were filled
 */





/*************************************************************************************************
 * LIB : initialisation
 *************************************************************************************************/

// max of 20 bytes accepted in arrays to send (5x1 int of 4 bytes)
float battery_level[2] = {0,0};
float accelerometer[4] = {1,0,0,0};
float temperature[2] = {2,0};
float array1[5] = {3,0,0,0,0};
float array2[5] = {4,0,0,0,0};
float array3[5] = {5,0,0,0,0}; 
bool received[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int pwm_values[3] = {0,0,0};
bool pwm = 0;
bool connected = 0;
int authentication_code[4] = {9,7,4,8};
bool authenticated = 0;
ScratchData scratch;




/*************************************************************************************************
 * USER : initialisation of variables and pins
 *************************************************************************************************/
#define ADDRESS 145008005
const int pause = 1000;




void setup() {
  pinMode(A0, INPUT);

  
/*************************************************************************************************
 * LIB : initialisation of BLE advertisement and custom UUID
 *************************************************************************************************/
  char addr[1024];
  sprintf(addr, "%lu", ADDRESS);
  Bean.setBeanName(addr);
  Bean.keepAwake(true);
}




void loop() {

/*************************************************************************************************
 * USER : set values to send to computer
 *************************************************************************************************/
 if(connected){
    array1[1] = analogRead(A0);
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
  connected = Bean.getConnectionState();
  if(authenticated){
    battery_level[0] = 0;
    accelerometer[0] = 1;
    temperature[0] = 2;
    array1[0] = 3;
    array2[0] = 4;
    array3[0] = 5;
    
    battery_level[1] = Bean.getBatteryLevel();
    temperature[1] = Bean.getTemperature();
    accelerometer[1] = Bean.getAccelerationX() + 500;
    accelerometer[2] = Bean.getAccelerationY() + 500;
    accelerometer[3] = Bean.getAccelerationZ() + 500;
    
    // write on scratch characteristic 1
    Bean.setScratchData(1, (const unsigned char *) battery_level, sizeof(battery_level));
    Bean.setScratchData(1, (const unsigned char *) accelerometer, sizeof(accelerometer));
    Bean.setScratchData(1, (const unsigned char *) temperature, sizeof(temperature));
    Bean.setScratchData(1, (const unsigned char *) array1, sizeof(array1));
    Bean.setScratchData(1, (const unsigned char *) array2, sizeof(array2));
    Bean.setScratchData(1, (const unsigned char *) array3, sizeof(array3));

    // read on scratch characteristic 2 (received) and 3 (pwm_values) 
    scratch = Bean.readScratchData(2);
    storeScratch(&scratch);
    scratch = Bean.readScratchData(3);
    storeScratch(&scratch); 
  }


  if(!authenticated){
    // read on scratch characteristic 4 (authentication code)
    scratch = Bean.readScratchData(4);
    storeScratch(&scratch); 
  }
    


/*************************************************************************************************
 * USER : use the values received from the computer to set the LED
 *************************************************************************************************/
  if(authenticated){
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

  // Boolean array received from the BLEManager, indexed by 0, contains 12 values
  if(scratch->data[0] == 0 && scratch->length == 13){
    for(int i=1; i<scratch->length; i++){
      received[i-1] = scratch->data[i];
    }
  }

  // Int array received from the BLEManager, indexed by 1, contains 3 values
  if(scratch->data[0] == 1 && scratch->length == 16){
    for(int i=1; i<4; i++){
      pwm_values[i-1] = (int)scratch->data[i*4];
    }
    if(pwm_values[0] == 0 && pwm_values[1] == 0 && pwm_values[2] == 0){
      pwm = 0;
    }
    else{
      pwm = 1;
    }
  }

  // Int array received from the BLEManager indexed by 2, contains 4 values
  if(scratch->data[0] == 2 && scratch->length == 20){
    int counter = 0;
    for(int i=0; i<4; i++){
      if((int)scratch->data[(i+1)*4] == authentication_code[i]){
        counter++;
      }
    }
    if(counter == 4){
      authenticated = 1;
    }
    else{
      authenticated = 0;
    }
  }
}




