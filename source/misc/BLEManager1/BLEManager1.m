#include "BLEManager.h"



/*
 * Pointer to the Max object
 */
void * this_class;





/************************************ MAIN ***************************************************************/

void ext_main(void * r){
    /*
     * Memory allocation for the object + first function to call for initialization (here: BLE_new)
     */
    setup((t_messlist **) &this_class, (method)BLE_new, 0L, (short)sizeof(BLE), 0L, A_DEFLONG, 0);
    
    
    /*
     * Link functions to events
     * Example: call function "BLE_bang" when a bang is received by the Max object
     * Example: call function "BLE_stop" when "stop" message is received by the Max object
     */
    addbang((method)BLE_bang);
    addmess((method)BLE_start, "start", A_DEFSYM, 0);
    addmess((method)BLE_stop, "stop", A_DEFSYM, 0);
    addmess((method)BLE_interval, "setSampleRate", A_LONG, 0);
    addmess((method)BLE_setOutput, "setOutput", A_GIMME, 0);
    addmess((method)BLE_setAddress, "setAddress", A_LONG, 0);
    addmess((method)BLE_setPWM, "PWM", A_GIMME, 0);
}







/************************************ MAIN FUNCTIONS ****************************************************/



/*
 * First initialization function called when the Max object is created
 */
void * BLE_new(long value){
    /*
     * Creation and allocation of the Max object
     */
    BLE * x;
    x = (BLE *)newobject(this_class);
    
    
    /*
     * Creation of the outlets
     */
    x->ble_accelerometer_output = listout(x);
    x->ble_temperature_output = intout(x);
    x->ble_battery_level_output = intout(x);
    x->ble_connected_output = intout(x);
    x->ble_addr_output = intout(x);
    x->ble_rssi_output = intout(x);
    x->ble_output = listout(x);
    
    
    /*
     * Initialization of the Manager object
     */
    x->ble_manager = [[Manager alloc]init];
    [x->ble_manager manager_new];
    [x->ble_manager manager_setAddress:value];
    
    
    /*
     * Initialization of the clock
     * The function "BLE_bang" will be executed everytime the clock goes off
     */
    x->ble_clock = clock_new(x, (method)BLE_bang);
    x->ble_interval = 100;
    
    return (x);
}




/*
 * Function called everytime a bang is received by the Max object in its left inlet
 * Also executed everytime the clock goes off
 */
void BLE_bang(BLE * x){
    
    /*
     * Reset the clock so that it goes off after x->ble_interval milliseconds
     */
    clock_delay(x->ble_clock, x->ble_interval);
    
    /*
     * Output the values of each outlet
     */
    outlet_list(x->ble_output, NULL, MAX_INPUT, [x->ble_manager manager_getArray]);
    outlet_int(x->ble_rssi_output, [x->ble_manager manager_getRSSI]);
    outlet_int(x->ble_addr_output, [x->ble_manager manager_getAddress]);
    outlet_int(x->ble_connected_output, [x->ble_manager manager_isConnected]);
    outlet_int(x->ble_battery_level_output, [x->ble_manager manager_getBatteryLevel]);
    outlet_list(x->ble_accelerometer_output, NULL, 3, [x->ble_manager manager_getAccelerometer]);
    outlet_int(x->ble_temperature_output, [x->ble_manager manager_getTemperature]);
    
    /*
     * Call the scan function continuously so that new BLE devices can be discovered
     */
    [x->ble_manager manager_scanContinuously];
    
    
    /*
     * Send values continuously to the connected device (if there is a connected device)
     */
    if([x->ble_manager manager_isConnected]){
        [x->ble_manager manager_sendAuthenticationCode];
        [x->ble_manager manager_sendPWM];
        [x->ble_manager manager_sendOutput];
    }
}




/*
 * Function executed when "start" message is received in left inlet of the Max object
 * First call of bang function
 */
