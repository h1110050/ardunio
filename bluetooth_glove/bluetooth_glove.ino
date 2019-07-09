/* This is a simple test for two way traffic via bluetooth 
   but you can try it first using the USB cable to the serial 
   monitor without the bluetooth module connected.
   
   Note that some bluetooth modules come set to a different baud rate.
   This means you need to alter the command
                Serial.begin(9600) accordingly
   Same goes for the setting in the bottom right corner on the 
   serial monitor
 
   NOTE ESPECIALLY
   You can watch proceedings on the PC serial monitor while you do
   the deeds on your Android device, but only watch. You cannot
   send from the serial monitor.
   
   HIT THE RESET BUTTON ON ARDUINO AFTER YOU HAVE MADE THE 
   CONNECTION WITH THE BLUETOOTH APP. And let the show begin....
   */

int thumb = A0;
int index = A1;
int mid = A2;
int ring = A3;
int pinky = A4;

void setup()
{
    Serial.begin(9600);
    pinMode(thumb, OUTPUT);
    pinMode(index, OUTPUT);
    pinMode(mid, OUTPUT);
    pinMode(ring, OUTPUT);
    pinMode(pinky, OUTPUT);
    /*Serial.println("---NUSHS ENIGINEERING INTEREST GROUP---");
    Serial.println("Welcome to E-Glove 2015,");
    Serial.println("try clenching your fist!");
    Serial.println("");*/
}

void loop() {
   //  check if thumb is closed or open 
   if (analogRead(thumb) < 600) {
     Serial.println("0"); }
   else if (analogRead(thumb) > 600) {
     Serial.println("5"); }
   //  check if index is closed or open 
   if (analogRead(index) < 600) {
     Serial.println("0"); }
   else if (analogRead(index) > 600) {
     Serial.println("4"); }
   //  check if middle is closed or open 
   if (analogRead(mid) < 600) {
     Serial.println("0"); }
   else if (analogRead(mid) > 600) {
     Serial.println("3"); }
   //  check if ring is closed or open 
   if (analogRead(ring) < 600) {
     Serial.println("0"); }
   else if (analogRead(ring) > 600) {
     Serial.println("2"); }
    //  check if pinky is closed or open 
   if (analogRead(pinky) < 600) {
     Serial.println("0"); }
   else if (analogRead(pinky) > 600) {
     Serial.println("1"); }
}
