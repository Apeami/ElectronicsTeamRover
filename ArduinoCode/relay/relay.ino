#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
char dataBuffer[5] = {0,0,0,0,0}; //This temporary storage buffer

char recievedBuffer[8] = {0,2,3,4,5,6,7,13};

const byte addressTransmit[6] = "00001";
const byte addressRecieve[6] = "00002";

int counter = 0;
const int THRESH = 1;

void setupWire(){
  Serial.begin(9600);
  pinMode(LED_BUILTIN,OUTPUT);
}

void setupRadio(){
  radio.begin();
  radio.openWritingPipe(addressTransmit);
  radio.openReadingPipe(1, addressRecieve);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
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
  //Serial.println(dataBuffer[4]);
  if (dataBuffer[4]== 1){
    digitalWrite(LED_BUILTIN,1);
  }
  if(dataBuffer[4]== 0){
    digitalWrite(LED_BUILTIN,0);
  }
}

void transmitWire(){
  for (int i=0; i<8;i++){
     Serial.write(recievedBuffer[i]);
  }
}

//This function transmittes the data through the radio
void transmitRadio(){
    //Serial.println("Transmitter");
    //Serial.println(dataBuffer[4],HEX);
    radio.stopListening();
    radio.write(dataBuffer, 5);
    radio.startListening();
}

int radioTick = 0; //This variable counts how long ago was last radio revcive
bool recieveRadio(){
  radioTick++;
  if (radio.available()) {
    radioTick=0; //reset
    radio.read(recievedBuffer, 8);
  }
  if (radioTick>75){ return false;} //Last radio recieve was 5 seconds ago, assume that radio is not connected.
  else{return true;}
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

  recievedBuffer[1]++;

  
  //First receive from computer and then transmit, 5 times every second.
  recieveWire();
  
  if (dataBuffer[4]==4){ //Operation mode is 4(Arduino Joystick)
    runJoystick();
  }
  
  transmitRadio();
//  bool connection = recieveRadio();
//  if (connection==true){
//    recievedBuffer[0]=1;
//  }if (connection==false){
//    recievedBuffer[0]=0;
//  }
  
  counter++;
  if (counter>THRESH){
    transmitWire();
    counter=0;
  }
  delay(1);
}
