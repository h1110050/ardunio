/* Turn an LED on/off based on a command send via BlueTooth
**
** Credit: The following example was used as a reference
** Rui Santos: http://randomnerdtutorials.wordpress.com
*/

int relay = 12; // use pin 12
int state = 0;
int flag = 0; // make sure that you return the state only once

void setup() {
  pinMode(relay, OUTPUT); // set output pin
  digitalWrite(relay, HIGH); //Change this depending on if you connect your relay NO or NC and whether you want the lights on or off when you turn on your arduino for the first time
  Serial.begin(9600); // Default connection rate for my BT module
}

void loop() {
  //if some data is sent, read it and save it in the state variable
  if(Serial.available() > 0){
    state = Serial.read();
    flag=0;
  }
  // if the state is 0 the led will turn off
  if (state == '0') {
    digitalWrite(relay, LOW);
    //Serial.println("off");
    if(flag == 0){
    flag = 1;
  }
}
  // if the state is 1 the led will turn on
  else if (state == '1') {
    digitalWrite(relay, HIGH);
    //Serial.println("on");
    if(flag == 0){
    flag = 1;
    }
  }
}
