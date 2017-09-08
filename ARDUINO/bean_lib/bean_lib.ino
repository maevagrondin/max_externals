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


#include "BLEManager_Bean.h"



/*************************************************************************************************
 * USER : initialisation of variables and pins
 *************************************************************************************************/
#define ADDRESS 145003089
const int pause = 1000;


void setup() {
  pinMode(A0, INPUT);

  
/*************************************************************************************************
 * LIB : initialisation of BLE advertisement and custom UUID
 *************************************************************************************************/
  init_BLE(ADDRESS);
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
 * LIB: send array1, array2 and array3 to computer
 *************************************************************************************************/
  send_arrays();


/*************************************************************************************************
 * USER : use the values received from the computer to set the LED
 *************************************************************************************************/
  if(authenticated){
    if(pwm == 0){
      Bean.setLed(received[1]*255, received[1]*255, received[1]*255);
    }
    if(pwm == 1){
      Bean.setLed(pwm_values[0], pwm_values[1], pwm_values[2]);
    } 
  }

  
}


