// PWM input pins, any of the following pins can be used: digital 0 - 13 or analog A0 - A5 

const int pwmPIN[]={11,12,13}; //Array of pins // an array to identify the PWM input pins (the array can be any length) 
                                  // first pin is channel 1, second is channel 2...etc

int RC_inputs = 0;                // The number of pins in pwmPIN that are connected to an RC receiver. Addition pins not connected to an RC receiver could be used for any other purpose i.e. detecting the echo pulse on an HC-SR04 ultrasonic distance sensor
                                  // When 0, it will automatically update to the number of pins specified in pwmPIN[] after calling setup_pwmRead().                                                
//                CH1     CH2     CH3
int RC_min[6] = { 988,    988,    988};
int RC_mid[6] = { 1500,   1500,   1500};
int RC_max[6] = { 2012,   2012,   2012};

// fail safe positions

float RC_failsafe[] = {0.00, 0.00, 0.00};
   
// enter a failsafe position (in the range of -+1) for each RC channel in case radio signal is lost
// if the array is the incorrect length for the number of RC channels, the failsafe will default to neutral i.e. 0. 
// The failsafe tolerances are: 10-330Hz & 500-2500us

/*
 *    GLOBAL PWM DECODE VARIABLES
 */

const int num_ch = 3;  // calculate the number of input pins (or channels)
volatile int PW[num_ch];                        // an array to store pulsewidth measurements
volatile boolean prev_pinState[num_ch];         // an array used to determine whether a pin has gone low-high or high-low
volatile unsigned long pciTime;                 // the time of the current pin change interrupt
volatile unsigned long pwmTimer[num_ch];        // an array to store the start time of each PWM pulse

volatile boolean pwmFlag[num_ch];               // flag whenever new data is available on each pin
volatile boolean RC_data_rdy;                   // flag when all RC receiver channels have received a new pulse
unsigned long pwmPeriod[num_ch];                 // period, mirco sec, between two pulses on each pin

byte pwmPIN_reg[num_ch];                        // each of the input pins expressed as a position on it's associated port register
byte pwmPIN_port[num_ch];                       // identify which port each input pin belongs to (0 = PORTB, 1 = PORTC, 2 = PORTD)

const int size_RC_min = sizeof(RC_min) / sizeof(int);           // measure the size of the calibration and failsafe arrays
const int size_RC_mid = sizeof(RC_mid) / sizeof(int);
const int size_RC_max = sizeof(RC_max) / sizeof(int);
const int size_RC_failsafe = sizeof(RC_failsafe) / sizeof(float);

// FUNCTION USED TO TURN ON THE INTERRUPTS ON THE RELEVANT PINS
// code from http://playground.arduino.cc/Main/PinChangeInterrupt

void pciSetup(byte pin){
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin));                   // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin));                   // enable interrupt for the group
}

// FUNCTION USED TO FIND THE PIN POSITION ON EACH PORT REGISTER: helps the interrupt service routines, ISR, run faster

void pwmPIN_to_port(){
  for (int i = 0; i < num_ch; i++){

    // determine which port and therefore ISR (PCINT0_vect, PCINT1_vect or PCINT2_vect) each pwmPIN belongs to.
                                                                  pwmPIN_port[i] = 1;    // pin belongs to PCINT1_vect (PORT C)
    if (pwmPIN[i] >= 0 && pwmPIN[i] <= 7)                         pwmPIN_port[i] = 2;    // pin belongs to PCINT2_vect (PORT D)
    else if (pwmPIN[i] >= 8 && pwmPIN[i] <= 13)                   pwmPIN_port[i] = 0;    // pin belongs to PCINT0_vect (PORT B)

    // covert the pin number (i.e. pin 11 or pin A0) to the pin position in the port register. There is most likely a better way of doing this using a macro...
    // (Reading the pin state directly from the port registers speeds up the code in the ISR)
    
    if(pwmPIN[i] == 0 || pwmPIN[i] == A0 || pwmPIN[i] == 8)         pwmPIN_reg[i] = 0b00000001;
    else if(pwmPIN[i] == 1 || pwmPIN[i] == A1 || pwmPIN[i] == 9)    pwmPIN_reg[i] = 0b00000010;
    else if(pwmPIN[i] == 2 || pwmPIN[i] == A2 || pwmPIN[i] == 10)   pwmPIN_reg[i] = 0b00000100;
    else if(pwmPIN[i] == 3 || pwmPIN[i] == A3 || pwmPIN[i] == 11)   pwmPIN_reg[i] = 0b00001000;
    else if(pwmPIN[i] == 4 || pwmPIN[i] == A4 || pwmPIN[i] == 12)   pwmPIN_reg[i] = 0b00010000;
    else if(pwmPIN[i] == 5 || pwmPIN[i] == A5 || pwmPIN[i] == 13)   pwmPIN_reg[i] = 0b00100000;
    else if(pwmPIN[i] == 6)                                         pwmPIN_reg[i] = 0b01000000;
    else if(pwmPIN[i] == 7)                                         pwmPIN_reg[i] = 0b10000000;
    
  }
}

