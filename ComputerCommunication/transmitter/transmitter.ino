#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver(2000, 4, 2, 5); 

char dataBuffer[5] = {0,0,0,0,0};

void setupWire(){
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
}


void setupRadio(){
  Serial.begin(9600);    // Debugging only
  if (!driver.init()){
    Serial.println("init failed");
  }
}

void recieveWire(){
  int done=false;
  static int i=0;
  while (Serial.available()){
    char data = Serial.read();
    if (done==false){ //This means that there still is data
      dataBuffer[i]=data;
      i++;
    }
    if (i>=5){done=false; i=0;} //array length passed - no more data
  } 
  if (dataBuffer[4]== 1){
    digitalWrite(LED_BUILTIN,1);
  }
  if(dataBuffer[4]== 0){
    digitalWrite(LED_BUILTIN,0);
  }
}

void transmitRadio(){
    //Serial.println("Transmitter");
    const char * msg = dataBuffer; //nessage is a data buffer
    //const char * msg = "hello my name is";

    driver.send((uint8_t *)msg, 5); //Data, length =5 (Constant)
    driver.waitPacketSent();
}

void setup(){
  setupRadio();
  setupWire();
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(9600);
}

void loop(){
  recieveWire();
  transmitRadio();
  delay(200);
}
