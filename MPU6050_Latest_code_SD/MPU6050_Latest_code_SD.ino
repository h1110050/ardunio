#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
MPU6050 mpu;

#include <SD.h>
#include <SPI.h>

int cs = 10; // Set Chip Select to pin ten
File myFile;  // a File Object
char myFileName[] = "MyFile.txt";  // The name of the file we will create

#define LED_PIN 13 // 

// supply your own gyro offsets here, scaled for min sensitivity use MPU6050_calibration.ino
//                       XA      YA      ZA      XG      YG      ZG
int MPUOffsets[6] = {   1788,    0,      0,      220,    76,    -85};

// ================================================================
// ===                      i2c SETUP Items                     ===
// ================================================================
void i2cSetup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif
}

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

// ================================================================
// ===                      MPU DMP SETUP                       ===
// ================================================================
int FifoAlive = 0; // tests if the interrupt is triggering
int IsAlive = -20;     // counts interrupt start at -20 to get 20+ good values before assuming connected
// MPU control/status vars
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
byte StartUP = 100; // lets get 100 readings from the MPU before we start trusting them (Bot is not trying to balance at this point it is just starting up.)

void MPU6050Connect() {
  static int MPUInitCntr = 0;
  // initialize device
  mpu.initialize(); // same
  // load and configure the DMP
  devStatus = mpu.dmpInitialize();// same

  if (devStatus != 0) {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)

    char * StatStr[5] { " ", "initial memory load", "DMP config updates", "3", "4"};

    MPUInitCntr++;

    Serial.print(F("MPU connection Try #"));
    Serial.println(MPUInitCntr);
    Serial.print(F("DMP Initialization "));
    Serial.print(StatStr[devStatus]);
    Serial.print(F(" failed (code "));
    Serial.println(F(")"));

    if (MPUInitCntr >= 10) return; //only try 10 times
    delay(1000);
    MPU6050Connect(); // Lets try again
    return;
  }
  mpu.setXAccelOffset(MPUOffsets[0]);
  mpu.setYAccelOffset(MPUOffsets[1]);
  mpu.setZAccelOffset(MPUOffsets[2]);
  mpu.setXGyroOffset(MPUOffsets[3]);
  mpu.setYGyroOffset(MPUOffsets[4]);
  mpu.setZGyroOffset(MPUOffsets[5]);

  Serial.println(F("Enabling DMP..."));
  mpu.setDMPEnabled(true);
  // enable Arduino interrupt detection
  Serial.println(F("Enabling interrupt detection (Arduino external interrupt pin 2 on the Uno)..."));
  Serial.print(F("mpu.getInterruptDrive=  ")); Serial.println(mpu.getInterruptDrive());
  attachInterrupt(0, dmpDataReady, RISING); //pin 2 on the Uno
  mpuIntStatus = mpu.getIntStatus(); // Same
  // get expected DMP packet size for later comparison
  packetSize = mpu.dmpGetFIFOPacketSize();
  delay(1000); // Let it Stabalize
  mpu.resetFIFO(); // Clear fifo buffer
  mpu.getIntStatus();
  mpuInterrupt = false; // wait for next interrupt
}

// ================================================================
// ===                     SD Card Start                        ===
// ================================================================
void SDCardSetup() {
  pinMode(cs, OUTPUT);

  if (!SD.begin(cs)) {
    Serial.println(F("SD did not initiliaze"));
    while (1) ;
  }
  Serial.println(F("SD initialized."));

  // Lets check to make sure that the SD card doesn't already have our file
  if (! SD.exists(myFileName)) {
    myFile = SD.open(myFileName, FILE_WRITE);
    myFile.println(F("My 1st Line of Data")); myFile.println();
  }
  else {
    myFile = SD.open(myFileName, FILE_WRITE);
    myFile.println(F("Another Line of Data")); myFile.println();
  }


}


// ================================================================
// ===                    MPU DMP Get Data                      ===
// ================================================================
void GetDMP() { // Best version I have made so far
  // Serial.println(F("FIFO interrupt at:"));
  // Serial.println(micros());
  static unsigned long LastGoodPacketTime;
  mpuInterrupt = false;
  FifoAlive = 1;
  fifoCount = mpu.getFIFOCount();
  if ((!fifoCount) || (fifoCount % packetSize)) { // we have failed Reset and wait till next time!
    digitalWrite(LED_PIN, LOW); // lets turn off the blinking light so we can see we are failing.
    mpu.resetFIFO();// clear the buffer and start over
  } else {
    while (fifoCount  >= packetSize) { // Get the packets until we have the latest!
      mpu.getFIFOBytes(fifoBuffer, packetSize); // lets do the magic and get the data
      fifoCount -= packetSize;
    }
    LastGoodPacketTime = millis();
    SaveDataToSDCard(); // <<<<<<<<<<<<<<<<<<<<<<<<<<<< On success  Save Data To SD Card() <<<<<<<<<<<<<<<<<<<
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // Blink the Light
  }
}

// ================================================================
// ===                  Save Data To SD Card                    ===
// ================================================================
void SaveDataToSDCard() {
  float Yaw, Pitch, Roll;
  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
  Yaw = (ypr[0] * 180.0 / M_PI);
  Pitch = (ypr[1] *  180.0 / M_PI);
  Roll = (ypr[2] *  180.0 / M_PI);

  unsigned long Time;
  Time = micros();
  myFile.print(millis()); myFile.print(F(",\t"));
  myFile.print(Yaw); myFile.print(F(","));
  myFile.print(Pitch); myFile.print(F(","));
  myFile.print(Roll); myFile.println();
  myFile.flush();  // Save it.

  Time = micros() - Time;
  char S[10]; 
  // Serial port Spam Prevention:
  static int Counter; // MPU6050 interrupt triggers every 10 Milliseconds unless other code takes too long
  if ( Counter >= 10 ) { // Print output every 100 milliseconds
    Serial.print(F("SD Card Save Time "));    Serial.print(Time); Serial.print(F(" Microseconds"));
    Serial.print(F("\tYaw "));   Serial.print(dtostrf((float) Yaw, 6, 1 , S));
    Serial.print(F("\tPitch "));  Serial.print(dtostrf((float) Pitch, 6, 1 , S));
    Serial.print(F("\tRoll "));   Serial.print(dtostrf((float) Roll, 6, 1 , S));
    Serial.println();
    Counter = 0;
  }
  Counter++;

  // Reset The FIFO Buffer Because the above lines probibly takes longer than 10 milliseconds to handle and The next FIFO output Could Be OLD and or Corrupt data
  mpuInterrupt = false;
  mpu.resetFIFO();// clear the FIFO buffer and start over

}
// ================================================================
// ===                         Setup                            ===
// ================================================================
void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200); //115200
  while (!Serial);
  Serial.println(F("SD Card Start")); 
  SDCardSetup();   /*////////////////////////////// SD Card Start //////////////////////////////////////*/
  Serial.println(F("i2c Setup")); 
  i2cSetup();      /*////////////////////////////// i2c Start //////////////////////////////////////*/
  Serial.println(F("MPU6050 Connect"));
  MPU6050Connect();/*////////////////////////////// MPU6050 Start //////////////////////////////////////*/
  Serial.println(F("Setup complete"));
}
// ================================================================
// ===                          Loop                            ===
// ================================================================
void loop() {
  if (mpuInterrupt ) { // wait for MPU interrupt or extra packet(s) available
    GetDMP();
  }
}
