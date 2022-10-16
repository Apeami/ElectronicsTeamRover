// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;
//MPU6050 mpu(0x69); // <-- use for AD0 high



#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };



// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}


// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================





void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize serial communication
    // (115200 chosen because it is required for Teapot Demo output, but it's
    // really up to you depending on your project)
    Serial.begin(115200);
    while (!Serial); // wait for Leonardo enumeration, others continue immediately

    // NOTE: 8MHz or slower host processors, like the Teensy @ 3.3V or Arduino
    // Pro Mini running at 3.3V, cannot handle this baud rate reliably due to
    // the baud timing being too misaligned with processor ticks. You must use
    // 38400 or slower in these cases, or use some kind of external separate
    // crystal solution for the UART timer.

    // initialize device
    Serial.println(F("Initializing I2C devices..."));
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    //while (Serial.available() && Serial.read()); // empty buffer
    //while (!Serial.available());                 // wait for data
    //while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}


int disX = 0;
int disY = 0;

int accelPrevX = 0;
int accelPrevY = 0;

int veloDiffX[200];
int veloDiffY[200];

int timePrev=0;
int timeDiff[200];
int currentStep=0;

int VelocityX = 0;
int VelocityY = 0;
int previousVelocityX = 0;
int previousVelocityY = 0;

int integrate(int* value, int* times, int len){
  Serial.println("Integrate");
  int total=0;

  for(int i=1;i<len;i++){
    total+= (value[i+1]-value[i])*times[i]*times[i]*times[i];
  }
  
  return total;
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;
    // read a packet from FIFO
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet 
        

        /*
         * 
         * ACTUAL IMPORTANT CODE HERE
         * 
         */

            // display real acceleration, adjusted to remove gravity
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetAccel(&aa, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);


            //Set new values
            int accelX = aaReal.x;
            int accelY = aaReal.y;
            int currTime = millis();

            //Velocity Calculate
//            Serial.print("prevV\t");
            //Serial.println(VelocityX);
            Serial.println(accelX);
//            Serial.print("\t");
//            Serial.print(VelocityY);
//            Serial.println("\n***********************");
            
            VelocityX += (accelPrevX - accelX);
            VelocityY += (accelPrevY - accelY);
//            
//            if (VelocityX>=0){
//              Serial.println("\n++++++++++++++++++++");
//            }else{
//              Serial.println("\n--------------------");
//            }
            //Distance Calculate
//            Serial.print("dis\t");
//            Serial.print(disX);
//            Serial.print("\t");
//            Serial.print(disY);
//            Serial.println("\n***********************");
            int timeDiffT = currTime - timePrev;
            disX += (VelocityX + previousVelocityX) * 0.5 * timeDiffT * 0.001;
            disY += (VelocityY + previousVelocityY) * 0.5 * timeDiffT * 0.001;


            
            //Subtract from previous
            veloDiffX[currentStep] = previousVelocityX;
            veloDiffY[currentStep] = previousVelocityX;
            timeDiff[currentStep] = currTime - timePrev;


            //Set previous values
            previousVelocityX = VelocityX;
            previousVelocityY = VelocityY;
            accelPrevX = accelX;
            accelPrevY = accelY;
            timePrev = currTime;

            //Check to see if integration is ready
            currentStep++;
            if (currentStep>200){
//              Serial.println("GettingReadyToPrint");
              currentStep=0;

//              Serial.println("\nX velo");
//              for (int i = 1;i<200 ; i++){
//                Serial.print(veloDiffX[i]);
//                Serial.print(" ");
//              }
//
//              Serial.println("\nTime");
//              for (int i = 1;i<200 ; i++){
//                Serial.print(timeDiff[i]);
//                Serial.print(" ");
//              }
              
//              int xChange = integrate(veloDiffX , timeDiff, 200);
//              disX+=xChange;
//              Serial.println("X value");
//              Serial.println(xChange);
//              Serial.println(disX);
//              int yChange = integrate(veloDiffY , timeDiff, 200);
//              disY+=yChange;
//              Serial.println("Y value");
//              Serial.println(xChange);
//              Serial.println(disY);
            }
            
            //delay(5000);
//            Serial.println("***********************");
//            Serial.print("areal\t");
//            Serial.print(aaReal.x);
//            Serial.print("\t");
//            Serial.print(aaReal.y);
//            Serial.print("\t");
//            Serial.println(aaReal.z);
//            Serial.println("***********************");
//            
//            Serial.print("prevA\t");
//            Serial.print(previousAccX);
//            Serial.print("\t");
//            Serial.print(previousAccY);
//            Serial.println("\n***********************");
//
//            Serial.print("diffA\t");
//            Serial.print(previousAccX - aaReal.x);
//            Serial.print("\t");
//            Serial.print(previousAccY - aaReal.y);
//            Serial.println("\n***********************");
//
//            Serial.print("prevV\t");
//            Serial.print(previousVelocityX);
//            Serial.print("\t");
//            Serial.print(previousVelocityY);
//            Serial.println("\n***********************");
//
//           
//            Serial.print("Velocity x = "); 
//            Serial.print(previousVelocityX);
//            Serial.print(" Velocity y = ");
//            Serial.print(previousVelocityY);
//            Serial.println("\n***********************");
//
//            Serial.print("Distance x = "); 
//            Serial.print(xDist);
//            Serial.print(" Distance y = ");
//            Serial.print(yDist);
//            Serial.println("\n***********************");


           

        /*
         * 
         * END ACTUAL IMPORTANT CODE
         * 
         */


        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}
