void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
double lulu = analogRead(A0);
double lala = analogRead(A2);
double vicky = analogRead(A4);
//Serial.println(lulu);
Serial.print((vicky/1023) * 5);
Serial.print("\t");
Serial.print((lala/1023) * 5);
Serial.print("\t");
Serial.println((lulu/1023) * 5);
delay(500);
}
