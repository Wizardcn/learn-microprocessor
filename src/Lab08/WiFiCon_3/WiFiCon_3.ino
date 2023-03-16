#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "xbUCI.h"
const char* ssid = "SUPERMan_";
const char* password ="SUPAWAT123";

xbUCI uci("wifi_check_off_web_");
WiFiClient client;
//###############################################
void service(void);
void connect_web();
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
    case 2: //status
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
        Serial.print("# off wifi");
        break;
    case 4: //web 
        if (WiFi.status() != WL_CONNECTED) {
            Serial.print("\n\r WiFi is not connected.!");
        }
        else {
            Serial.print("\n\r# Web connecting =================================\n\r");
            connect_web();
        }
        break;
    }
    Serial.print("\n\r>");
    uci.cmdID = 0;
}

void connect_web() {
    unsigned long timeout;
    if (!client.connect("www.pandaworker.com", 80)) {
        Serial.println("Web connect failed!!!");
        return;
    }
    timeout = millis();
    client.print("GET /ee/index.php?id=620610230&key=F8A12508&cmd=rtcc HTTP/1.1\r\nHOST: www.pandaworker.com\r\nConnection: close\r\n\r\n");
    while(client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println("Web connect timeout!!!");
            client.stop();
            return;
        }
    }

    while(client.available()) {
        Serial.print(client.readStringUntil('\r'));
    }
    Serial.println("\n\r# End of Web connecting ==========================");
}