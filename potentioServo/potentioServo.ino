#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
int servopin = 9;

void setup() {
  Serial.begin(9600);
  myservo.attach(servopin);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 175);     // scale it to use it with the servo (value between 0 and 180)
  delay(50);
  myservo.write(val);                  // sets the servo position according to the scaled value                          // waits for the servo to get there
  delay(15); 
  //Serial.println(val);
}
