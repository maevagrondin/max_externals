#ifndef BLEMANAGER_BEAN_H
#define BLEMANAGER_BEAN_H

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


void init_BLE(int addr);
void send_arrays();

#endif

