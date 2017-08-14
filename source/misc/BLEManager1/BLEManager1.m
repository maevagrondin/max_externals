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
    addmess((method)BLE_setOutput, "setOutput", A_GIMME, 0);
    addmess((method)BLE_setAddress, "setAddress", A_LONG, 0);
    addmess((method)BLE_setPWM, "PWM", A_GIMME, 0);
}




/************************************ IMPLEMENTATION ************************************************/

void * BLE_new(long value){
    // create object
    BLE * x;
    x = (BLE *)newobject(this_class);
    x->ble_manager = [[Manager alloc]init];
    
    // create the outlets
    x->ble_connected_output = intout(x);
    x->ble_addr_output = intout(x);
    x->ble_rssi_output = intout(x);
    x->ble_output = listout(x);
    
    [x->ble_manager manager_new];
    x->ble_clock = clock_new(x, (method)BLE_bang);
    x->ble_interval = 100;
    
    [x->ble_manager manager_setAddress:value];
    
    return (x);
}


void BLE_bang(BLE * x){
    clock_delay(x->ble_clock, x->ble_interval);
    outlet_list(x->ble_output, NULL, MAX_INPUT, [x->ble_manager manager_getArray]);
    outlet_int(x->ble_rssi_output, [x->ble_manager manager_getRSSI]);
    outlet_int(x->ble_addr_output, [x->ble_manager manager_getAddress]);
    outlet_int(x->ble_connected_output, [x->ble_manager manager_isConnected]);
    [x->ble_manager manager_scanContinuously];
    if([x->ble_manager manager_isConnected]){
        [x->ble_manager manager_sendOutput];
        [x->ble_manager manager_sendPWM];
    }
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

void BLE_setAddress(BLE * x, long addr){
    [x->ble_manager manager_setAddress:addr];
}

void BLE_setPWM(BLE * x, Symbol * s, short ac, Atom * av){
    if(ac>3){
        ac=3;
    }
    for(int i=0; i<ac; i++){
        [x->ble_manager manager_setPWM:i+1 with_value: av[i].a_w.w_long];
    }
}

void BLE_setOutput(BLE * x, Symbol * s, short ac, Atom * av){
    if(ac>MAX_OUTPUT-1)
        ac=MAX_OUTPUT-1;
    for(int i=1; i<ac+1; i++){
        [x->ble_manager manager_setOutput:i with_value:av[i].a_w.w_long];
    }
}


/************************************ IMPLEMENTATION OBJET ******************************************/

@implementation Manager


- (void) manager_new{
    manager_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    manager_peripherals = [NSMutableArray new];
    
    for(int i=0; i<MAX_INPUT; i++){
        atom_setfloat(manager_input+i, 0);
    }
    for(int i=0; i<MAX_OUTPUT; i++){
        manager_output[i] = 0;
    }
    for(int i=1; i<4; i++){
        manager_pwm[i] = 0;
    }
    manager_pwm[0] = 1;
    manager_connected = 0;
}


- (t_atom *) manager_getArray{
    return manager_input;
}

- (bool) manager_isConnected{
    return manager_connected;
}


- (void) manager_setOutput:(int)index with_value:(bool)value{
    manager_output[index] = value;
}

- (void) manager_setAddress:(long)addr{
    manager_address = addr;
    Object * isObjectAt0 = [manager_peripherals firstObject];
    if(isObjectAt0 != nil){
        [manager_centralManager cancelPeripheralConnection:manager_peripherals[0]];
        [manager_peripherals removeObjectAtIndex:0];
        [self manager_resetValues];
    }
}

- (int) manager_getRSSI{
    return manager_rssi;
}

- (int) manager_getAddress{
    return manager_address;
}

- (void) manager_scanContinuously{
    [manager_centralManager scanForPeripheralsWithServices:nil options:nil];
}

- (void) manager_resetValues{
    for(int i=0; i<MAX_INPUT; i++){
        atom_setfloat(manager_input+i, 0);
    }
    manager_connected = 0;
    manager_rssi = 0;
}

- (void) manager_setPWM:(int)index with_value:(long)value{
    manager_pwm[index] = value;
}

- (void) manager_sendPWM{
    CBPeripheral * periph = manager_peripherals[0];
    for(CBService * service in periph.services){
        for(CBCharacteristic * c in service.characteristics){
            if([c.UUID isEqual:[CBUUID UUIDWithString:@"2222"]] || [c.UUID isEqual:[CBUUID UUIDWithString:@"a495ff22-c5b1-4b44-b512-1370f02d74de"]]){
                NSData * data = [NSData dataWithBytes:&manager_pwm length:sizeof(manager_pwm)];
                [periph writeValue:data forCharacteristic:c type:CBCharacteristicWriteWithoutResponse];
            }
        }
    }
}


- (void) manager_sendOutput{
    CBPeripheral * periph = manager_peripherals[0];
    for(CBService * service in periph.services){
        for(CBCharacteristic * c in service.characteristics){
            if([c.UUID isEqual:[CBUUID UUIDWithString:@"2222"]] || [c.UUID isEqual:[CBUUID UUIDWithString:@"a495ff22-c5b1-4b44-b512-1370f02d74de"]]){
                NSData * data = [NSData dataWithBytes:&manager_output length:sizeof(manager_output)];
                [periph writeValue:data forCharacteristic:c type:CBCharacteristicWriteWithoutResponse];
            }
        }
    }
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
    
    if([peripheral.name isEqualToString:[NSString stringWithFormat:@"%d", manager_address]]){
        [manager_peripherals insertObject:peripheral atIndex:0];
        [manager_centralManager connectPeripheral:peripheral options:nil];
    }
}


- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    [peripheral setDelegate:self];
    [peripheral discoverServices:nil];
    manager_connected = 1;
}


- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    for(int i=0; i<MAX_INPUT; i++){
        atom_setfloat(manager_input+i, 0);
    }
    manager_connected = 0;
}


- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
     manager_connected = 0;
    //[manager_centralManager connectPeripheral:peripheral options:nil];
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    for(CBService * service in peripheral.services){
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2221"]] forService:service]; //read
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2222"]] forService:service]; //write
        
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"a495ff21-c5b1-4b44-b512-1370f02d74de"]] forService:service]; // read bean
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"a495ff22-c5b1-4b44-b512-1370f02d74de"]] forService:service]; // write bean
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    for(CBCharacteristic * characteristic in service.characteristics){
        [peripheral setNotifyValue:YES forCharacteristic:characteristic];
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error{
    if((*(float *)([characteristic.value bytes])) == 1){
        for(int i=1; i<=(MAX_INPUT/2); i++){
            int value = *(((float *)([characteristic.value bytes]))+i);
            atom_setfloat(manager_input+i-1, value);
        }
    }
    if((*(float *)([characteristic.value bytes])) == 2){
        for(int i=1; i<=(MAX_INPUT/2); i++){
            int value = *(((float *)([characteristic.value bytes]))+i);
            atom_setfloat(manager_input+(MAX_INPUT/2)+i-1, value);
        }
    }
    [peripheral readRSSI];
    manager_rssi = [peripheral.RSSI intValue];
}

@end



