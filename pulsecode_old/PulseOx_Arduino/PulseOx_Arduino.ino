
#include <math.h>

//--GLOBAL VARIABLES--
//int IR = 3;

void setup() {
  //pinMode(IR, OUTPUT);     //set IR pin to output
  //digitalWrite(IR, HIGH);  //turn off IR LED
  Serial.begin(9600);
}

void loop() {
  int irOut;
  irOut = analogRead(0);
  Serial.println(irOut);
  delay(50);
}
