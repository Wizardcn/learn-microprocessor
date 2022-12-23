#include <Arduino.h>
#define LED_ON_BOARD 16
#define LED_ON_MODULE 2

void setup() {
  pinMode(LED_ON_MODULE, OUTPUT);
  pinMode(LED_ON_BOARD, OUTPUT);
  digitalWrite(LED_ON_MODULE, 1);
  digitalWrite(LED_ON_BOARD, 0);
  delay(2000);
}

void loop() {
  digitalWrite(LED_ON_MODULE, 1);
  digitalWrite(LED_ON_BOARD, 0);
  delay(500);
  digitalWrite(LED_ON_MODULE, 0);
  digitalWrite(LED_ON_BOARD, 1);
  delay(500);
}
