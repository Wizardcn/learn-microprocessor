#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "xbUCI.h"
#include "xbWeb.h"

xbUCI uci("wifi_check_connect_rtcc_count_clear_dummy_");
xbWeb myw("SUPERMan_","SUPAWAT123","www.pandaworker.com", "620610230", "F8A12508");
//###############################################
void service(void);

void setup()
{
    delay(3000);
    // H/W initialize ---------------------------
    Serial.begin(9600);
    // Variables and state initialize -----------
    uci.initialize();
    // End of setup -----------------------------
}

void loop()
{
    if (Serial.available())
        uci.run((char)Serial.read());
    if (uci.cmdID > 0)
        service();
}

void service(void)
{
    int res;
    switch (uci.cmdID)
    {
    case 1: //wifi
        Serial.print("# wifi begin...");
        myw.reconnect();
        break;
    case 2: //check
        Serial.print("# check ");
        Serial.print(myw.status());
        break;
    case 3: //connect 
        Serial.print("# Web connect, for test");
        res = myw.web_connect("cmd=connect");
        if (res == 0) {
            Serial.print("\n\rResponse: " + myw.response);
        }
        else
            Serial.print("! error");
        break;
    case 4: //rtcc 
        Serial.print("# Web connect, for RTCC setting");
        res = myw.web_connect("cmd=rtcc");
        if (res == 0) {
            Serial.print("\n\rResponse:  " + myw.response);
            Serial.print("\n\rParameter: " + myw.parameter);
            Serial.print("\nType: ")
        }
        else
            Serial.print("! error");
        break;        
    case 5: //count
        Serial.print("# Web connect, for counting records");
        res = myw.web_connect("cmd=count&ch=1");
        if (res == 0) {
            Serial.print("\n\rResponse:  " + myw.response);
            Serial.print("\n\rParameter: " + myw.parameter);
        }
        else
            Serial.print("! error");
        break;
    case 6: //clear
        Serial.print("# Web connect, for clearing all records");
        res = myw.web_connect("cmd=clear&ch=1");
        if (res == 0) {
            Serial.print("\n\rResponse:  " + myw.response);
            Serial.print("\n\rParameter: " + myw.parameter);
        }
        else
            Serial.print("! error");
        break;
    case 7: //dummy
        Serial.print("# Web connect, for adding a new record");
        res = myw.web_connect("cmd=add&ch=1&dat=254.4&tst=2021-09-21%2022:30:00");
        if (res == 0) {
            Serial.print("\n\rResponse:  " + myw.response);
            Serial.print("\n\rParameter: " + myw.parameter);
        }
        else
            Serial.print("! error");
        break;        
    }
    Serial.print("\n\r>");
    uci.cmdID = 0;
}

