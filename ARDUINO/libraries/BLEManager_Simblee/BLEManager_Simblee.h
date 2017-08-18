#ifndef BLEMANAGER_SIMBLEE_H
#define BLEMANAGER_SIMBLEE_H

extern float array1[5];
extern float array2[5];
extern float array3[5];
extern bool received[12];
extern int pwm_values[3];
extern bool pwm;
extern bool connected;


void init_BLE();
void send_arrays();

#endif
