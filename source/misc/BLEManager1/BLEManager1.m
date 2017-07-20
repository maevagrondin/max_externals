#include "ext.h"
#define MAX_SIZE 1024
#include "BLEManager.h"
#include <objc/runtime.h>
#include <Foundation/Foundation.h>
#include <string.h>

void * this_class;


/************************************ MAIN **********************************************************/

void ext_main(void * r){
    setup((t_messlist **) &this_class, (method)BLE_new, 0L, (short)sizeof(BLE), 0L, A_DEFLONG, 0);
    addbang((method)BLE_bang);
    //addint((method)BLE_inlet);
    addmess((method)BLE_start, "start", A_DEFSYM, 0);
    addmess((method)BLE_stop, "stop", A_DEFSYM, 0);
    addmess((method)BLE_interval, "setSampleRate", A_LONG, 0);
}




/************************************ IMPLEMENTATION ************************************************/

void * BLE_new(long value){
    // create object
    BLE * x;
    x = (BLE *)newobject(this_class);
    x->ble_manager = [[Manager alloc]init];
    
    // create the outlet
    x->ble_output = listout(x);
    
    [x->ble_manager manager_new];
    x->ble_clock = clock_new(x, (method)BLE_bang);
    x->ble_interval = 1000;
    
    long my_array[3];
    
    my_array[0] = 42;
    my_array[1] = 674;
    my_array[2] = 8;
    
    for(int i=0; i<3; i++){
        atom_setlong(x->ble_array+i, my_array[i]);
    }
    
    
    return (x);
}



//TODO
void BLE_bang(BLE * x){
    clock_delay(x->ble_clock, x->ble_interval);
    long res = [x->ble_manager manager_bang];
    outlet_list(x->ble_output, NULL, 3, x->ble_array);
}


void BLE_inlet(BLE * x, long value){
    BLE_bang(x);
}

void BLE_start(BLE * x){
    BLE_bang(x);
}

void BLE_stop(BLE * x){
    clock_unset(x->ble_clock);
}

void BLE_interval(BLE * x, long value){
    x->ble_interval = value;
}




/************************************ IMPLEMENTATION OBJET ******************************************/

@implementation Manager


// init the object
- (void) manager_new{
    manager_count = 0;
    manager_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    manager_peripherals = [NSMutableArray new];
    manager_res = 0;
}


// calculate and set the output
- (long) manager_bang{
    return manager_res;
}

- (long)manager_getCount{
    return manager_count;
}



/************************************ Delegate Methods for central manager **************************/

- (void)centralManagerDidUpdateState:(CBCentralManager *)central
{
    switch (central.state) {
        case CBCentralManagerStatePoweredOff:
            NSLog(@"CoreBluetooth BLE hardware is powered off");
            break;
        case CBCentralManagerStatePoweredOn:
            NSLog(@"CoreBluetooth BLE hardware is powered on and ready");
            [manager_centralManager scanForPeripheralsWithServices:nil options:nil];
            break;
        case CBCentralManagerStateResetting:
            NSLog(@"CoreBluetooth BLE hardware is resetting");
            break;
        case CBCentralManagerStateUnauthorized:
            NSLog(@"CoreBluetooth BLE state is unauthorized");
            break;
        case CBCentralManagerStateUnknown:
            NSLog(@"CoreBluetooth BLE state is unknown");
            break;
        case CBCentralManagerStateUnsupported:
            NSLog(@"CoreBluetooth BLE hardware is unsupported on this platform");
            break;
        default:
            break;
    }
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI{
    if([peripheral.name isEqualToString:[NSString stringWithUTF8String:"LILYPAD"]]){
        [manager_peripherals addObject:peripheral];
        manager_count++;
        [manager_centralManager connectPeripheral:peripheral options:nil];
        [manager_centralManager stopScan];
    }
}



- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    [peripheral setDelegate:self];
    [peripheral discoverServices:@[[CBUUID UUIDWithString:@"FE84"]]];
}


- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    for(CBService * service in peripheral.services){
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2221"]] forService:service];
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    for(CBCharacteristic * characteristic in service.characteristics){
        [peripheral setNotifyValue:YES forCharacteristic:characteristic];
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error{
    manager_res = *(int *)([characteristic.value bytes]);
}

@end


