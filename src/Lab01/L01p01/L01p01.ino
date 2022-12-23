#include <Arduino.h>
#define LED_ON_BOARD 16
#define LED_ON_MODULE 2
//===============================================
void setup()
{
    delay(3000);
    pinMode(LED_ON_BOARD, OUTPUT);
    pinMode(LED_ON_MODULE, OUTPUT);
}

void loop()
{
    digitalWrite(LED_ON_BOARD, HIGH);
    digitalWrite(LED_ON_MODULE, LOW);
    delay(1000);
    digitalWrite(LED_ON_BOARD, LOW);
    digitalWrite(LED_ON_MODULE, HIGH);
    delay(1000);
}
// ###############################################
