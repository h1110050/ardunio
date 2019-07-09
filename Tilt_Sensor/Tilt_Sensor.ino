/*  ___   ___  ___  _   _  ___   ___   ____ ___  ____  
 * / _ \ /___)/ _ \| | | |/ _ \ / _ \ / ___) _ \|    \ 
 *| |_| |___ | |_| | |_| | |_| | |_| ( (__| |_| | | | |
 * \___/(___/ \___/ \__  |\___/ \___(_)____)___/|_|_|_|
 *                  (____/ 
 * This is the Arduino Tilt Sensor sketch.
 *This is just a simple example for you to start put hands on 
 *your tilt sensor. In this example, an LED will be turned off 
 *if the sensor is upright, and will be turned on if the sensor 
 *is tilted.
 * Tutorial URL http://osoyoo.com/2017/07/31/arduino-lesson-sw-520d-tilt-sensor/
 * CopyRight www.osoyoo.com
 */
const int ledPin = 13;//the led attach to
void setup()
{ 
pinMode(ledPin,OUTPUT);//initialize the ledPin as an output
pinMode(2,INPUT);//set pin2 as INPUT
digitalWrite(2, HIGH);//set pin2 as HIGH
} 

void loop() 
{ 
int digitalVal = digitalRead(2);//Read the value of pin2
if(HIGH == digitalVal)//if tilt switch is not breakover
{
digitalWrite(ledPin,LOW);//turn the led off
}
else ////if tilt switch breakover
{
digitalWrite(ledPin,HIGH);//turn the led on 
}
}

