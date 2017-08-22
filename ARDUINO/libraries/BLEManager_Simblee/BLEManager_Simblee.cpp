#include "BLEManager_Simblee.h"
#include <SimbleeBLE.h>


// max of 20 bytes accepted (5x1 int of 4 bytes)
float temperature[2] = {2,0};
float array1[5] = {3,0,0,0,0};
float array2[5] = {4,0,0,0,0};
float array3[5] = {5,0,0,0,0};

bool received[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int pwm_values[3] = {0,0,0};

bool pwm = 0;
bool connected = 0;



/*************************************************************************************************
 * LIB : initialisation
 *************************************************************************************************/
void init_BLE(){
    char res[1024];
    sprintf(res, "%d", SimbleeCOM.getESN());
    SimbleeBLE.deviceName = res;
    SimbleeBLE.advertisementInterval = 20;
    SimbleeBLE.txPowerLevel = -4;
    SimbleeBLE.customUUID = "2220";
    SimbleeBLE.begin();
}



/*************************************************************************************************
 * LIB: send array1, array2 and array3 to computer
 *************************************************************************************************/
void send_arrays(){
    if(connected){
        temperature[0] = 2;
        array1[0] = 3;
        array2[0] = 4;
        array3[0] = 5;
        temperature[1] = Simblee_temperature(CELSIUS);
        SimbleeBLE.send((char *)temperature, sizeof(temperature));
        SimbleeBLE.send((char *)array1, sizeof(array1));
        SimbleeBLE.send((char *)array2, sizeof(array2));
        SimbleeBLE.send((char *)array3, sizeof(array3));
        SimbleeBLE.connectable = false;
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
        if(pwm_values[0] == 0 && pwm_values[1] == 0 && pwm_values[2] == 0){
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