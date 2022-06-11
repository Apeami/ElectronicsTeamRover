

//Pins used in this program
//Pin A0-5 -  used for the inputs from the controller

void pciSetup(byte pin){
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin));                   // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin));                   // enable interrupt for the group
}
const int pwmPIN[]={A0,A1,A2,A3,A4};
const int num_ch = 5;
int prev_pinState[num_ch];

int pwmTimer[num_ch];
int timeHigh[num_ch];
int timeLow[num_ch];

int pciTime;

ISR(PCINT1_vect){                                                 
  pciTime = micros();                                             
  for (int i = 0; i < num_ch; i++){                              
      int state = digitalRead(pwmPIN[i]);                               
      if(prev_pinState[i] == 0 && state){                         
        prev_pinState[i] = 1; 
                                          
        timeLow[i] = pciTime - pwmTimer[i];                     
        pwmTimer[i] = pciTime; 
        //Serial.println(timeLow[i]);                                      
      }
      else if (prev_pinState[i] == 1 && !state){                  
        prev_pinState[i] = 0; 
                                            
        timeHigh[i] = pciTime - pwmTimer[i];    
        pwmTimer[i] = pciTime;   
        //Serial.println(timeHigh[i]);                     
      }   
  }
}

void setupControllerReader(){
  //Sets pin to input and sets-up the interrupt
  //Pin interrupt setup
  for(int i = 0; i < num_ch; i++){              
    pciSetup(pwmPIN[i]);  
    pinMode(pwmPIN[i],INPUT);                    
  }
}



int calculateDutyCycle(int A, int B){
  return (A*100)/(A+B);
}


void setup() {
  Serial.begin(9600);
  setupMotorDriver();
}

void loop() {

  //Read byte from serial and print
  Serial.println(timeHigh[0]);
  Serial.println(timeLow[0]);
  Serial.println(calculateDutyCycle(timeHigh[0], timeLow[0]));

  //Delay for smoothness
  delay(100);
}
