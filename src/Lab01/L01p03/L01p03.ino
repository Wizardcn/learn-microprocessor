#include <Arduino.h>
#define LED_ON_MODULE 2
byte varCount; // Global variable ไม่ถูกทำลายเมื่อจบการทำงานของ function
//-----------------------------------------------
void taskToggle();
void taskCountUp();
//===============================================
void setup()
{
    // Power On Delay
    delay(3000); // Power On Delay: Waiting for Chip or peripheral initializing to ensure all peripheral ready.

    // H/W initialize: set up hardware firstly
    pinMode(LED_ON_MODULE, OUTPUT);
    Serial.begin(115200); // open serial port

    // Variables and state initialize
    varCount = 0;

    // End of setup
    Serial.println("\n\r\n\r#### End of setup ####");
}
void loop()
{
    taskToggle();
    taskCountUp();
    delay(1000);
}
// ###############################################
void taskToggle()
{
    Serial.println("Task: Blink");
    digitalWrite(LED_ON_MODULE, !digitalRead(LED_ON_MODULE));
}

void taskCountUp()
{
    Serial.print("Task: Counting up = ");
    Serial.println(varCount);
    varCount++;
}
