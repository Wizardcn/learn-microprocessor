#include <Arduino.h>
#include <Wire.h>
#include <TaskScheduler.h>
#include "xbUCI.h"
#include "xbRTCC.h"

#define BH1750address  0x23
#define RTCC_ALARM_PIN 16
#define LED_MODE 2
xbUCI uci("check_get_set_utl_ltl_run_");
xbRTCC rtcc;
byte mode;
byte ctrlstate;
unsigned long vUTL, vLTL; 
unsigned long vLight;
//###############################################
void service(void);
void checksequence(void);
void ctrlProcess(void);
Task t1(1000, TASK_FOREVER, &ctrlProcess);
Scheduler runner;

void setup()
{
    delay(3000);
    // H/W initialize ---------------------------
    Serial.begin(115200);
    Wire.begin();
    pinMode(RTCC_ALARM_PIN, INPUT);
    pinMode(LED_MODE, OUTPUT);
    // Variables and state initialize -----------
    uci.initialize();
    mode = 0;
    runner.init();
    runner.addTask(t1);
    delay(500);
    t1.disable();
    vLTL = 100;
    vUTL = 400;
    rtcc.Check();
    // End of setup -----------------------------
}

void loop()
{
    runner.execute();
    if (mode == 0) {                //configuration mode
        if (Serial.available())
            uci.run((char)Serial.read());
        if (uci.cmdID > 0)
            service();
        digitalWrite(LED_MODE, 1);
    }
    else {                          //automatic mode
        if (Serial.available()) checksequence();
        digitalWrite(LED_MODE, 0);
    }
}

void service(void)
{
    byte res;
    switch (uci.cmdID)
    {
    case 1: //check     
        rtcc.Check();
        Serial.print("# Connect = ");
        Serial.print(rtcc.bConnected);
        Serial.print(", Valid = ");
        Serial.print(rtcc.bValid);
        Serial.print(", Alarm = ");
        Serial.print(rtcc.bAlarmEnable);
        break;
    case 2: //get
        rtcc.GetTime();
        rtcc.TimeToString();
        Serial.print("# Now --> ");
        Serial.print(rtcc.sTime);
        break;
    case 3: //set
        res = rtcc.SetTime(uci.parameter);
        if (res == 0)
            Serial.print("? Invalid format time");
        else
            Serial.print("# Set time");    
        break;
    case 4: //utl
        vUTL = uci.parameter.toInt();
        Serial.print("# UTL = ");
        Serial.print(vUTL);
        break;
    case 5: //ltl
        vLTL = uci.parameter.toInt();
        Serial.print("# LTL = ");
        Serial.print(vLTL);
        break;
    case 6: //run
        Serial.print("# Run\n\r\n\r");
        mode = 1;
        ctrlstate = 0;
        t1.enable();
        uci.cmdID = 0;
        return;
    }
    Serial.print("\n\r>");
    uci.cmdID = 0;
}

void checksequence(void) 
{
    char rb;
    rb = Serial.read();
    if (rb == '$') {
        t1.disable();
        mode = 0;
        uci.initialize();
    }
}
//###############################################
void readBH1750(void) {
    byte buff[2];
    char rotext[7];
    Wire.beginTransmission(BH1750address);
    Wire.requestFrom(BH1750address, 2);
    buff[0] = Wire.read();
    buff[1] = Wire.read();
    Wire.endTransmission();
    vLight = buff[0]*256 + buff[1];
    vLight *= 6;
    vLight /= 5;
}
void generateEvent(byte dir) {
    rtcc.GetTime();
    rtcc.TimeToString();
    Serial.print("@ ");
    Serial.print(rtcc.sTime);
    if (dir == 1)
        Serial.print(" H / ");
    else
        Serial.print(" L / ");
    Serial.println(vLight);
}
void ctrlProcess(void) {
    readBH1750();
    if (ctrlstate == 0) {
        //low state
        if (vLight > vUTL) {
            ctrlstate = 1;
            generateEvent(1);
        }
    }
    else {
        //high state
        if (vLight < vLTL) {
            ctrlstate = 0;
            generateEvent(0);
        }
    }
}