#ifndef BLEMANAGER_SIMBLEE_H
#define BLEMANAGER_SIMBLEE_H

#include <SimbleeBLE.h>

extern float battery_level[2];
extern float accelerometer[4];
extern float temperature[2];
extern float array1[5];
extern float array2[5];
extern float array3[5];
extern bool received[12];
extern int pwm_values[3];
extern bool pwm;
extern bool connected;
extern int authentication_code[4];
extern bool authenticated;


void init_BLE();
void send_arrays();

#endif