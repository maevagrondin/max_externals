void setup() {
  // put your setup code here, to run once:

}

void loop() {
  if(Bean.getConnectionState() == false){
    Bean.setLed(255,0,0);
  }
  if(Bean.getConnectionState() == true){
    Bean.setLed(0,0,255);
  }
}
