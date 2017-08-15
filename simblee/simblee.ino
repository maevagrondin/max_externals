// LIB : init
#include <SimbleeBLE.h>
// max of 20 bytes accepted (5x1 int of 4 bytes)
float array1[5] = {1,1,2,3,4};
float array2[5] = {2,5,6,7,8};
float array3[5] = {3,9,10,11,12};
bool received[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int pwm_values[3] = {0,0,0};
int pwm = 0;
int connected = 0;


// USER : variables
const int LED = 13;
const int pause = 1000;


void setup()
{
  // USER : init pins
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  pinMode(3, INPUT);

  // LIB : init
  char res[1024];
  sprintf(res, "%d", SimbleeCOM.getESN());
  SimbleeBLE.deviceName = res;
  SimbleeBLE.advertisementInterval = 20;
  SimbleeBLE.txPowerLevel = -4;
  SimbleeBLE.customUUID = "2220";
  Serial.begin(9600);
  SimbleeBLE.begin();
}

void loop()
{
  // USER : set values to send to computer
  array1[1] = analogRead(3);

  // TO BE DELETED : blink when not connected
  if(!connected){
    digitalWrite(LED, HIGH);
    delay(pause);
    digitalWrite(LED, LOW);
    delay(pause);
    SimbleeBLE.connectable = true;
  }

  // LIB : send values to computer
  if(connected){
    SimbleeBLE.send((char *)array1, sizeof(array1));
    SimbleeBLE.send((char *)array2, sizeof(array2));
    SimbleeBLE.send((char *)array3, sizeof(array3));
    SimbleeBLE.connectable = false;
  }

  // USER : use the values to set the pins
  if(pwm == 0){
    digitalWrite(LED, received[7]);
  }
  if(pwm == 1){
    analogWrite(LED, pwm_values[0]);
  }
}


// LIB
void SimbleeBLE_onReceive(char *data, int len){
  if(data[0] == 0){
    if(len<=13){
      for(int i=1; i<len; i++){
        received[i-1] = data[i];
      }
    }
  }
  if(data[0] == 1){
    int * res= (int *) data;
    if(sizeof(res) <= 4){
      for(int i=1; i<sizeof(res); i++){
        pwm_values[i-1] = res[i];
      }
    }
    if(res[1] == 0){
      pwm = 0;
    }
    else{
      pwm = 1;
    }
  }
}

// LIB
void SimbleeBLE_onConnect(){
  connected = 1;
}

// LIB
void SimbleeBLE_onDisconnect(){
  connected = 0;
}
