#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
//#include <Servo.h>
#include <AFMotor.h>

//Global settings
RH_ASK driver(2000, A5, 4, 5); 

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = 5;

int switchedOn = 0;

char dataIn[5] = {0};

int DRIVERTYPE = 0; //0=With motor controller board
                         //1=With no motorcontroler board

void setupRadio(){
  Serial.begin(9600);    // Debugging only
  if (!driver.init()){
    Serial.println("init failed");
  }
}

void recieveRadio(){
  if (driver.recv(buf, &buflen)){ // Non-blocking
    // Message with a good checksum received, dump it.
    //This is Simply debug now
    driver.printBuffer("Got:", buf, buflen); 
    String rcv;
    for (int i = 0; i < buflen; i++) {
      rcv += (char)buf[i];
    }
  }
}


//Servo ESC1;
//Servo ESC2;
AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
AF_DCMotor motor3(3, MOTOR12_64KHZ);
AF_DCMotor motor4(4, MOTOR12_64KHZ);
void runWheel(int motorSpeed, int side){
  if(DRIVERTYPE==0){
    Serial.println("Run");
    Serial.println(motorSpeed);
    motorSpeed = map(motorSpeed, 0, 127, -100, 100);
    Serial.println(motorSpeed);
    if (side==0){
      motor1.setSpeed(abs(motorSpeed));
      if (motorSpeed==0){motor1.run(RELEASE);return;}
      if (motorSpeed>0){motor1.run(FORWARD);}
      if (motorSpeed<0){motor1.run(BACKWARD);}
      motor2.setSpeed(abs(motorSpeed));
      if (motorSpeed==0){motor2.run(RELEASE);return;}
      if (motorSpeed>0){motor2.run(FORWARD);}
      if (motorSpeed<0){motor2.run(BACKWARD);}
    }
    if (side==1){
      motor3.setSpeed(abs(motorSpeed));
      if (motorSpeed==0){motor3.run(RELEASE);return;}
      if (motorSpeed>0){motor3.run(FORWARD);}
      if (motorSpeed<0){motor3.run(BACKWARD);} 
      motor4.setSpeed(abs(motorSpeed));
      if (motorSpeed==0){motor4.run(RELEASE);return;}
      if (motorSpeed>0){motor4.run(FORWARD);}
      if (motorSpeed<0){motor4.run(BACKWARD);} 
    }
  }
  else if(DRIVERTYPE==1){
//    motorSpeed = map(motorSpeed, 0, 127, 0, 180);
//    if (side==0){
//      ESC1.write(motorSpeed);
//    }
//    if (side==1){
//      ESC2.write(motorSpeed);
//    }
  }
}

void runServo(int angle, int number){
  if(DRIVERTYPE==0){
    
  }else if(DRIVERTYPE==1){
    
  }
}

void runLight(int state){
  if (state==0){digitalWrite(LED_BUILTIN, LOW);}
  if (state==1){digitalWrite(LED_BUILTIN, HIGH);}
}

void decodeData(char* data){
  Serial.println(data[0],HEX);
  Serial.println(data[1],HEX);
  Serial.println(data[2],HEX);
  Serial.println(data[3],HEX);
  Serial.println(data[4],HEX);
  runWheel(data[0],0);
  runWheel(data[1],1);
  //runServo(data[2],0);
  //runServo(data[3],1);
  switchedOn = data[4];
}

void setup(){
  setupRadio();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  recieveRadio();
  decodeData(buf);
  delay(200);
  //Temp Motor test
//  delay(2000);
//  runWheel(0, 0);
//  delay(2000);
//  runWheel(64, 0);
//  delay(2000);
//  runWheel(127, 0);
}
