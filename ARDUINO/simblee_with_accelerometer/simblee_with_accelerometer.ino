/*
 * Address of board:
 * The address of the board is the ESN (Electronic Serial Number) of the board
 * It can be retrieved using Serial.println(SimbleeCOM.getESN())
 * This address must be used in the Max Object BLEManager to recognize the board
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
#include <SimbleeBLE.h>

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




/*************************************************************************************************
 * USER : initialisation of variables and pins
 *************************************************************************************************/
const int LED = 13;
const int pause = 1000;


void setup(){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  




/*************************************************************************************************
 * LIB : initialisation of BLE advertisement and custom UUID
 *************************************************************************************************/
  char res[1024];
  sprintf(res, "%d", SimbleeCOM.getESN());
  SimbleeBLE.deviceName = res;
  SimbleeBLE.advertisementInterval = 20;
  SimbleeBLE.txPowerLevel = -4;
  SimbleeBLE.customUUID = "2220";
  SimbleeBLE.begin();
}






void loop()
{
/*************************************************************************************************
 * USER : set values to send to computer
 *************************************************************************************************/
  if(!connected){
    digitalWrite(LED, HIGH);
    delay(pause);
    digitalWrite(LED, LOW);
    delay(pause);
  }
  if(connected){
    array1[1] = analogRead(3);
    accelerometer[1] = analogRead(6);
    accelerometer[2] = analogRead(5);
  }



/*************************************************************************************************
 * LIB: send array1, array2 and array3 to computer
 *************************************************************************************************/
  if(authenticated){

    battery_level[0] = 0;
    accelerometer[0] = 1;
    temperature[0] = 2;
    array1[0] = 3;
    array2[0] = 4;
    array3[0] = 5;
    
    temperature[1] = Simblee_temperature(CELSIUS);

    SimbleeBLE.send((char *)battery_level, sizeof(battery_level));
    SimbleeBLE.send((char *)accelerometer, sizeof(accelerometer));
    SimbleeBLE.send((char *)temperature, sizeof(temperature));
    SimbleeBLE.send((char *)array1, sizeof(array1));
    SimbleeBLE.send((char *)array2, sizeof(array2));
    SimbleeBLE.send((char *)array3, sizeof(array3));
    
    SimbleeBLE.connectable = false;
  }

/*************************************************************************************************
 * USER : use the values received from the computer to set the LED
 *************************************************************************************************/
  if(authenticated){
    if(pwm == 0){
      digitalWrite(LED, received[0]);
    }
    if(pwm == 1){
      analogWrite(LED, pwm_values[2]);
    }
  }
}







/*************************************************************************************************
 * LIB : store the values received from the computer into array "received" and array "pwm_values"
 *************************************************************************************************/
void SimbleeBLE_onReceive(char *data, int len){

  // Boolean array received from the BLEManager, indexed by 0, contains 12 values
  if(data[0] == 0 && len == 13){
    for(int i=1; i<len; i++){
      received[i-1] = data[i];
    }
  }

  // Int array received from the BLEManager, indexed by 1, contains 3 values
  if(data[0] == 1 && len == 16){
    int * res= (int *) data;
    for(int i=1; i<4; i++){
      pwm_values[i-1] = res[i];
    }
    if(pwm_values[0] == 0 && pwm_values[1] == 0 && pwm_values[2] == 0){
      pwm = 0;
    }
    else{
      pwm = 1;
    }
  }

  // Int array received from the BLEManagerm indexed by 2, contains 4 values
  if(data[0] == 2 && len == 20){
    int * res = (int *) data;
    int counter = 0;
    for(int i=0; i<4; i++){
      if(res[i+1] == authentication_code[i]){
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


/*************************************************************************************************
 * LIB : connection and deconnection updates
 *************************************************************************************************/
void SimbleeBLE_onConnect(){
  connected = 1;
}

void SimbleeBLE_onDisconnect(){
  connected = 0;
}


