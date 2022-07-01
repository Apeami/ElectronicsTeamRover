
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


/*
--------------------------------------------------------
                      INITIALIZATION ZONE
--------------------------------------------------------
*/

//Transmission operation
//Array 5 value - [Motor L, Motor R, Servo 1, Servo 2, operationMode]


//PIN information
//Reserved pins 13,12,11,1,0 ,5(Maybe)
// Radio Pins - 2
RF24 radio(A9, A8); // CE, CSN
const byte address[6] = "00001";

//Motor control Pins
int ML1A = 22;
int ML1B = 23;
int ML1S = 2;
int ML2A = 24;
int ML2B = 25;
int ML2S = 3;
int ML3A = 26;
int ML3B = 27;
int ML3S = 4;
int MR1A = 28;
int MR1B = 29;
int MR1S = 5;
int MR2A = 30;
int MR2B = 31;
int MR2S = 6;
int MR3A = 32;
int MR3B = 33;
int MR3S = 7;


//Radio input pins - Pins A0 - A5
//Controller reader variables
const int pwmPIN[]={A0,A1,A2,A3,A4}; //Array of pins
const int num_ch = 5; //Number of pins
int prev_pinState[num_ch]; //The previous state of every pin //Don't change
int pwmTimer[num_ch]; //The time that the pin is currently in it's state //Don't change
int timeHigh[num_ch]; //The time that the pin is high
int timeLow[num_ch];  //and low
int pciTime;


//Radio Settings
uint8_t buflen = 5;
uint8_t buf[5]; //This is the array in which the radio input is found.


/*
--------------------------------------------------------
                      COMMUNICATION ZONE
--------------------------------------------------------
*/

//This function recieves radio input, and puts it into buf array. It also returnes a bool if radio is connected.
int radioTick = 0; //This variable counts how long ago was last radio revcive
bool recieveRadio(){
  Serial.println("RECIEVE");
  radioTick++;
  if (radio.available()) {
    Serial.println("EFE");
    radioTick=0; //reset
    radio.read(buf, buflen);
  }

  if (radioTick>25){ return false;} //Last radio recieve was 5 seconds ago, assume that radio is not connected.
  else{return true;}
}

ISR(PCINT1_vect){ //Interrupt pin 1, interrupt service routine, caused by a change in pins A0-A5
  pciTime = micros();  //gets the current time
  for (int i = 0; i < num_ch; i++){ //loops through all of the pins in A0 to A4, but since we don't know what pin caused the interrupt we have to check all of them.
      int state = digitalRead(pwmPIN[i]); //Gets the state of the pin, should be either high or low.
      if(prev_pinState[i] == 0 && state){  //If the pin is high
        prev_pinState[i] = 1;
        timeLow[i] = pciTime - pwmTimer[i]; //Calculate the time that was low
        pwmTimer[i] = pciTime; //Set the time
        //Serial.println(timeLow[i]);
      }
      else if (prev_pinState[i] == 1 && !state){ //If the pin is low
        prev_pinState[i] = 0;
        timeHigh[i] = pciTime - pwmTimer[i]; //Calculate the time that was high
        pwmTimer[i] = pciTime; //Set the time
        //Serial.println(timeHigh[i]);
      }
  }
}


/*
--------------------------------------------------------
                      MOTOR RUN ZONE
--------------------------------------------------------
*/

//Run wheel with value on each side
void runWheel(int motorSpeedLeft,int motorSpeedRight){
  //Ensure that value is within required range
  //In 0 - 127 , out -100 - 100
  motorSpeedLeft = map(motorSpeedLeft, 0, 127, -100, 100);
  motorSpeedRight = map(motorSpeedRight, 0, 127, -100, 100);
  analogWrite(9, motorSpeedLeft);
  analogWrite(10, motorSpeedRight);
  Serial.println("RUN");
  Serial.println("Motorspeed: " + String(motorSpeedLeft) + " " + String(motorSpeedRight) );
  if (motorSpeedLeft>10 && motorSpeedRight >10){//Forward
    Serial.println("Forward");
    digitalWrite(A0,LOW);
    digitalWrite(A1,HIGH);
    digitalWrite(A2,LOW);
    digitalWrite(A3,HIGH);
  }else if (motorSpeedLeft< -10 && motorSpeedRight < -10){//Back
    Serial.println("Backwards");
    digitalWrite(A0,HIGH);
    digitalWrite(A1,LOW);
    digitalWrite(A2,HIGH);
    digitalWrite(A3,LOW);
  }else if (motorSpeedLeft<-10 && motorSpeedRight > 10){//Left
    Serial.println("Turn Left");
    digitalWrite(A0,LOW);
    digitalWrite(A1,HIGH);
    digitalWrite(A2,HIGH);
    digitalWrite(A3,LOW);
  }else if(motorSpeedLeft>10 && motorSpeedRight< -10){//Right
    Serial.println("Turn Right");
    digitalWrite(A0,HIGH);
    digitalWrite(A1,LOW);
    digitalWrite(A2,LOW);
    digitalWrite(A3,HIGH);
  } else { //Stop
    Serial.println("Stop");
    digitalWrite(A0,LOW);
    digitalWrite(A1,LOW);
    digitalWrite(A2,LOW);
    digitalWrite(A3,LOW);
  }
}

