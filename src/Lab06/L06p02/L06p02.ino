#include <Arduino.h>
#include <Wire.h>
#include "xbUCI.h"
#include "xbRTCC.h"

#define RTCC_ALARM_PIN 16
xbUCI uci("check_get_set_alarm_");
xbRTCC rtcc;
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
    // Variables and state initialize -----------
    uci.initialize();
    // End of setup -----------------------------
}

void loop()
{
    if (Serial.available())
        uci.run((char)Serial.read());
    if (uci.isUserCmd())
        service();
    if (rtcc.alarmIsReady(RTCC_ALARM_PIN))
        serveAlarm();
}

void service(void)
{
    byte res;
    String datetime;
    switch (uci.getCmdId())
    {
    case 1: // check
        alarmStatus alarmStatus = rtcc.getAlarmStatus();
        Serial.print("# Connect = ");
        Serial.print(alarmStatus.bConnected);
        Serial.print(", Valid = ");
        Serial.print(alarmStatus.bValid);
        Serial.print(", AlarmEnable = ");
        Serial.print(alarmStatus.bAlarmEnable);

        break;
    case 2: // get time
        datetime = rtcc.GetTime();
        Serial.print("# Now --> ");
        Serial.print(datetime);
        break;
    case 3: // set
        res = rtcc.SetTime(uci.getParam());
        if (res == 0)
            Serial.print("? Invalid format time");
        else
            Serial.print("# Set time");
        break;
    case 4: // alarm
        if (uci.getParam() == "on")
        {
            rtcc.SetAlarm(1);
            Serial.print("# Turn on alarm");
        }
        else if (uci.getParam() == "off")
        {
            rtcc.SetAlarm(0);
            Serial.print("# Turn off alarm");
        }
        else
        {
            Serial.print("? Invalid parameter");
        }
        break;
    }
    Serial.print("\n\r>");
    uci.resetCmdId();
}

void serveAlarm(void)
{
    rtcc.ClearAlarm();
    String datetime = rtcc.GetTime();

    Serial.print("**** Alarm **** ");
    Serial.print(datetime);
    Serial.print("\n\r>");
}