// SETUP OF PIN CHANGE INTERRUPTS

void setup_pwmRead(){
  
  for(int i = 0; i < num_ch; i++){              // run through each input pin
    pciSetup(pwmPIN[i]);                        // enable pinchange interrupt for pin
  }
  pwmPIN_to_port();                             // determines the port for each input pin
                                                // pwmPIN_to_port() also coverts the pin number in pwmPIN[] (i.e. pin 11 or pin A0) to the pin position in the port register (i.e. 0b00000001) for use in the ISR.
  
  if(RC_inputs == 0 || RC_inputs > num_ch) RC_inputs = num_ch;    // define the number of pins connected to an RC receiver.                                          
} 

// INTERRUPT SERVICE ROUTINES (ISR) USED TO READ PWM INPUT

// the PCINT0_vect (B port register) reacts to any changes on pins D8-13.
// the PCINT1_vect (C port register)          ""        ""         A0-A5.
// the PCINT2_vect (D port register)          ""        ""         D0-7.

// port registers are used to speed up if statements in ISR code:
// https://www.arduino.cc/en/Reference/PortManipulation http://tronixstuff.com/2011/10/22/tutorial-arduino-port-manipulation/
// http://harperjiangnew.blogspot.co.uk/2013/05/arduino-port-manipulation-on-mega-2560.html


// READ INTERRUPTS ON PINS D8-D13: ISR routine detects which pin has changed, and returns PWM pulse width, and pulse repetition period.

//int prevValue[3];
//unsigned int prevTime[3];
//int timeHigh[3];
//int timeLow[3];
//int pins[3] = {9,10,11};
//9,10,11
//const int pwmPIN[]={9,10,11}; //Array of pins
//const int num_ch = 3; //Number of pins
//int prev_pinState[num_ch]; //The previous state of every pin
//
//int pwmTimer[num_ch]; //The time that the pin is currently in it's state
int timeHigh[num_ch]; //The time that the pin is high
int timeLow[num_ch];  //and low
//
//int pciTime;


