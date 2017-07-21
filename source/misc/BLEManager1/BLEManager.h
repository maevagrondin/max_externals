#ifndef BLEManager_h
#define BLEManager_h

#import <CoreBluetooth/CoreBluetooth.h>




/************************************ OBJECT DECLARATION ********************************************/

@interface Manager : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>{
    long manager_values[MAX_SIZE];
    long manager_count;
    long manager_res;
    CBCentralManager * manager_centralManager;
    NSMutableArray * manager_peripherals;
    t_atom manager_array[MAX_PIN];
}

- (void) manager_new;
- (long) manager_bang;
- (long) manager_getCount;
- (t_atom *) manager_getArray;

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


#endif