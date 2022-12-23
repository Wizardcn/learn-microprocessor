#include <Arduino.h>
#define LED_ON_MODULE 2
byte varCount;
//-----------------------------------------------
void taskToggle();
void taskCountUp();
void taskDummy();
//===============================================
void setup() {
    delay(3000);
    // H/W initialize
    pinMode(LED_ON_MODULE, OUTPUT);
    Serial.begin(115200);
    // Variables and state initialize
    varCount = 0;
    // End of setup
    Serial.println("\n\r\n\r#### End of setup ####");
}
void loop() {
    Serial.print("*** round ****: ");
    Serial.println(millis()); 
    taskToggle();
    taskCountUp();
    taskDummy();
    delay(1000);
}
//###############################################
void taskToggle() {
    Serial.println("Task: Blink");
    digitalWrite(LED_ON_MODULE, !digitalRead(LED_ON_MODULE));
}
void taskCountUp() {
    Serial.print("Task: Counting up = ");
    Serial.println(varCount);
    varCount++;
}
void taskDummy() {
    int ii, j;
    Serial.println("Task: Dummy long loop count");
    for(ii=0; ii<2800; ii++) {
        j = ii+1;
    }
}
