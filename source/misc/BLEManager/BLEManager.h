#ifndef BLEManager_h
#define BLEManager_h

#import <CoreBluetooth/CoreBluetooth.h>




/************************************ OBJECT DECLARATION ********************************************/

@interface Manager : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>{
    long manager_values[MAX_SIZE];
    long manager_count;
    long useless;
    CBCentralManager * manager_centralManager;
    NSMutableArray * manager_peripherals;
}

- (void) manager_new;
- (long) manager_bang;

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


#endif