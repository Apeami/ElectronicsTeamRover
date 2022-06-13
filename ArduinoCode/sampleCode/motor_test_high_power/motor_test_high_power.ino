void setup() {
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);

}

void loop() {
  digitalWrite(5,HIGH);
  delay(5000);
  digitalWrite(5,LOW);
  digitalWrite(6,HIGH);
  delay(5000);
  digitalWrite(6,LOW);
  digitalWrite(10,HIGH);
  delay(5000);
  digitalWrite(10,LOW);
  digitalWrite(11,HIGH);
  delay(5000);
  digitalWrite(11,LOW);

}
