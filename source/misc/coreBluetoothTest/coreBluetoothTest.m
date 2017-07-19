#include "ext.h"
#define MAX_SIZE 1024
#include "coreBluetoothTest.h"
#include <objc/runtime.h>
#include <Foundation/Foundation.h>
#include <string.h>

void * this_class;


/************************************ MAIN **********************************************************/

void ext_main(void * r){
    setup((t_messlist **) &this_class, (method)minOrMaxObjC_new, 0L, (short)sizeof(MinOrMaxObjC), 0L, A_DEFLONG, 0);
    addmess((method)minOrMaxObjC_list, "list", A_GIMME, 0);
    addinx((method)minOrMaxObjC_in1, 1);
    addbang((method)minOrMaxObjC_bang);
}




/************************************ IMPLEMENTATION ************************************************/

void * minOrMaxObjC_new(long value){
    // create object
    MinOrMaxObjC * x;
    x = (MinOrMaxObjC *)newobject(this_class);
    x->am_objc = [[MinMax alloc]init];
    
    intin(x,1); // create the right inlet
    x->struct_out = intout(x); // create the outlet
    
    [x->am_objc am_new:value];
    return (x);
}


void minOrMaxObjC_list(MinOrMaxObjC * x, Symbol * s, short ac, Atom * av){
    [x->am_objc am_list:av andLength:ac];
    minOrMaxObjC_bang(x);
}


void minOrMaxObjC_in1(MinOrMaxObjC * x, long value){
    [x->am_objc am_in1:value];
}


void minOrMaxObjC_bang(MinOrMaxObjC * x){
    [x->am_objc am_bang];
    outlet_int(x->struct_out, [x->am_objc am_getRes]);
}





/************************************ IMPLEMENTATION OBJET ******************************************/

@implementation MinMax


// init the object
- (void) am_new: (long) value{
    am_valright = value;
    am_count = 0;
    am_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    am_peripherals = [NSMutableArray new];
}



// store the left inlet list
- (void) am_list:(Atom *)av andLength:(short)ac{
    if(ac >= MAX_SIZE)
        ac = MAX_SIZE;
    for(int i=0; i<ac; i++){
        am_list[i] = av[i].a_w.w_long;
    }
    am_count = ac;
}



// store the right inlet number
- (void) am_in1:(long)value{
    am_valright = value;
}



// calculate and set the output
- (void) am_bang{
    long res = am_list[0];
    for(int i=0; i<am_count-1; i++){
        res = [self am_calculate:res andV2:am_list[i+1]];
    }
    am_res = res;
}



// calculate help
- (long) am_calculate:(long)v1 andV2:(long) v2{
    if(am_valright < 0){
        if(v1 < v2)
            return v1;
        else
            return v2;
    }
    else{
        if(v1 > v2)
            return v1;
        else
            return v2;
    }
}


// access the output
- (long) am_getRes{
    // return am_res;
    return am_cb;
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
            [am_centralManager scanForPeripheralsWithServices:nil options:nil];
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
    NSLog(@"Did discover peripheral: %@", peripheral);
    am_cb = 42;
    if([peripheral.name isEqualToString:[NSString stringWithUTF8String:"LILYPAD"]]){
        [am_peripherals addObject:peripheral];
        [am_centralManager connectPeripheral:peripheral options:nil];
        am_cb = 123;
        [am_centralManager stopScan];
    }
}



- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    NSLog(@"Connection successfull to peripheral: %@",peripheral);
    am_cb = 85;
    [peripheral setDelegate:self];
    [peripheral discoverServices:@[[CBUUID UUIDWithString:@"FE84"]]];
}


- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    NSLog(@"Connection failed to peripheral: %@",peripheral);
    am_cb = 53;
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    am_cb = 987;
    for(CBService * service in peripheral.services){
        am_cb = 14;
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2221"]] forService:service];
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    for(CBCharacteristic * characteristic in service.characteristics){
        am_cb = 7452;
        [peripheral setNotifyValue:YES forCharacteristic:characteristic];
        
    }
}


- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error{
    am_cb = *(int *)([characteristic.value bytes]);
}

@end



