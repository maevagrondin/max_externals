#include "BLEManager_Simblee.h"


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