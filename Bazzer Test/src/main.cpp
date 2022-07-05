#include <Arduino.h>

#define Bazzer A15


void setup() {
  pinMode(Bazzer, OUTPUT);
}

void loop() {
  digitalWrite(Bazzer, HIGH);
  delay(1000);
  digitalWrite(Bazzer, LOW);
  delay(1000);
}