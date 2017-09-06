#ifndef BLEManager_h
#define BLEManager_h


#import <CoreBluetooth/CoreBluetooth.h>
#include <objc/runtime.h>
#include <Foundation/Foundation.h>
#include <string.h>
#include "ext.h"


#define AUTH_CODE() {9,7,4,8}


#define MAX_INPUT 12
#define MAX_OUTPUT 13



/************************************ OBJECT DECLARATION ********************************************/

@interface Manager : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>{
    
    // Instance of the Central Manager
    CBCentralManager * manager_centralManager;
    
    // Stores the connected peripheral
    NSMutableArray * manager_peripherals;
    
    // Stores pin values (sent by peripheral)
    t_atom manager_input[MAX_INPUT];
    
    // Stores user values (to be sent to peripheral)
    // First element always equals 0 (manager_output[0]==0)
    bool manager_output[MAX_OUTPUT];
    
    // Stores PWM values set by user (to be sent to peripheral)
    // First element always equals 1 (manager_pwm[0]==1)
    int manager_pwm[4];
    
    // Address of peripheral authorised to connect to central
    int manager_address;
    
    // Authentication code (to be sent to peripherals in order to be authorized to receive their values)
    // First element always equals 2 (manager_authentification[0]==2)
    int manager_authentication[5];
    
    // Other values received from the peripheral
    int manager_connected;
    int manager_rssi;
    t_atom manager_accelerometer[3];
    int manager_battery_level;
    int manager_temperature;
}




- (void) manager_new;
- (void) manager_scanContinuously;
- (void) manager_resetValues;

// GET functions
- (t_atom *) manager_getArray;
- (int) manager_getAddress;
- (int) manager_getRSSI;
- (bool) manager_isConnected;
- (int) manager_getBatteryLevel;
- (int) manager_getTemperature;
- (t_atom *) manager_getAccelerometer;

// SET functions
- (void) manager_setAddress:(long)addr;
- (void) manager_setPWM:(int)index with_value:(long)value;
- (void) manager_setOutput:(int)index with_value:(bool)value;

// SEND functions
- (void) manager_sendOutput;
- (void) manager_sendPWM;
- (void) manager_sendAuthenticationCode;


@end



/************************************ MAIN STRUCTURE *************************************************/

typedef struct BLE{
    
    // instance of the Max object
    t_object ble_object;
    
    // pointer to the Manager object
    Manager * ble_manager;
    
    // pointers to the outlets of the Max object
    void * ble_output;
    void * ble_rssi_output;
    void * ble_addr_output;
    void * ble_connected_output;
    void * ble_accelerometer_output;
    void * ble_battery_level_output;
    void * ble_temperature_output;
    
    // clock information
    void * ble_clock;
    long ble_interval;
} BLE;



/************************************ MAIN FUNCTIONS DECLARATION **************************************/

void * BLE_new(long value);
void BLE_bang(BLE * x);
void BLE_start(BLE * x);
void BLE_stop(BLE * x);
void BLE_interval(BLE * x, long value);
void BLE_setAddress(BLE * x, long addr);
void BLE_setOutput(BLE * x, Symbol * s, short ac, Atom * av);
void BLE_setPWM(BLE * x, Symbol * s, short ac, Atom * av);


#endif