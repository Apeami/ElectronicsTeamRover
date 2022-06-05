RH_ASK driver(2000, A5, 4, 5); 

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = 5;

void setup() {
  Serial.begin(9600);    // Debugging only
  if (!driver.init()){
    Serial.println("init failed");
  }
}

void loop() {
    if (driver.recv(buf, &buflen)){ // Non-blocking
    // Message with a good checksum received, dump it.
    driver.printBuffer("Got:", buf, buflen); 
    String rcv;
    for (int i = 0; i < buflen; i++) {
      rcv += (char)buf[i];
    }
    Serial.println(rcv);
  }
}
