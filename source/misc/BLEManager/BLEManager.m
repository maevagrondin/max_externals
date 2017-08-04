#include "ext.h"
#include "BLEManager.h"
#include <objc/runtime.h>
#include <Foundation/Foundation.h>
#include <string.h>

void * this_class;


/************************************ MAIN **********************************************************/

void ext_main(void * r){
    setup((t_messlist **) &this_class, (method)BLE_new, 0L, (short)sizeof(BLE), 0L, A_DEFLONG, 0);
    addbang((method)BLE_bang);
    addmess((method)BLE_start, "start", A_DEFSYM, 0);
    addmess((method)BLE_stop, "stop", A_DEFSYM, 0);
    addmess((method)BLE_interval, "setSampleRate", A_LONG, 0);
    //addmess((method)BLE_setOutput, "setOutput", A_GIMME, 0);
}




/************************************ IMPLEMENTATION ************************************************/

void * BLE_new(long value){
    BLE * x;
    x = (BLE *)newobject(this_class);
    x->ble_manager = [[Manager alloc]init];
    x->ble_output = listout(x);
    [x->ble_manager manager_new];
    x->ble_clock = clock_new(x, (method)BLE_bang);
    x->ble_interval = 100;
    return (x);
}


void BLE_bang(BLE * x){
    clock_delay(x->ble_clock, x->ble_interval);
    for(int i=0; i<MAX_PERIPHERAL; i++){
        outlet_list(x->ble_output, NULL, MAX_PIN+1, [x->ble_manager manager_getArray:i]);
    }
    /*if([x->ble_manager manager_isConnected])
        [x->ble_manager manager_sendOutput];*/
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

/*void BLE_setOutput(BLE * x, Symbol * s, short ac, Atom * av){
    if(ac>MAX_OUTPUT)
        ac=MAX_OUTPUT;
    for(int i=0; i<ac; i++){
        [x->ble_manager manager_setOutput:i with_value:av[i].a_w.w_long];
    }
}*/


/************************************ IMPLEMENTATION OBJET ******************************************/

@implementation Manager


- (void) manager_new{
    manager_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    manager_peripherals = [NSMutableArray new];
    manager_uuid_to_index = [[NSMutableDictionary alloc] initWithCapacity:MAX_PERIPHERAL];
    manager_nb_peripherals = 0;
    
    for(int i=0; i<MAX_PERIPHERAL; i++){
        for(int j=0; j<MAX_PIN; j++){
            atom_setfloat((t_atom *)manager_input+(i*MAX_PIN)+j, 0);
        }
    }
    for(int i=0; i<MAX_PERIPHERAL; i++){
        for(int j=0; j<MAX_OUTPUT; j++){
            manager_output[i][j] = 0;
        }
    }
    
    for(int i=0; i<MAX_PIN; i++){
        atom_setfloat(manager_return+i, 0);
    }
}


- (t_atom *) manager_getArray:(int)index{
    atom_setfloat(manager_return, index);
    for(int i=0; i<MAX_PIN; i++){
        manager_return[i+1] = manager_input[index][i];
    }
    return manager_return;
}



/*- (bool) manager_isConnected{
    return manager_connected;
}*/


/*- (void) manager_setOutput:(int)index with_value:(bool)value{
    manager_output[index] = value;
}*/

/*- (void) manager_sendOutput{
    CBPeripheral * periph = manager_peripherals[0];
    for(CBService * service in periph.services){
        for(CBCharacteristic * c in service.characteristics){
            if([c.UUID isEqual:[CBUUID UUIDWithString:@"2222"]]){
                NSData * data = [NSData dataWithBytes:&manager_output length:sizeof(manager_output)];
                [periph writeValue:data forCharacteristic:c type:CBCharacteristicWriteWithoutResponse];
            }
        }
    }
}*/



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
    
    NSString * uuid = [peripheral.identifier UUIDString];
    int first = 0;
    
    if([manager_uuid_to_index objectForKey:uuid] == nil){
        first = 1;
    }
    
    if([peripheral.name isEqualToString:[NSString stringWithUTF8String:"LILYPAD"]] && first){
        [manager_peripherals addObject:peripheral];
        
        [manager_uuid_to_index setObject:[NSNumber numberWithInt:manager_nb_peripherals] forKey:uuid];
        manager_nb_peripherals++;
        [manager_centralManager connectPeripheral:peripheral options:nil];
    }
}


- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    [peripheral setDelegate:self];
    [peripheral discoverServices:nil];
    //manager_connected = 1;
}


- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
}


- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
     //manager_connected = 0;
    [manager_centralManager connectPeripheral:peripheral options:nil];
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    for(CBService * service in peripheral.services){
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2221"]] forService:service]; //read
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2222"]] forService:service]; //write
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    for(CBCharacteristic * characteristic in service.characteristics){
        [peripheral setNotifyValue:YES forCharacteristic:characteristic];
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error{
    NSString * uuid = [peripheral.identifier UUIDString];
    int index = [[manager_uuid_to_index objectForKey:uuid] intValue];
    
    if((*(float *)([characteristic.value bytes])) == 1){
        for(int i=1; i<=(MAX_PIN/2+1); i++){
            int value = *(((float *)([characteristic.value bytes]))+i);
            atom_setfloat((t_atom *)manager_input+(index*MAX_PIN)+i-1, value);
        }
    }
    if((*(float *)([characteristic.value bytes])) == 2){
        for(int i=1; i<=(MAX_PIN/2); i++){
            int value = *(((float *)([characteristic.value bytes]))+i);
            atom_setfloat((t_atom *)manager_input+(index*MAX_PIN)+(MAX_PIN/2+1)+i-1, value);
        }
    }
}

@end



