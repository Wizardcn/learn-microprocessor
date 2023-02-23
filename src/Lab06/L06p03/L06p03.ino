#include <Arduino.h>
#include <Wire.h>
#include "xbUCI.h"
#include "xbRTCC.h"

#define BH1750address 0x23
#define RTCC_ALARM_PIN 16
xbUCI uci("check_get_set_run_stop_");
xbRTCC rtcc;
bool bRun;
String vLight;
// ###############################################
void service(void);
void serveAlarm(void);
void setup()
{
    delay(3000);
    // H/W initialize ---------------------------
    Serial.begin(115200);
    Wire.begin();
    pinMode(RTCC_ALARM_PIN, INPUT);
    Wire.beginTransmission(BH1750address);
    Wire.write(0x10);
    Wire.endTransmission();
    // Variables and state initialize -----------
    uci.initialize();
    bRun = 0;
    // End of setup -----------------------------
}

void loop()
{
    if (Serial.available())
        uci.run((char)Serial.read());
    if (uci.cmdID > 0)
        service();
    if ((rtcc.bAlarmEnable == 1) && (digitalRead(RTCC_ALARM_PIN) == 0))
        serveAlarm();
}

void service(void)
{
    byte res;
    switch (uci.cmdID)
    {
    case 1: // check
        rtcc.Check();
        Serial.print("# RTCC Valid = ");
        Serial.print(rtcc.bValid);
        Serial.print(", Run = ");
        Serial.print(bRun);
        break;
    case 2: // get time
        rtcc.GetTime();
        rtcc.TimeToString();
        Serial.print("# Now --> ");
        Serial.print(rtcc.sTime);
        break;
    case 3: // set
        res = rtcc.SetTime(uci.parameter);
        if (res == 0)
            Serial.print("! Invalid format time");
        else
            Serial.print("# Set time");
        break;
    case 4: // run
        if (bRun)
        {
            Serial.print("! Machine is running.");
        }
        else if (rtcc.bValid == 0)
        {
            Serial.print("! Cannot run the Machine due to RTCC");
        }
        else
        {
            rtcc.SetAlarm(1);
            Serial.print("# Machine starts.");
            bRun = 1;
        }
        break;
    case 5: // stop
        if (bRun == 0)
        {
            Serial.print("! Machine is off");
        }
        else
        {
            rtcc.SetAlarm(0);
            Serial.print("# Machine stops.");
            bRun = 0;
        }
        break;
    }
    Serial.print("\n\r>");
    uci.cmdID = 0;
}

void readBH1750(void)
{
    unsigned long ro;
    byte buff[2];
    char rotext[7];
    Wire.beginTransmission(BH1750address);
    Wire.requestFrom(BH1750address, 2);
    buff[0] = Wire.read();
    buff[1] = Wire.read();
    Wire.endTransmission();
    ro = buff[0] * 256 + buff[1];
    ro = ro * 6;
    ro = ro / 5;
    sprintf(rotext, "%6d", ro);
    vLight = String(rotext);
}

void serveAlarm(void)
{
    rtcc.ClearAlarm();
    rtcc.GetTime();
    rtcc.TimeToString();

    readBH1750();
    Serial.print("* ");
    Serial.print(rtcc.sTime);
    Serial.print(", ");
    Serial.print(vLight);
    Serial.print(" LUX");
    Serial.print("\n\r>");
}