void BLE_start(BLE * x){
    BLE_bang(x);
}




/*
 * Function executed when "stop" message is received in left inlet of the Max object
 * Turns off the clock so that the "bang" function is not executed anymore
 */
void BLE_stop(BLE * x){
    [x->ble_manager manager_resetValues];
    
    outlet_list(x->ble_output, NULL, MAX_INPUT, [x->ble_manager manager_getArray]);
    outlet_int(x->ble_rssi_output, [x->ble_manager manager_getRSSI]);
    outlet_int(x->ble_addr_output, [x->ble_manager manager_getAddress]);
    outlet_int(x->ble_connected_output, [x->ble_manager manager_isConnected]);
    outlet_int(x->ble_battery_level_output, [x->ble_manager manager_getBatteryLevel]);
    outlet_list(x->ble_accelerometer_output, NULL, 3, [x->ble_manager manager_getAccelerometer]);
    outlet_int(x->ble_temperature_output, [x->ble_manager manager_getTemperature]);
    
    clock_unset(x->ble_clock);
}




/*
 * Function executed when "setSampleRate" message is received in left inlet of the Max object
 * Updates the interval time of the clock
 */
void BLE_interval(BLE * x, long value){
    if(value>=100){
        x->ble_interval = value;
    }
}




/*
 * Function executed when "setAddress" message is received in left inlet of the Max object
 * Updates the address of the device authorized to be connected
 */
void BLE_setAddress(BLE * x, long addr){
    [x->ble_manager manager_setAddress:addr];
}




/*
 * Function executed when "PWM" message is received in left inlet of the Max object
 * Fills the array "manager_pwm" with the 3 values chosen by the user
 */
void BLE_setPWM(BLE * x, Symbol * s, short ac, Atom * av){
    if(ac>3){
        ac=3;
    }
    for(int i=0; i<ac; i++){
        [x->ble_manager manager_setPWM:i+1 with_value: av[i].a_w.w_long];
    }
}


/*
 * Function executed when "setOutput" message is received in left inlet of the Max object
 * Fills the array "manager_output" with the 12 values chosen by the user
 */
void BLE_setOutput(BLE * x, Symbol * s, short ac, Atom * av){
    if(ac>MAX_OUTPUT-1)
        ac=MAX_OUTPUT-1;
    for(int i=1; i<ac+1; i++){
        [x->ble_manager manager_setOutput:i with_value:av[i-1].a_w.w_long];
    }
}







/************************************ BLE MANAGER OBJECT ******************************************/



@implementation Manager


/*
 * Initialization function for the Manager object
 * Initializes all attributes of the Manager object
 */
- (void) manager_new{
    manager_centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    manager_peripherals = [NSMutableArray new];
    manager_connected = 0;
    manager_battery_level = 0;
    manager_temperature = 0;
    
    
    manager_authentication[0] = 2;
    int tab[4] = AUTHENTICATION_CODE();
    for(int i=0; i<4; i++){
        manager_authentication[i+1] = tab[i];
    }
    
    
    for(int i=0; i<MAX_INPUT; i++){
        atom_setfloat(manager_input+i, 0);
    }
    
    
    for(int i=0; i<MAX_OUTPUT; i++){
        manager_output[i] = 0;
    }
    
    
    manager_pwm[0] = 1;
    for(int i=1; i<4; i++){
        manager_pwm[i] = 0;
    }
    
    
    for(int i=0; i<3; i++){
        atom_setlong(manager_accelerometer+i, 0);
    }
}





/*
 * GET functions
 */
- (t_atom *) manager_getArray{
    return manager_input;
}

- (int) manager_getRSSI{
    return manager_rssi;
}

- (int) manager_getAddress{
    return manager_address;
}

- (bool) manager_isConnected{
    return manager_connected;
}

- (int) manager_getBatteryLevel{
    return manager_battery_level;
}

