#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile

//Transmission operation
//Array 5 value - [Motor L, Motor R, Servo 1, Servo 2, operationMode]


//PIN information
//Reserved pins 13,12,11,1,0 ,5(Maybe)
// Radio Pins - 2 
int RADIO_RECIEVE = 2;
//Motor control Pins - 4, 6, 7, 8
int MC_L_F = 4;
int MC_L_B = 6;
int MC_R_F = 7;
int MC_R_B = 8;
//Motor speed pins - 9, 10
int MS_L = 9;
int MS_R = 10;
//Radio input pins - Pins A0 - A5


//Controller reader variables
const int pwmPIN[]={A0,A1,A2,A3,A4}; //Array of pins
const int num_ch = 5; //Number of pins
int prev_pinState[num_ch]; //The previous state of every pin
int pwmTimer[num_ch]; //The time that the pin is currently in it's state
int timeHigh[num_ch]; //The time that the pin is high
int timeLow[num_ch];  //and low
int pciTime;


//Radio Settings
RH_ASK driver(2000, A5, 4, 5); //Set up radio communication library
uint8_t buflen = 5; 
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN]; //This is the array in which the radio input is found.

//This function recieves radio input, and puts it into buf array. It also returnes a bool if radio is connected.
int radioTick = 0; //This variable counts how long ago was last radio revcive
bool recieveRadio(){
  radioTick++;
  if (driver.recv(buf, &buflen)){ // Non-blocking. Message with a good checksum received, dump it.
    radioTick=0; //reset
    driver.printBuffer("Got:", buf, buflen); //Print
    String rcv;
    for (int i = 0; i < buflen; i++) { //Print
      rcv += (char)buf[i];
    }
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
  if (!driver.init()){
    Serial.println("init failed");
  }
}

//Ensure all motor pins are set to iotupt
void motorSetup(){
  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
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

//List of operation modes
// 0 - Manual from radio controlled joystick (LED off)
// 1 - Manual from computer keyboard (LED on)
// 2 - Automatic #1 (LED even flash)
// 3 - Automatic #2 (LED odd flash)
int operationMode = 0;

void loop() {
  //This loop runs 5 times every second
  bool isConnectedRadio = recieveRadio(); //Recieve new data from radio input
  if (!isConnectedRadio){
    operationMode = 0; //Check to see if radio is connected, and if not set to operation mode 0
  }else{
    operationMode = buf[4]; //Get operation mode from radio signal sent.
  }
  setLEDPin(operationMode); //Set blinking LED to mark current operation mode
  runWheel(buf[0],buf[1]);
  delay(200);
}
