#include <Arduino.h>
#define LED_ON_MODULE 2
#define PERIOD_TASK 1000
unsigned long tick_last;
byte rqtask;
byte varCount;
//-----------------------------------------------
void taskToggle();
void taskCountUp();
void taskDummy();
void myschedule();
//===============================================
void setup()
{
    delay(3000);
    // H/W initialize
    pinMode(LED_ON_MODULE, OUTPUT);
    Serial.begin(115200);
    // Variables and state initialize
    tick_last = millis();
    varCount = 0;
    // End of setup
    Serial.println("\n\r\n\r#### End of setup ####");
}
void loop()
{
    myschedule();
    if (rqtask == 1)
    {
        rqtask = 0;
        Serial.print("*** round ****: ");
        Serial.println(millis());
        taskToggle();
        taskCountUp();
        taskDummy();
    }
}
// ###############################################
void myschedule()
{
    unsigned long tnow, tdiff;
    tnow = millis();
    Serial.println("---------------- injected ---------------");
    Serial.print("tick_last: ");
    Serial.println(tick_last);
    Serial.print("tnow: ");
    Serial.println(tnow);
    Serial.println("------------------------------------");
    if (tnow > tick_last)
    {
        tdiff = tnow - tick_last;
    }
    else if (tnow < tick_last)
    { // If Overflow
        tdiff = 0xFFFFFFFF;
        tdiff -= tick_last; // 2's complement
        tdiff += tnow;
        tdiff++;
    }
    else
        return;
    if (tdiff < PERIOD_TASK)
        return;
    //---------------------------------------------
    tick_last += PERIOD_TASK;
    rqtask = 1;
}

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

void taskDummy()
{
    int ii, j;
    Serial.println("Task: Dummy long loop count");
    for (ii = 0; ii < 2800; ii++)
    {
        j = ii + 1;
    }
}