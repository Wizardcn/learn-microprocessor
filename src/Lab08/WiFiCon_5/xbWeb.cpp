#include "xbWeb.h"

xbWeb::xbWeb(char *ssid, char *password, char *host, char *id, char *key) {
    pssid = ssid;
    ppassword = password;
    phost = host;
    pid = id;
    pkey = key;
}

void xbWeb::reconnect(int attempt) {
   
  for (int i=0; i<attempt; i++) {
    if (WiFi.status() != WL_CONNECTED) {

      if (i!=0) {
        Serial.println(" TimeOut");
      }
      
      Serial.print("Attempt " + String(i+1));

      WiFi.mode(WIFI_STA);
      WiFi.begin(pssid, ppassword);

      delay(5000);

      
    } else break;
  }
  

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println(" TimeOut");
    Serial.println("Tried 10 times to connect WiFi, cannot access to the Router, Please try again.");
  } else Serial.print("\n\r WiFi is connected.");
            
            
}

void xbWeb::disconnect(void) {
    WiFi.disconnect();
}
int xbWeb::status(void) {
    return WiFi.status();
}

int xbWeb::web_connect(String query) {
    //return value: 0 -> get response from web
    unsigned long start;
    unsigned long request_time;
    String command;
    int divider_idx;

    int ix, iy;
    String qq;
    qq = String("GET /ee/index.php?id="+ String(pid)+ "&key="+ String(pkey) + "&" + query +" HTTP/1.1\r\nHOST: www.pandaworker.com\r\nConnection: close\r\n\r\n");
    if (WiFi.status() != WL_CONNECTED) return 1;
    if (!client.connect(phost, 80)) return 2;

    start = millis();    
    client.print(qq);
    while(client.available() == 0) {
        if (millis() - start > 5000) {
            //Serial.println("Web connect timeout!!!");
            client.stop();
            return 3;
        }
    }

    Serial.print("Finished request, ");
    request_time = millis()-start;
    Serial.print(request_time);
    Serial.println(" ms");

    // divider_idx = query.indexOf("&");
    // if (divider_idx == -1) {
    //   command = query;
    // } else {
    //   command = query.substring(0, divider_idx);
    // }

    // Serial.println(command);
    
    while (client.available()) {
      // if (command == "cmd=add") {
      //   break;
      // } else {
      //   qq = client.readStringUntil('\r');
      // }
      qq = client.readStringUntil('\r');
    }

    Serial.print("Processed response, ");
    Serial.print(millis()-(request_time + start));
    Serial.println(" ms");

    ix = qq.indexOf("__", 0);
    if (ix < 0) return 4;
    ix = ix + 2;
    iy = qq.indexOf(",", ix);
    response = qq.substring(ix,iy);
    parameter = qq.substring(iy+1);
    return 0;
}