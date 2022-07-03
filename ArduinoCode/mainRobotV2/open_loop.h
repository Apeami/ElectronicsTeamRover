/**
 * Pinmapping
 * Backwheels
 *
 * Direction: 22,23,24,26
 * Speed: 3,4
 *
 * Middlewheels
 *
 *
 *
 *
 *
 *
 *
 */

int stopMoving(int moveTime)
{
  //Print out the movement direction, time of movement and speed
  Serial.println("");
  Serial.println(String("Moving ") + __func__  + " for " + String(float(moveTime)/1000) + "s");

  //Serial.println("Time: " + String(millis()));
  int startingTime = millis();
  int endTime = startingTime + moveTime;
  int timeDifference = millis()- startingTime;
  for (int i = startingTime; i <= endTime; i = startingTime + timeDifference)
  {
    //Serial.println("Starting time: " + String(startingTime)+ " End time: "+ String(endTime));
    //Serial.println("Loop time: "+ String(millis()));
    //Serial.println("i: " + String(i));
    //delay(1000);

    // Set the pins to the correct direction and speed
    digitalWrite(22,LOW);
    digitalWrite(23,LOW);
    analogWrite(3, 0);
    digitalWrite(24,LOW);
    digitalWrite(26,LOW);
    analogWrite(4, 0);

    digitalWrite(28,LOW);
    digitalWrite(30,LOW);
    analogWrite(5, 0);
    digitalWrite(32,LOW);
    digitalWrite(34,LOW);
    analogWrite(6, 0);

    digitalWrite(36,LOW);
    digitalWrite(38,LOW);
    analogWrite(7, 0);
    digitalWrite(40,LOW);
    digitalWrite(42,LOW);
    analogWrite(8, 0);

    timeDifference = millis() - startingTime;
    //Serial.println("Time difference: "+ String(timeDifference));

  }

  //Returns the final time difference
  Serial.println("****************************************");
  return timeDifference;
}



int forward(int moveTime, int moveSpeed)
{
  // Cap the move speed to be within the correct range
  if (moveSpeed > 255)
  {
    //Serial.println("Capping speed at 255");
    moveSpeed = 255;
  }
  else if (moveSpeed < 0)
  {
    //Serial.println("Set speed below 0");
    moveSpeed = 0;
  }
  //Print out the movement direction, time of movement and speed
  Serial.println("");
  Serial.println(String("Moving ") + __func__  + " for " + String(float(moveTime)/1000) + "s" + " at " + " speed: " + String(moveSpeed));

  //Serial.println("Time: " + String(millis()));
  int startingTime = millis();
  int endTime = startingTime + moveTime;
  int timeDifference = millis()- startingTime;
  for (int i = startingTime; i <= endTime; i = startingTime + timeDifference)
  {
    //Serial.println("Starting time: " + String(startingTime)+ " End time: "+ String(endTime));
    //Serial.println("Loop time: "+ String(millis()));
    //Serial.println("i: " + String(i));
    //delay(1000);

    // Set the pins to the correct direction and speed
    digitalWrite(22,HIGH);
    digitalWrite(23,LOW);
    analogWrite(3, moveSpeed);
    digitalWrite(24,HIGH);
    digitalWrite(26,LOW);
    analogWrite(4, moveSpeed);

    digitalWrite(28,HIGH);
    digitalWrite(30,LOW);
    analogWrite(5, moveSpeed);
    digitalWrite(32,HIGH);
    digitalWrite(34,LOW);
    analogWrite(6, moveSpeed);

    digitalWrite(36,HIGH);
    digitalWrite(38,LOW);
    analogWrite(7, moveSpeed);
    digitalWrite(40,HIGH);
    digitalWrite(42,LOW);
    analogWrite(8, moveSpeed);

    timeDifference = millis() - startingTime;
    //Serial.println("Time difference: "+ String(timeDifference));

  }
  //stopMoving(1);
  //Returns the final time difference
  Serial.println("****************************************");
  return timeDifference;
}

