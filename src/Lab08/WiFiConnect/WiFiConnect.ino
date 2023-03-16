#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "xbUCI.h"
//const char* ssid = "@JumboPlusIoT";
//const char* password ="ilovecmu";
const char* ssid = "SUPERMan_";
const char* password ="SUPAWAT123";

xbUCI uci("wifi_check_off_");
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
    byte res;
    switch (uci.cmdID)
    {
    case 1: //wifi
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        Serial.print("# wifi begin...");
        break;
    case 2: //check
        Serial.print("# check ");
        res = WiFi.status();
        Serial.print(res);
        if (res == 3) {
            Serial.print("\n\r SSID: ");
            Serial.print(WiFi.SSID());
            Serial.print("\n\r IP: ");
            Serial.print(WiFi.localIP());
            Serial.print("\n\r RSSI: ");
            Serial.print(WiFi.RSSI());
            Serial.print("\n\r MAC: ");
            Serial.print(WiFi.macAddress());
        }
        break;
    case 3: //off
        WiFi.disconnect();
        Serial.print("# wifi off");
        break;
    }
    Serial.print("\n\r>");
    uci.cmdID = 0;
}