- (t_atom *) manager_getAccelerometer{
    return manager_accelerometer;
}

- (int) manager_getTemperature{
    return manager_temperature;
}





/*
 * SET functions
 */
- (void) manager_setOutput:(int)index with_value:(bool)value{
    manager_output[index] = value;
}

- (void) manager_setPWM:(int)index with_value:(long)value{
    manager_pwm[index] = value;
}




/*
 * Deconnects the connected peripheral if there is one
 * Reset all attributes of Manager object
 */
- (void) manager_setAddress:(long)addr{
    manager_address = addr;
    Object * isObjectAt0 = [manager_peripherals firstObject];
    if(isObjectAt0 != nil){
        [manager_centralManager cancelPeripheralConnection:manager_peripherals[0]];
        [manager_peripherals removeObjectAtIndex:0];
        [self manager_resetValues];
    }
}





- (void) manager_scanContinuously{
    [manager_centralManager scanForPeripheralsWithServices:nil options:nil];
}





/*
 * Reset all attributes of Manager object
 */
- (void) manager_resetValues{
    for(int i=0; i<MAX_INPUT; i++){
        atom_setfloat(manager_input+i, 0);
    }
    for(int i=0; i<3; i++){
        atom_setlong(manager_accelerometer+i, 0);
    }
    manager_connected = 0;
    manager_rssi = 0;
    manager_battery_level = 0;
    manager_temperature = 0;
}





/*
 * Send "manager_pwm" array to the write characteristic of the connected device
 * Write characteristic for Simblee : 2222
 * Write characteristic for Bean : a495ff23-c5b1-4b44-b512-1370f02d74de
 */
- (void) manager_sendPWM{
    CBPeripheral * periph = manager_peripherals[0];
    for(CBService * service in periph.services){
        for(CBCharacteristic * c in service.characteristics){
            if([c.UUID isEqual:[CBUUID UUIDWithString:@"2222"]] || [c.UUID isEqual:[CBUUID UUIDWithString:@"a495ff23-c5b1-4b44-b512-1370f02d74de"]]){
                NSData * data = [NSData dataWithBytes:&manager_pwm length:sizeof(manager_pwm)];
                [periph writeValue:data forCharacteristic:c type:CBCharacteristicWriteWithoutResponse];
            }
        }
    }
}




/*
 * Send "manager_output" array to the write characteristic of the connected device
 * Write characteristic for Simblee : 2222
 * Write characteristic for Bean : a495ff22-c5b1-4b44-b512-1370f02d74de
 */
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




/*
 * Send "manager_authentication" array to the write characteristic of the connected device
 * Write characteristic for Simblee : 2224
 * Write characteristic for Bean : a495ff22-c5b1-4b44-b512-1370f02d74de
 */
- (void) manager_sendAuthenticationCode{
    CBPeripheral * periph = manager_peripherals[0];
    for(CBService * service in periph.services){
        for(CBCharacteristic * c in service.characteristics){
            if([c.UUID isEqual:[CBUUID UUIDWithString:@"2222"]] || [c.UUID isEqual:[CBUUID UUIDWithString:@"a495ff24-c5b1-4b44-b512-1370f02d74de"]]){
                NSData * data = [NSData dataWithBytes:&manager_authentication length:sizeof(manager_authentication)];
                [periph writeValue:data forCharacteristic:c type:CBCharacteristicWriteWithoutResponse];
            }
        }
    }
}








/************************************ CORE BLUETOOTH Delegate Methods for central manager **************************/


/*
 * Start scanning for peripherals when Manager object created successfully
 */
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




/*
 * Executed when a peripheral has been discovered by the scan function
 * Connects to the peripheral if it has the right address (the peripheral must be advertising its address)
 */
- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary *)advertisementData RSSI:(NSNumber *)RSSI{
    
    if([peripheral.name isEqualToString:[NSString stringWithFormat:@"%d", manager_address]]){
        [manager_peripherals insertObject:peripheral atIndex:0];
        [manager_centralManager connectPeripheral:peripheral options:nil];
    }
}




