#ifndef BLEManager_h
#define BLEManager_h

#import <CoreBluetooth/CoreBluetooth.h>

#define MAX_PIN 7
#define MAX_OUTPUT 8
#define MAX_PERIPHERAL 2


/************************************ OBJECT DECLARATION ********************************************/

@interface Manager : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>{
    CBCentralManager * manager_centralManager;
    NSMutableArray * manager_peripherals;
    t_atom manager_input[MAX_PERIPHERAL][MAX_PIN]; //stores pin values (sent by peripheral)
    bool manager_output[MAX_PERIPHERAL][MAX_OUTPUT]; //stores user values (to be sent to peripheral)
    //bool manager_connected[MAX_PERIPHERAL];
    NSMutableDictionary * manager_uuid_to_index;
    t_atom manager_return[MAX_PIN+1];
    int manager_nb_peripherals;
}

- (void) manager_new;
- (t_atom *) manager_getArray:(int)index;
//- (void) manager_setOutput:(int)index with_value:(bool)value;
//- (void) manager_sendOutput;
//- (bool) manager_isConnected;

@end



/************************************ MAIN STRUCTURE *************************************************/

typedef struct BLE{
    t_object ble_object;
    Manager * ble_manager;
    void * ble_output;
    void * ble_clock;
    long ble_interval;
} BLE;



/************************************ FUNCTION DECLARATION *******************************************/
void * BLE_new(long value);
void BLE_bang(BLE * x);
void BLE_start(BLE * x);
void BLE_stop(BLE * x);
void BLE_interval(BLE * x, long value);
//void BLE_setOutput(BLE * x, Symbol * s, short ac, Atom * av);




#endif