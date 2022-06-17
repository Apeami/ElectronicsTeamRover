#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

//Global settings
RH_ASK driver(2000, A5, 4, 5); 
uint8_t buflen = 5;
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void recieveRadio(){
  if (driver.recv(buf, &buflen)){ // Non-blocking. Message with a good checksum received, dump it.
    driver.printBuffer("Got:", buf, buflen); //Print
    String rcv;
    for (int i = 0; i < buflen; i++) {
      rcv += (char)buf[i];
    }
  }
}

void runWheel(int motorSpeedLeft,int motorSpeedRight){
  motorSpeedLeft = map(motorSpeedLeft, 0, 127, -100, 100);
  motorSpeedRight = map(motorSpeedRight, 0, 127, -100, 100);
  Serial.println("RUN");
  Serial.println("Motorspeed: " + String(motorSpeedLeft) + " " + String(motorSpeedRight) );
  if (motorSpeedLeft>10 && motorSpeedRight >10){
    Serial.println("Forward");
    digitalWrite(3,LOW);
    digitalWrite(6,HIGH);
    digitalWrite(9,LOW);
    digitalWrite(10,HIGH);
  }else if (motorSpeedLeft< -10 && motorSpeedRight < -10){
    Serial.println("Backwards");
    digitalWrite(3,HIGH);
    digitalWrite(6,LOW);
    digitalWrite(9,HIGH);
    digitalWrite(10,LOW);
  }else if (motorSpeedLeft<-10 && motorSpeedRight > 10){
    Serial.println("Turn Left");
    digitalWrite(3,LOW);
    digitalWrite(6,HIGH);
    digitalWrite(9,HIGH);
    digitalWrite(10,LOW);
  }else if(motorSpeedLeft>10 && motorSpeedRight< -10){
    Serial.println("Turn Right");
    digitalWrite(3,HIGH);
    digitalWrite(6,LOW);
    digitalWrite(9,LOW);
    digitalWrite(10,HIGH);
  } else {
    Serial.println("Stop");
    digitalWrite(3,LOW);
    digitalWrite(6,LOW);
    digitalWrite(9,LOW);
    digitalWrite(10,LOW);
  }
}

void setupRadio(){
  Serial.begin(9600);    // Debugging only
  if (!driver.init()){
    Serial.println("init failed");
  }
}

void motorSetup(){
  pinMode(3,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
}

void setup() {
  setupRadio();
  motorSetup();
}

void loop() {
  recieveRadio();
  runWheel(buf[0],buf[1]);
  delay(200);
}
