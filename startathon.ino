//bluetooth thing
#include <SoftwareSerial.h>

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
//int16_t gx, gy, gz;

// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO

SoftwareSerial blueSerial(0,1);

bool isGoingUpStairs(float data[20]){
  
  if(data[19] < 0)
    return false;
  
  if(data[3] < 12310){
    return true;
  }
  else{
    if(data[5] < 17392){
      if(data[5]<11832){
        return true;
      }
      else{
        if(data[18]< 12000){
          return true;
        }
        else
          return false;
      }
    }
    else{
      return true;
    }
  }

}

void setup() {
  // initialize the serial communication:
  Serial.begin(9600);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(3, OUTPUT);
  
  //initialize bluetooth
  blueSerial.begin(38400);
  
      // Wait for hardware to initialize
    delay(1000);
  
     // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

float data[20];
int count = 0;
int beepCount = 0;

void loop() {
  Serial.print("force: ");
  Serial.println(analogRead(A2));
  Serial.print("button: ");
  Serial.println(analogRead(A3));
  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(100);
  
  bool wind = false;
  bool chin = false;
  bool button = false;
  bool nearby = false;
  bool stairs = false;
  
  int beep = 3;
  
    if(blueSerial.available()){
      char c = blueSerial.read();
      if(c){
        wind = true;
      }
    }
    
    if(analogRead(A2)>=1000){
      chin = true;
    }
    
    if(analogRead(A3)==1023){
      button = true;
    }
  

  
      // read raw accel/gyro measurements from device
    accelgyro.getAcceleration(&ax, &ay, &az);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    #ifdef OUTPUT_READABLE_ACCELGYRO
        // display tab-separated accel/gyro x/y/z values
        
     
     if(count == 19){
          data[19] = ay;
          stairs = isGoingUpStairs(data);
          count++;
        }
        else if(count >= 20){
           for(int i = 0; i <19;i++){
             data[i] = data[i+1];
             //Serial.println(data[i]);
           }
           data[19] = ay;
           count++;
           stairs = isGoingUpStairs(data);
        }
        else{
         data[count] = ay;
         count++;
        }    
        

    if(wind||nearby||stairs){
      if(!(chin||button)){
        beepCount = 2;
      }
    }
    
    
    if(beepCount >= 0){
      tone(beep, 1000); // Send 1KHz sound signal...
      delay(50);
      noTone(beep);
      beepCount --;
    }
        
    delay(200);
    #endif

    #ifdef OUTPUT_BINARY_ACCELGYRO
        Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 0xFF));
    #endif
}


