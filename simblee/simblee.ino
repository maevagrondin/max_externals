#include <SimbleeBLE.h>
#include <stdio.h>

const int LED = 13;
const int pause = 1000;
int connected = 0;
int first_msg = 1;
int my_esn = 0;

// max of 20 bytes accepted (5x1 int of 4 bytes)
float array1[5]={1,0,0,0,0};
float array2[4]={2,0,0,0};

bool received[8]={0,0,0,0,0,0,0,0};

void setup()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  char res[1024];
  sprintf(res, "%d", SimbleeCOM.getESN());

  SimbleeBLE.deviceName = res;
  SimbleeBLE.advertisementInterval = 20;
  SimbleeBLE.txPowerLevel = -4;

  SimbleeBLE.customUUID = "2220";

  Serial.begin(9600);

  SimbleeBLE.begin();

  pinMode(3, INPUT);
}

void loop()
{
  //array1[1] = analogRead(3);
  array1[1] = 1995;

  //Serial.printf("%s\n", SimbleeBLE.deviceName);
  if(!connected){
  analogWrite(13, 50);
  delay(1000);
  analogWrite(13, 255);
  delay(1000);
    /*digitalWrite(LED, HIGH);
    delay(pause);
    digitalWrite(LED, LOW);
    delay(pause);*/
    SimbleeBLE.connectable = true;
  }
  else{
    //digitalWrite(LED, HIGH);
    SimbleeBLE.send((char *)array1, sizeof(array1));
    SimbleeBLE.send((char *)array2, sizeof(array2));
    SimbleeBLE.connectable = false;
  }
}


void SimbleeBLE_onReceive(char *data, int len){
  for(int i=0; i<8; i++){
    received[i] = data[i]
  }
  digitalWrite(LED, received[7]);
}

void SimbleeBLE_onConnect(){
  connected = 1;
}

void SimbleeBLE_onDisconnect(){
  connected = 0;
}

void SimbleeBLE_onAdvertisement(bool start){
  Serial.println("ADVERTISING");
}
