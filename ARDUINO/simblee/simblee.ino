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
#include <SimbleeBLE.h>

// max of 20 bytes accepted (5x1 int of 4 bytes)
float array1[5] = {1,0,0,0,0};
float array2[5] = {2,0,0,0,0};
float array3[5] = {3,0,0,0,0};

bool received[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int pwm_values[3] = {0,0,0};

bool pwm = 0;
int connected = 0;




/*************************************************************************************************
 * USER : initialisation of variables and pins
 *************************************************************************************************/
const int LED = 13;
const int pause = 1000;


void setup(){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(3, INPUT);




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
  if(connected){
    array1[1] = analogRead(3);
    array1[2] = 1995;
    array3[4] = 42;
  }
  if(!connected){
    digitalWrite(LED, HIGH);
    delay(pause);
    digitalWrite(LED, LOW);
    delay(pause);
  }


/*************************************************************************************************
 * LIB: send array1, array2 and array3 to computer
 *************************************************************************************************/
  if(connected){
    array1[0] = 1;
    array2[0] = 2;
    array3[0] = 3;
    SimbleeBLE.send((char *)array1, sizeof(array1));
    SimbleeBLE.send((char *)array2, sizeof(array2));
    SimbleeBLE.send((char *)array3, sizeof(array3));
    SimbleeBLE.connectable = false;
  }

/*************************************************************************************************
 * USER : use the values received from the computer to set the LED
 *************************************************************************************************/
  if(pwm == 0){
    digitalWrite(LED, received[7]);
  }
  if(pwm == 1){
    analogWrite(LED, pwm_values[0]);
  }
}


/*************************************************************************************************
 * LIB : store the values received from the computer into array "received" and array "pwm_values"
 *************************************************************************************************/
void SimbleeBLE_onReceive(char *data, int len){
  if(data[0] == 0){
    if(len<=13){
      for(int i=1; i<len; i++){
        received[i-1] = data[i];
      }
    }
  }
  if(data[0] == 1){
    int * res= (int *) data;
    if(sizeof(res) <= 4){
      for(int i=1; i<sizeof(res); i++){
        pwm_values[i-1] = res[i];
      }
    }
    if(res[1] == 0){
      pwm = 0;
    }
    else{
      pwm = 1;
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
