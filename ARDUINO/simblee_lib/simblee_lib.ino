/*
 * Send values to the computer:
 * The user must place int or float values into "array1", "array2" or "array3" at indexes 1, 2, 3 and 4 (3x4=12 values in total)
 *
 * Read values from the computer:
 * The computer can send 12 boolean values to the Simblee stored in array "received" at indexes between 0 and 11
 * These values can be used to set the value of a pin or an LED
 *
 * Read PWM values from the computer:
 * The computer can send 3 int values to the Simblee stored in array "pwm_values" at indexes 0, 1 qnd 2
 * These values can be used to control the brightness of an LED
 *
 * PWM indicator:
 * The boolean "pwm" equals 1 if the computer is sending PWM values, 0 otherwise
 *
 * Connection inticator:
 * The boolean "connection" equals 1 if the Simblee is connected to a computer (central device), 0 otherwise
 *
 * Everything in sections "LIB" must remain unchanged
 * Everything in sections "USER" is an example of the use of user arrays (array1, array2, array3, received, pwm_values) and can be changed
 * The user has 3 sections :
 *  - one for initializing his variables in setup function
 *  - one at the beginning of loop before "array1", "array2" and "array3" are sent
 *  - one at the end of loop after "array1", "array2" and "array3" were sent and after "received" and "pwm_values" were filled
 */


 #include "BLEManager_Simblee.h"

/*************************************************************************************************
 * USER : initialisation of variables and pins
 *************************************************************************************************/
const int LED = 13;
const int pause = 1000;


void setup(){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(3, INPUT);
  pinMode(15, OUTPUT);
  digitalWrite(15,LOW);

/*************************************************************************************************
 * LIB : initialisation of BLE advertisement and custom UUID
 *************************************************************************************************/
 init_BLE();
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
  }


/*************************************************************************************************
 * LIB: send array1, array2 and array3 to computer
 *************************************************************************************************/
 send_arrays();



/*************************************************************************************************
 * USER : use the values received from the computer to set the LED
 *************************************************************************************************/
 if(connected){
    if(pwm == 0){
      digitalWrite(LED, received[7]);
      digitalWrite(15, received[0]);
    }
    if(pwm == 1){
      analogWrite(LED, pwm_values[0]);
      analogWrite(15, pwm_values[1]);
    }
 }
}
