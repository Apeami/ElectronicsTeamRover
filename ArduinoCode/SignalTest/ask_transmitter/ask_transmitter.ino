

#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile


RH_ASK driver(2000, 4, 2, 5); 

void setup()
{

    Serial.begin(9600);	  // Debugging only

    if (!driver.init())

         Serial.println("init failed");
}

void loop()
{
    Serial.println("Transmitting");
    const byte msg[] = {0xAA,0x43,0xA6,0xAA};

  
  for (int n = 0; n <= 3; n++)
  {
    Serial.println(msg[n], HEX);
  }
    driver.send((uint8_t *)msg, 4);
    driver.waitPacketSent();
    delay(1000);
}
