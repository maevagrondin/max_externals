#ifndef BLEManager_h
#define BLEManager_h

#import <CoreBluetooth/CoreBluetooth.h>

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
    
    int manager_connected;
    int manager_rssi;
}

- (void) manager_new;
- (void) manager_scanContinuously;
- (void) manager_resetValues;

// GET functions
- (t_atom *) manager_getArray;
- (int) manager_getAddress;
- (int) manager_getRSSI;
- (bool) manager_isConnected;

// SET functions
- (void) manager_setAddress:(long)addr;
- (void) manager_setPWM:(int)index with_value:(long)value;
- (void) manager_setOutput:(int)index with_value:(bool)value;

// SEND functions
- (void) manager_sendOutput;
- (void) manager_sendPWM;


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