int backward(int moveTime, int moveSpeed)
{

  // Cap the move speed to be within the correct range
  if (moveSpeed > 255)
  {
    //Serial.println("Capping speed at 255");
    moveSpeed = 255;
  }
  else if (moveSpeed < 0)
  {
    //Serial.println("Set speed below 0");
    moveSpeed = 0;
  }
  //Print out the movement direction, time of movement and speed
  Serial.println("");
  Serial.println(String("Moving ") + __func__  + " for " + String(float(moveTime)/1000) + "s" + " at " + " speed: " + String(moveSpeed));

  //Serial.println("Time: " + String(millis()));
  int startingTime = millis();
  int endTime = startingTime + moveTime;
  int timeDifference = millis()- startingTime;
  for (int i = startingTime; i <= endTime; i = startingTime + timeDifference)
  {
    //Serial.println("Starting time: " + String(startingTime)+ " End time: "+ String(endTime));
    //Serial.println("Loop time: "+ String(millis()));
    //Serial.println("i: " + String(i));
    //delay(1000);

    // Set the pins to the correct direction and speed
    digitalWrite(22,LOW);
    digitalWrite(23,HIGH);
    analogWrite(3, moveSpeed);
    digitalWrite(24,LOW);
    digitalWrite(26,HIGH);
    analogWrite(4, moveSpeed);

    digitalWrite(28,LOW);
    digitalWrite(30,HIGH);
    analogWrite(5, moveSpeed);
    digitalWrite(32,LOW);
    digitalWrite(34,HIGH);
    analogWrite(6, moveSpeed);

    digitalWrite(36,LOW);
    digitalWrite(38,HIGH);
    analogWrite(7, moveSpeed);
    digitalWrite(40,LOW);
    digitalWrite(42,HIGH);
    analogWrite(8, moveSpeed);

    timeDifference = millis() - startingTime;
    //Serial.println("Time difference: "+ String(timeDifference));

  }
  //stopMoving(1);
  //Returns the final time difference
  Serial.println("****************************************");
  return timeDifference;
}

int right(int moveTime, int moveSpeed)
{
  // Cap the move speed to be within the correct range
  if (moveSpeed > 255)
  {
    //Serial.println("Capping speed at 255");
    moveSpeed = 255;
  }
  else if (moveSpeed < 0)
  {
    //Serial.println("Set speed below 0");
    moveSpeed = 0;
  }
  //Print out the movement direction, time of movement and speed
  Serial.println("");
  Serial.println(String("Moving ") + __func__  + " for " + String(float(moveTime)/1000) + "s" + " at " + " speed: " + String(moveSpeed));

  //Serial.println("Time: " + String(millis()));
  int startingTime = millis();
  int endTime = startingTime + moveTime;
  int timeDifference = millis()- startingTime;
  for (int i = startingTime; i <= endTime; i = startingTime + timeDifference)
  {
    //Serial.println("Starting time: " + String(startingTime)+ " End time: "+ String(endTime));
    //Serial.println("Loop time: "+ String(millis()));
    //Serial.println("i: " + String(i));
    //delay(1000);

    // Set the pins to the correct direction and speed
    digitalWrite(22,HIGH);
    digitalWrite(23,LOW);
    analogWrite(3, moveSpeed);
    digitalWrite(24,LOW);
    digitalWrite(26,HIGH);
    analogWrite(4, moveSpeed);

    digitalWrite(28,HIGH);
    digitalWrite(30,LOW);
    analogWrite(5, moveSpeed);
    digitalWrite(32,LOW);
    digitalWrite(34,HIGH);
    analogWrite(6, moveSpeed);

    digitalWrite(36,HIGH);
    digitalWrite(38,LOW);
    analogWrite(7, moveSpeed);
    digitalWrite(40,LOW);
    digitalWrite(42,HIGH);
    analogWrite(8, moveSpeed);


    timeDifference = millis() - startingTime;
    //Serial.println("Time difference: "+ String(timeDifference));

  }
  //stopMoving(1);
  //Returns the final time difference
  Serial.println("****************************************");
  return timeDifference;
}

