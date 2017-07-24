#ifndef BLEManager_h
#define BLEManager_h

#import <CoreBluetooth/CoreBluetooth.h>


struct BLE;
typedef struct BLE BLE;

/************************************ OBJECT DECLARATION ********************************************/

@interface Manager : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>{
    CBCentralManager * manager_centralManager;
    NSMutableArray * manager_peripherals;
    t_atom manager_array[MAX_PIN]; //stores pin values (sent by peripheral)
    int manager_output[MAX_PIN]; //stores user values (to be sent to peripheral)
}

- (void) manager_new;
- (t_atom *) manager_getArray;
- (void) manager_setOutput:(int)index with_value:(long)value;
- (void) manager_sendOutput:(BLE *)x;

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
void BLE_inlet(BLE * x, long value);
void BLE_start(BLE * x);
void BLE_stop(BLE * x);
void BLE_interval(BLE * x, long value);
void BLE_setOutput(BLE * x, Symbol * s, short ac, Atom * av);




#endif