#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;


const int damp = 0.9;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Initialize MPU6050");
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor");
    delay(500);
  }
}

int pitchF;
int rollF;

void loop()
{
  // Read normalized values 
  Vector normAccel = mpu.readNormalizeAccel();

  // Calculate Pitch & Roll
  int pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI;
  int roll = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/M_PI;

  //Low pass filter
  pitchF = 0.9 * pitchF + 0.1 * pitch;
  rollF = 0.9 * rollF + 0.1 * roll;

  //Format
  byte pitchS = (pitchF+180)*0.7;
  byte rollS = (rollF+180)*0.7;
  Serial.write(pitchS);
  Serial.write(rollS);

  delay(33);//Delay for timing
}