ISR(PCINT0_vect){ // this function will run if a pin change is detected on portB
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
//  Serial.println("Int0");
//  pciTime = micros();                                             // Record the time of the PIN change in microseconds
//  Serial.println(pciTime);
//  for (int i = 0; i <= 2 ; i++){
//    Serial.println(pciTime);
//    int value = digitalRead(pins[i]);
//    Serial.println(value);
//    if (prevValue[i] == 0 and value == 1){ //Step Up
//      Serial.println("Step UP");
//      timeLow[i]= pciTime - prevTime[i];
//      Serial.println(pciTime);
//      Serial.println(timeLow[i]);
//      Serial.println(prevTime[i]);
//      prevTime[i] = pciTime;
//      prevValue[i] = value;
//    }if (prevValue[i] == 1 and value == 0){ //Step Down
//      Serial.println("Step Down");
//      timeHigh[i]= pciTime - prevTime[i];
//      Serial.println(pciTime);
//      Serial.println(timeHigh[i]);
//      Serial.println(prevTime[i]);
//      prevTime[i] = pciTime;
//      prevValue[i] = value;
//    }
//  }
//  Serial.println(pciTime);
//  for (int i = 0; i < num_ch; i++){                               // run through each of the channels
//    if (pwmPIN_port[i] == 0){                                     // if the current channel belongs to portB
//      
//      if(prev_pinState[i] == 0 && PINB & pwmPIN_reg[i]){          // and the pin state has changed from LOW to HIGH (start of pulse)
//        prev_pinState[i] = 1;                                     // record pin state
//        pwmPeriod[i] = pciTime - pwmTimer[i];                     // calculate the time period, micro sec, between the current and previous pulse
//        pwmTimer[i] = pciTime;                                    // record the start time of the current pulse
//      }
//      else if (prev_pinState[i] == 1 && !(PINB & pwmPIN_reg[i])){ // or the pin state has changed from HIGH to LOW (end of pulse)
//        prev_pinState[i] = 0;                                     // record pin state
//        PW[i] = pciTime - pwmTimer[i];                            // calculate the duration of the current pulse
//        pwmFlag[i] = HIGH;                                        // flag that new data is available
//        if(i+1 == RC_inputs) RC_data_rdy = HIGH;                  
//      }
//    }
//  }
}

// READ INTERRUPTS ON PINS A0-A5: ISR routine detects which pin has changed, and returns PWM pulse width, and pulse repetition period.

ISR(PCINT1_vect){                                                 // this function will run if a pin change is detected on portC
  pciTime = micros();                                             // Record the time of the PIN change in microseconds

  for (int i = 0; i < num_ch; i++){                               // run through each of the channels
    if (pwmPIN_port[i] == 1){                                     // if the current channel belongs to portC
      
      if(prev_pinState[i] == 0 && PINC & pwmPIN_reg[i]){          // and the pin state has changed from LOW to HIGH (start of pulse)
        prev_pinState[i] = 1;                                     // record pin state
        pwmPeriod[i] = pciTime - pwmTimer[i];                     // calculate the time period, micro sec, between the current and previous pulse
        pwmTimer[i] = pciTime;                                    // record the start time of the current pulse
      }
      else if (prev_pinState[i] == 1 && !(PINC & pwmPIN_reg[i])){ // or the pin state has changed from HIGH to LOW (end of pulse)
        prev_pinState[i] = 0;                                     // record pin state
        PW[i] = pciTime - pwmTimer[i];                             // calculate the duration of the current pulse
        pwmFlag[i] = HIGH;                                         // flag that new data is available
        if(i+1 == RC_inputs) RC_data_rdy = HIGH;
      }
    }
  }
}

// READ INTERRUPTS ON PINS D0-7: ISR routine detects which pin has changed, and returns PWM pulse width, and pulse repetition period.

ISR(PCINT2_vect){                                                 // this function will run if a pin change is detected on portD

  pciTime = micros();                                             // Record the time of the PIN change in microseconds

  for (int i = 0; i < num_ch; i++){                               // run through each of the channels
    if (pwmPIN_port[i] == 2){                                     // if the current channel belongs to portD
      
      if(prev_pinState[i] == 0 && PIND & pwmPIN_reg[i]){          // and the pin state has changed from LOW to HIGH (start of pulse)
        prev_pinState[i] = 1;                                     // record pin state
        pwmPeriod[i] = pciTime - pwmTimer[i];                     // calculate the time period, micro sec, between the current and previous pulse
        pwmTimer[i] = pciTime;                                    // record the start time of the current pulse
      }
      else if (prev_pinState[i] == 1 && !(PIND & pwmPIN_reg[i])){ // or the pin state has changed from HIGH to LOW (end of pulse)
        prev_pinState[i] = 0;                                     // record pin state
        PW[i] = pciTime - pwmTimer[i];                            // calculate the duration of the current pulse
        pwmFlag[i] = HIGH;                                        // flag that new data is available
        if(i+1 == RC_inputs) RC_data_rdy = HIGH;
      }
    }
  }
}



