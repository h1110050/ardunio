void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(53, INPUT);
  pinMode(A8, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(A8), DEC);
  //Serial.print("\t");
  //Serial.println(digitalRead(53), DEC);
}
