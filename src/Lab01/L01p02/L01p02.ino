#include <Arduino.h>
#define LED_ON_MODULE 2
void taskToggle();

//===============================================
void setup() {
	delay(3000);
    pinMode(LED_ON_MODULE, OUTPUT);
}
void loop() {
    taskToggle();
    delay(1000);
}
//###############################################
void taskToggle() {
    digitalWrite(LED_ON_MODULE, !digitalRead(LED_ON_MODULE));
}
