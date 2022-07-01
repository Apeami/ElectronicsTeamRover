#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
char dataBuffer[5] = {0,0,0,0,0}; //This temporary storage buffer

const byte address[6] = "00001";

void setupWire(){
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
}

void setupRadio(){
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
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

    radio.write(dataBuffer, 5);
}

//Setup funciton
void setup(){
  setupRadio();
  setupWire();
  pinMode(LED_BUILTIN,OUTPUT);
  Serial.begin(9600);
}

void runJoystick(){
  //TODO
}

void loop(){
  //First receive from computer and then transmit, 5 times every second.
  recieveWire();
  
  if (dataBuffer[4]==4){
    runJoystick();
  }
  
  transmitRadio();
  delay(200);
}
