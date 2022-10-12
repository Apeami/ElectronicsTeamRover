

void runWheel(int motorSpeed, int side){
  if (side==0){
    Serial.println("Motorspeed: " + String(motorSpeed));
    if (motorSpeed==0){
      digitalWrite(5,LOW);
      digitalWrite(6,LOW);
    }if (motorSpeed>0){
      digitalWrite(5,LOW);
      digitalWrite(6,HIGH);
    }if (motorSpeed<0){
      digitalWrite(5,HIGH);
      digitalWrite(6,LOW);
    }
  }if (side==1){
    if (motorSpeed==0){
      digitalWrite(10,LOW);
      digitalWrite(11,LOW);
    }if (motorSpeed<0){
      digitalWrite(10,LOW);
      digitalWrite(11,HIGH);
    }if (motorSpeed>0){
      digitalWrite(10,HIGH);
      digitalWrite(11,LOW);
    }
  }
}

void motorSetup(){
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
}


void setup() {
  motorSetup();
  Serial.begin(9600);
}

void loop() {
  runWheel(100, 0);
  runWheel(100, 1);
  delay(3000);
  runWheel(0, 0);
  runWheel(0, 1);
  delay(3000);
  runWheel(-100, 0);
  runWheel(-100, 1);
  delay(3000);
}
