#include <Encoder.h>
#include <util/atomic.h>

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2,3);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

long oldPosition  = -999;
int velocity = 0; 

long timePrev = millis();
void loop() {

  
  long newPosition = myEnc.read();
  int currTime = millis();
  velocity = ((newPosition - oldPosition)*100) / (currTime - timePrev);
  //Serial.println(newPosition - oldPosition);
  oldPosition = newPosition;
  timePrev = currTime;
  //Serial.println(newPosition);
  Serial.println(velocity);

  delay(100);
}
