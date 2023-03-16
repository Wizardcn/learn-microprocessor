#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "xbUCI.h"
#include "xbRTCC.h"
#include "xbWeb.h"

#define BH1750address  0x23
#define RTCC_ALARM_PIN 16
xbUCI uci("check_get_sync_set_alarm_read_wifi_connect_");
xbRTCC rtcc;
xbWeb myw("SUPERMan_","SUPAWAT123","www.pandaworker.com", "620610230", "F8A12508");
unsigned int rodata;
//###############################################
void service(void);
void serveAlarm(void);
void setup()
{
    delay(3000);
    // H/W initialize ---------------------------
    Serial.begin(9600);
    Wire.begin();
    Wire.beginTransmission(BH1750address);
    Wire.write(0x10);
    Wire.endTransmission();

    pinMode(RTCC_ALARM_PIN, INPUT);
    // Variables and state initialize -----------
    uci.initialize();
    Serial.println("# WiFi begin...");
    myw.reconnect();
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

void readBH1750() {
    byte i, buff[2];
    Serial.print("Task: read data from BH1750, ");
    Wire.beginTransmission(BH1750address);
    Wire.requestFrom(BH1750address, 2);
    i = 0;
    while(Wire.available()) //
    {
      buff[i] = Wire.read();  // receive one byte
      i++;
    }
    Wire.endTransmission();
    if (i == 2) {
      rodata = buff[0]*256 + buff[1];
      Serial.print(" Readout = ");
      Serial.print(rodata);
      Serial.print("\n\r");
    }
    else {
      Serial.println("!reading fails! \n\r");
    }
}

void service(void)
{
    byte res;
    String time;
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
        Serial.print(", WiFi = ");
        Serial.print(myw.status());
        break;
    case 2: //get time

        res = myw.web_connect("cmd=rtcc");
        if (res == 0) {
            // local time
            rtcc.GetTime();
            rtcc.TimeToString();
            Serial.print("# Now (Local) --> ");
            Serial.print(rtcc.sTime);

            time = myw.parameter;
            time.replace(",", " ");
            time.trim();
            time = "20" + time;

            // global time
            Serial.print("\n# Now (Global) --> ");
            Serial.print(time);
        } else
            Serial.print("\n! error to fetch time on global rtcc");
            break;
        break;
    case 3: // sync

        Serial.print("# Web connect, request to global rtcc to set local rtcc");
        res = myw.web_connect("cmd=rtcc");
        if (res == 0) {
            Serial.print("\n\rResponse:  " + myw.response);
            Serial.print("\n\rParameter: " + myw.parameter);

            time = myw.parameter;
            time.replace(",", " ");
            time.trim();
            time = "20" + time;

            res = rtcc.SetTime(time);
            if (res == 0)
                Serial.print("\n? Cannot process the time from server, Invalid format time");
            else
                Serial.print("\n#  Finished sync");             
                
            }
        else
            Serial.print("! error to set time on local rtcc");
            break;
        break;

    case 4: // set 2000-00-00 12:00:00
        res = rtcc.SetTime(uci.parameter);
        if (res == 0)
            Serial.print("? Invalid format time");
        else
            Serial.print("# Set time");    
        break;
    case 5: //alarm
        if (uci.parameter == "on") 
        {
            rtcc.SetAlarm(1);
            Serial.print("# Turn on alarm");
        }
        else if (uci.parameter == "off") 
        {
            rtcc.SetAlarm(0);
            Serial.print("# Turn off alarm");
        }
        else {
            Serial.print("? Invalid parameter");
        }
        break;
    case 6: //read
        readBH1750();
        break;
    case 7: //wifi
        if (WiFi.status() != WL_CONNECTED) {
            Serial.print("\n\r WiFi is not connected.!");
            Serial.print("\n# wifi begin...");
            myw.reconnect();
        } else if (WiFi.status() == WL_CONNECTED) {
          Serial.print("\n\r WiFi is connected.");
        }
        break;
    case 8: //connect 
        Serial.print("# Web connect, for test");
        res = myw.web_connect("cmd=connect");
        if (res == 0) {
            Serial.print("\n\rResponse: " + myw.response);
        }
        else
            Serial.print("! error");
        break;        
    }
    Serial.print("\n\r>");
    uci.cmdID = 0;
}




void serveAlarm(void) {
    int res;
    String str;
    rtcc.ClearAlarm();
    rtcc.GetTime();
    rtcc.TimeToString();
    Serial.print("**** Alarm **** ");
    Serial.print(rtcc.sTime);
    Serial.print("\n\r>");
    delay(40);
    readBH1750();
    
    str = String("cmd=add&ch=2&dat=" + String(rodata) + "&tst=" + rtcc.sTime.substring(2,10) + "%20" + rtcc.sTime.substring(11));
    //Serial.println(str);
    
    res = myw.web_connect(str);
    if (res == 0) {
        Serial.print("\n\rResponse:  " + myw.response);
        Serial.print("\n\rParameter: " + myw.parameter);
    }
    else
        Serial.print("! error");

    Serial.println("\n\r>");
}