int left(int moveTime, int moveSpeed)
{
  // Cap the move speed to be within the correct range
  if (moveSpeed > 255)
  {
    //Serial.println("Capping speed at 255");
    moveSpeed = 255;
  }
  else if (moveSpeed < 0)
  {
    //Serial.println("Set speed below 0");
    moveSpeed = 0;
  }
  //Print out the movement direction, time of movement and speed
  Serial.println("");
  Serial.println(String("Moving ") + __func__  + " for " + String(float(moveTime)/1000) + "s" + " at " + " speed: " + String(moveSpeed));

  //Serial.println("Time: " + String(millis()));
  int startingTime = millis();
  int endTime = startingTime + moveTime;
  int timeDifference = millis()- startingTime;
  for (int i = startingTime; i <= endTime; i = startingTime + timeDifference)
  {
    //Serial.println("Starting time: " + String(startingTime)+ " End time: "+ String(endTime));
    //Serial.println("Loop time: "+ String(millis()));
    //Serial.println("i: " + String(i));
    //delay(1000);

    // Set the pins to the correct direction and speed
    digitalWrite(22,LOW);
    digitalWrite(23,HIGH);
    analogWrite(3, moveSpeed * 0.8);
    digitalWrite(24,HIGH);
    digitalWrite(26,LOW);
    analogWrite(4, moveSpeed * 0.8 );

    digitalWrite(28,LOW);
    digitalWrite(30,HIGH);
    analogWrite(5, moveSpeed);
    digitalWrite(32,HIGH);
    digitalWrite(34,LOW);
    analogWrite(6, moveSpeed);

    digitalWrite(36,LOW);
    digitalWrite(38,HIGH);
    analogWrite(7, moveSpeed);
    digitalWrite(40,HIGH);
    digitalWrite(42,LOW);
    analogWrite(8, moveSpeed);

    timeDifference = millis() - startingTime;
    //Serial.println("Time difference: "+ String(timeDifference));

  }
  //stopMoving(1);
  //Returns the final time difference
  Serial.println("****************************************");
  return timeDifference;
}



void calibrateAngular()
{}

void calibrateLinear()
{}

void printTime(int timeMilli)
{
  Serial.println("Real time of move " + String(float(timeMilli)/1000) + "s");
  Serial.println("****************************************");
}
//void setup() {
//  // put your setup code here, to run once:
//  Serial.begin(9600);
//
//  //Backwheels
//  pinMode(22, OUTPUT);
//  pinMode(23, OUTPUT);
//  pinMode(24, OUTPUT);
//  pinMode(26, OUTPUT);
//  pinMode(3, OUTPUT);
//  pinMode(4, OUTPUT);
//
//  //Middlewheels
//  pinMode(28, OUTPUT);
//  pinMode(30, OUTPUT);
//  pinMode(32, OUTPUT);
//  pinMode(34, OUTPUT);
//  pinMode(5, OUTPUT);
//  pinMode(6, OUTPUT);
//
//  //Backwheels
//  pinMode(36, OUTPUT);
//  pinMode(38, OUTPUT);
//  pinMode(40, OUTPUT);
//  pinMode(42, OUTPUT);
//  pinMode(7, OUTPUT);
//  pinMode(8, OUTPUT);
//
//
////  printTime(forward(5000, 255));
////  printTime(backward(5000, 255));
////  printTime(stopMoving(3000));
////  printTime(left(5000, 255));
//  printTime(right(5000, 255));
//  printTime(stopMoving(3000));
//
//
//}

//void loop() {
//  // put your main code here, to run repeatedly:
//  Serial.println("Time: " + String(millis()));
//  delay(1000);
//
//}