/*
 * Executed if the connection to the peripheral has been successfull
 * Discovers all services offered by the connected peripheral
 */
- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    [peripheral setDelegate:self];
    [peripheral discoverServices:nil];
    manager_connected = 1;
}




/*
 * Executed if the connection to the peripheral has failed
 */
- (void)centralManager:(CBCentralManager *)central didFailToConnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    [self  manager_resetValues];
    manager_connected = 0;
}




/*
 * Executed if the connection with the peripheral has expired
 */
- (void)centralManager:(CBCentralManager *)central didDisconnectPeripheral:(CBPeripheral *)peripheral error:(NSError *)error{
    [self  manager_resetValues];
     manager_connected = 0;
}




/*
 * Executed if a service offered by the peripheral has been discovered
 * Discovers custom read and write characteristics of this service
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error{
    for(CBService * service in peripheral.services){
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2221"]] forService:service]; //read simblee
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"2222"]] forService:service]; //write simblee
        
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"a495ff21-c5b1-4b44-b512-1370f02d74de"]] forService:service]; // read bean
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"a495ff22-c5b1-4b44-b512-1370f02d74de"]] forService:service]; // write bean output
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"a495ff23-c5b1-4b44-b512-1370f02d74de"]] forService:service]; // write bean pwm
        [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"a495ff24-c5b1-4b44-b512-1370f02d74de"]] forService:service]; // write bean authentication code
    }
}




/*
 * Executed when a charcateristic has been discovered 
 * Subscribes to the characteristic
 */
- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error{
    for(CBCharacteristic * characteristic in service.characteristics){
        [peripheral setNotifyValue:YES forCharacteristic:characteristic];
    }
}




/*
 * Executed when a charcateristic has been updated by the peripheral and when the central manager has subscribed to this characteristic
 * Reads the values of the characteristic and stores them in "manager_input" array
 */
- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error{
    
    // battery level array indexed by 0, contains 1 value
    if((*(float *)([characteristic.value bytes])) == 0){
        manager_battery_level = *((float *)([characteristic.value bytes])+1);
    }
    
    // accelerometer array indexed by 1, contains 3 values
    if((*(float *)([characteristic.value bytes])) == 1){
        for(int i=0; i<3; i++){
            atom_setlong(manager_accelerometer+i, *(((float *)([characteristic.value bytes]))+i+1));
        }
    }
    
    // temperature array indexed by 2, contains 1 value
    if((*(float *)([characteristic.value bytes])) == 2){
        manager_temperature = *((float *)([characteristic.value bytes])+1);
    }
    
    // inputs array indexed by 3, contains 4 values
    if((*(float *)([characteristic.value bytes])) == 3){
        for(int i=1; i<=(MAX_INPUT/3); i++){
            int value = *(((float *)([characteristic.value bytes]))+i);
            atom_setfloat(manager_input+i-1, value);
        }
    }
    
    // inputs array indexed by 4, contains 4 values
    if((*(float *)([characteristic.value bytes])) == 4){
        for(int i=1; i<=(MAX_INPUT/3); i++){
            int value = *(((float *)([characteristic.value bytes]))+i);
            atom_setfloat(manager_input+(MAX_INPUT/3)+i-1, value);
        }
    }
    
    // inputs array indexed by 5, contains 4 values
    if((*(float *)([characteristic.value bytes])) == 5){
        for(int i=1; i<=(MAX_INPUT/3); i++){
            int value = *(((float *)([characteristic.value bytes]))+i);
            atom_setfloat(manager_input+2*(MAX_INPUT/3)+i-1, value);
        }
    }
    
    // retrieve RSSI value of peripheral
    [peripheral readRSSI];
    manager_rssi = [peripheral.RSSI intValue];
}

@end