/*
 *  Receiver Calibration
 */

 // NEED TO SPEED UP

float calibrate(float Rx, int Min, int Mid, int Max){
   float calibrated;
   if (Rx >= Mid)
   {
    calibrated = map(Rx, Mid, Max, 0, 1000);  // map from 0% to 100% in one direction
   }
   else if (Rx == 0)
   {
    calibrated = 0;                           // neutral
   }
   else
   {
    calibrated = map(Rx, Min, Mid, -1000, 0); // map from 0% to -100% in the other direction
   }
  return calibrated * 0.001;
}

// Basic Receiver FAIL SAFE
// check for 500-2500us and 10-330Hz (same limits as pololu)

boolean FAILSAFE(int CH){

   int i = CH-1;
   boolean failsafe_flag = LOW;
        
       if(pwmFlag[i] == 1)                             // if a new pulse has been measured.
         {
            pwmFlag[i] = 0;                            // set flag to zero
      
            if(pwmPeriod[i] > 100000)                  // if time between pulses indicates a pulse rate of less than 10Hz   
            {
              failsafe_flag = HIGH;                       
            }
            else if(pwmPeriod[i] < 3000)               // or if time between pulses indicates a pulse rate greater than 330Hz   
            {
              failsafe_flag = HIGH;                             
            }

            if(PW[i] < 500 || PW[i] > 2500)           // if pulswidth is outside of the range 500-2500ms
            {
              failsafe_flag = HIGH;                        
            }   
         }
        else if (micros() - pwmTimer[i] > 100000)     // if there is no new pulswidth measurement within 100ms (10hz)
        {
          failsafe_flag = HIGH;                      
        }

    return failsafe_flag;   
}

/*
 *  Quick print function of Rx channel input
 */

void print_RCpwm(){                             // display the raw RC Channel PWM Inputs
  for (int i = 0; i < RC_inputs; i++){
    //Serial.print(" ch");Serial.print(i+1);
    Serial.print("  ");
    if(PW[i] < 1000) Serial.print(" ");
    Serial.print(PW[i]);
  }
  Serial.println("");
}

void print_decimal2percentage(float dec){
  int pc = dec*100;
  // the number and text will take up 6 charactors i.e ___3%_ or -100%_
  if (pc >= 0) Serial.print(" ");
  if (abs(pc) < 100) Serial.print(" ");
  if (abs(pc) < 10) Serial.print(" ");
  Serial.print(" ");Serial.print(pc);Serial.print("% ");
}

/*
 * GENERIC PWM FUNCTIONS
 */

unsigned long pin_time;
float pin_pwm;
float pin_period;

boolean PWM_read(int CH){
  if(CH < 1 && CH > num_ch) return false;
  int i = CH-1;
  boolean avail = pwmFlag[i];
  if (avail == HIGH){
    pwmFlag[i] = LOW;
    noInterrupts();
    pin_time = pwmTimer[i];
    pin_pwm = PW[i];
    pin_period = pwmPeriod[i];
    interrupts();
  }
  return avail;
}

unsigned long PWM_time(){return pin_time;}
float PWM_period(){return pin_period;}
float PWM(){return pin_pwm;}

float PWM_freq(){
  float freq;
  return freq = 1000000 / pin_period;  // frequency Hz
}

float PWM_duty(){
  float duty;
  duty = pin_pwm/pin_period;
  return duty;
}

int getPW(int i) {
  if (FAILSAFE(i+1) == HIGH) {
    return RC_failsafe[i];
  }
  if (PW[i] < 1000) {
    PW[i] = 1000;
  } else if (PW[i] > 2000) {
    PW[i] = 2000;
  }
  return PW[i];
}


void setup(){
  Serial.begin(9600);
  setup_pwmRead();
}

void loop(){
//  PWM_read(9);
//  PWM_read(10);
//  PWM_read(11);
//  print_RCpwm();
  Serial.println("Values");
  Serial.println(timeHigh[0]);
  Serial.println(timeHigh[1]);
  Serial.println(timeHigh[2]);
  delay(200);
}
