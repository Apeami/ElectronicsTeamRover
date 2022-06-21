#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

RH_ASK driver(2000, 4, 2, 5); //Set up radio communication library

char dataBuffer[5] = {0,0,0,0,0}; //This temporary storage buffer

void setupWire(){
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
}

void setupRadio(){
  if (!driver.init()){ //Initialize radio
    Serial.println("init failed");
  }
}

//This function recieves information from the computer via wire.
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

//This function transmittes the data through the radio
void transmitRadio(){
    Serial.println("Transmitter");
    Serial.println(dataBuffer[4],HEX);
    const char * msg = dataBuffer; //message is a data buffer

    driver.send((uint8_t *)msg, 5); //Data, length =5 (Constant)
    driver.waitPacketSent();
}

//Setup funciton
void setup(){
  setupRadio();
  setupWire();
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(9600);
}

void loop(){
  //First receive from computer and then transmit, 5 times every second.
  recieveWire();
  transmitRadio();
  delay(200);
}
