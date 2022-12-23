#include <Arduino.h>

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial.println("*** Syetem start ***");
}

void loop()
{
  Serial.println("Hello Art!");
  delay(5000);
}
