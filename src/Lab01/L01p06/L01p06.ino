#include <Arduino.h>
#define LED_ON_MODULE 2
#define PERIOD_TASK 1000
#define PRESCALE_2  2
unsigned long tick_last;
byte rqtask, rqtask_2;
byte precount_2;
byte varCount;
//-----------------------------------------------
void taskToggle();
void taskCountUp();
void taskDummy();
void myschedule();
//===============================================
void setup() {
    delay(3000);
    // H/W initialize
    pinMode(LED_ON_MODULE, OUTPUT);
    Serial.begin(115200);
    // Variables and state initialize
    tick_last = millis();
    rqtask = 0;
    rqtask_2 = 0;
    precount_2 = 0;
    varCount = 0;
    // End of setup
    Serial.println("\n\r\n\r\n\r#### End of setup ####");
}
void loop() {
    myschedule();
    if (rqtask == 1) {
        rqtask = 0;
        Serial.print("*** round ****: ");
        Serial.println(millis());
        taskToggle();
        taskCountUp();
    }
    if(rqtask_2 == 1) {
        rqtask_2 = 0;
        taskDummy();
    }
}
//###############################################
void myschedule() {
  unsigned long tnow, tdiff;
  tnow = millis();
  if (tnow > tick_last) {
    tdiff = tnow - tick_last;
  }
  else if (tnow < tick_last) {
    tdiff = 0xFFFFFFFF;
    tdiff -= tick_last;
    tdiff += tnow;
    tdiff++;
  }
  else return;
  if (tdiff < PERIOD_TASK) return;
  //---------------------------------------------
  tick_last += PERIOD_TASK;
  rqtask = 1;
  precount_2++;
  if (precount_2 == PRESCALE_2) {
      precount_2 = 0;
      rqtask_2 = 1;
  }
}
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
