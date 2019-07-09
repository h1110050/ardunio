// Controlling a servo position using a potentiometer (variable resistor)
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott>

// Require mouse control library
#include <MouseController.h>
 
// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
MouseController mouse(usb);

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
 
int val;    // variable to read the value from the analog pin

void mouseMoved() {
  //val = mouse.getXChange() + 127;            // reads the value of the potentiometer (value between 0 and 1023)
  //val = map(val, 0, 255, 0, 179);
  //Serial.println(val);
  //myservo.write(val);  // sets the servo position according to the scaled value
}

 
void setup()
{
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}
 
void loop()
{
  usb.Task();
  // waits for the servo to get there
  
  myservo.write(0);
  delay(1000);
  myservo.write(90);
  delay(1000);
  myservo.write(179);
  delay(1000);
}
