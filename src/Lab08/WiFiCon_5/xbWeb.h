#ifndef XBWEB_H
#define XBWEB_H
#include <Arduino.h>
#include <ESP8266WiFi.h>
class xbWeb
{
public:
    xbWeb(char *ssid, char *password, char *host, char *id, char *key);
    void reconnect(void);
    void disconnect(void);
    int status(void);
    int web_connect(String query);
    String response;
    String parameter;
private:
    char *pssid;
    char *ppassword;
    char *phost;
    char *pid;
    char *pkey;
    WiFiClient client;
};
#endif