void operateExternalJoystick(){
  int verMap = map(timeHigh[1],1000,1800, 0, 127);
  int horMap = map(timeHigh[1],1000,1800, 0, 127);
  runWheel(verMap,horMap);
}


/*
--------------------------------------------------------
                      SETUP ZONE
--------------------------------------------------------
*/


void pciSetup(byte pin){ //this sets up all of the interrupt bits. It is done so that the interrupt will fire when a change in pins A0-A5 is detected
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin));                   // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin));                   // enable interrupt for the group
}

void setupControllerReader(){
  //Sets pin to input and sets-up the interrupt
  //Pin interrupt setup
  for(int i = 0; i < num_ch; i++){  //Loop through each pin
    pciSetup(pwmPIN[i]);  //Set up interrupts with that pin
    pinMode(pwmPIN[i],INPUT);  //Set it as an input
  }
}

//This calculates duty cycle
int calculateDutyCycle(int A, int B){
  return (A*100)/(A+B);
}

//This function sets up all the requirements for the radio
void setupRadio(){
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

//Ensure all motor pins are set to iotupt
void motorSetup(){

  pinMode(ML1A,OUTPUT) ; pinMode(ML1B,OUTPUT) ; pinMode(ML1S,OUTPUT) ;
  pinMode(ML2A,OUTPUT) ; pinMode(ML2B,OUTPUT) ; pinMode(ML2S,OUTPUT) ;
  pinMode(ML3A,OUTPUT) ; pinMode(ML3B,OUTPUT) ; pinMode(ML3S,OUTPUT) ;
  pinMode(MR1A,OUTPUT) ; pinMode(MR1B,OUTPUT) ; pinMode(MR1S,OUTPUT) ;
  pinMode(MR2A,OUTPUT) ; pinMode(MR2B,OUTPUT) ; pinMode(MR2S,OUTPUT) ;
  pinMode(MR3A,OUTPUT) ; pinMode(MR3B,OUTPUT) ; pinMode(MR3S,OUTPUT) ; 

  buf[0] = 63.5; //Ensure that initial value is stop
  buf[1] = 63.5;
}

//This function provides the onboard led to change blink patterns to signify operation mode
int tickerLED = 0;
void setLEDPin(int state){
  tickerLED++;
  if (tickerLED>10){tickerLED=0;}
  if (state==0){
    digitalWrite(LED_BUILTIN,LOW);
  }if (state==1){
    digitalWrite(LED_BUILTIN,HIGH);
  }if (state==2){
    if (tickerLED<5){digitalWrite(LED_BUILTIN,HIGH);}
    if (tickerLED>=5){digitalWrite(LED_BUILTIN,LOW);}
  }if (state==3){
    if (tickerLED<1){digitalWrite(LED_BUILTIN,HIGH);}
    if (tickerLED>=1){digitalWrite(LED_BUILTIN,LOW);}
  }
}

//Setup function
void setup() {
  Serial.begin(9600); //Debugging only
  pinMode(LED_BUILTIN,OUTPUT);
  setupRadio();
  motorSetup();
  setupControllerReader();
}


/*
--------------------------------------------------------
                      MAIN LOOP ZONE
--------------------------------------------------------
*/


//List of operation modes
// 0 - Manual from radio controlled joystick (LED off)
// 1 - Manual from computer keyboard (LED on)
// 2 - Automatic #1 (LED even flash)
// 3 - Automatic #2 (LED odd flash)
int operationMode = 0;

//If the external joystick wants to take control
int extJoystickControl = 0;

void loop() {
  //This loop runs 5 times every second
  bool isConnectedRadio = recieveRadio(); //Recieve new data from radio input
  if (!isConnectedRadio){
    operationMode = 0; //Check to see if radio is connected, and if not set to operation mode 0
  }else{
    operationMode = buf[4]; //Get operation mode from radio signal sent.
  }
  setLEDPin(operationMode); //Set blinking LED to mark current operation mode

  //Code to get input form joystick
  extJoystickControl = 0; //TEMP TODO
  
  //TEMP 
  operationMode = 1;

  if (extJoystickControl == 0){
    if (operationMode==5){ //Operation of external joystick
      operateExternalJoystick();
    }
    if (operationMode==7){ //Arduino automatic operation
      Serial.println("NO auto operation");
    }

    if (operationMode==1 or operationMode==2 or operationMode==3 or operationMode==4 or operationMode==6){ //Motion is calculated from another location
     Serial.println(buf[0]);
     Serial.println(buf[1]);
     //runWheel(buf[0],buf[1]);
    }
  }if (extJoystickControl == 1){
    operateExternalJoystick()
  }
  delay(200);
}
