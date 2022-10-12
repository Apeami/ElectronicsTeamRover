//Pins used in this program
//Pin A0-5 -  used for the inputs from the controller

void pciSetup(byte pin){ //this sets up all of the interrupt bits. It is done so that the interrupt will fire when a change in pins A0-A5 is detected
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin));                   // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin));                   // enable interrupt for the group
}
const int pwmPIN[]={A0,A1,A2,A3,A4}; //Array of pins
const int num_ch = 5; //Number of pins
int prev_pinState[num_ch]; //The previous state of every pin

int pwmTimer[num_ch]; //The time that the pin is currently in it's state
int timeHigh[num_ch]; //The time that the pin is high
int timeLow[num_ch];  //and low

int pciTime;

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
  A /= 100;
  B /= 100;
  Serial.println(A+B);
  Serial.println(A*1000);
  return (A*100)/(A+B);
}


void setup() {
  Serial.begin(9600); //Setup serial
  setupControllerReader(); //Setup
}

void loop() {

  //Read byte from serial and print
  Serial.println("LOOP");
  Serial.println(timeHigh[0]);
  //Serial.println(timeLow[0]);
  //Serial.println(calculateDutyCycle(timeHigh[0], timeLow[0]));

  //Delay for smoothness
  delay(100);
}
