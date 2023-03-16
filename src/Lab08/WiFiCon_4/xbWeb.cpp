#include "xbWeb.h"

xbWeb::xbWeb(char *ssid, char *password, char *host, char *id, char *key) {
    pssid = ssid;
    ppassword = password;
    phost = host;
    pid = id;
    pkey = key;
}

void xbWeb::reconnect(void) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(pssid, ppassword);
}

void xbWeb::disconnect(void) {
    WiFi.disconnect();
}
int xbWeb::status(void) {
    return WiFi.status();
}

int xbWeb::web_connect(String query) {
    //return value: 0 -> get response from web
    unsigned long timeout;
    int ix, iy;
    String qq;
    qq = String("GET /ee/index.php?id="+ String(pid)+ "&key="+ String(pkey) + "&" + query +" HTTP/1.1\r\nHOST: www.pandaworker.com\r\nConnection: close\r\n\r\n");
    if (WiFi.status() != WL_CONNECTED) return 1;
    if (!client.connect(phost, 80)) return 2;
    timeout = millis();    
    client.print(qq);
    while(client.available() == 0) {
        if (millis() - timeout > 5000) {
            //Serial.println("Web connect timeout!!!");
            client.stop();
            return 3;
        }
    }
    while(client.available()) {
        qq = client.readStringUntil('\r');
    }

    ix = qq.indexOf("__", 0);
    if (ix < 0) return 4;
    ix = ix + 2;
    iy = qq.indexOf(",", ix);
    response = qq.substring(ix,iy);
    parameter = qq.substring(iy+1);
    return 0;
}