#include <Arduino.h>
char rxbuffer[32], rxcount, *rxptr;
//-------------------------------------
void UserCommand();
//=====================================
void setup() {
    delay(3000);
    // H/W initialize
    Serial.begin(115200);
    // Variables and state initialize
    rxcount = 0;
    rxptr = &rxbuffer[0];
    // End of setup   
    Serial.println("\n\r\n\r\n\r#### End of setup ####");
    Serial.print('>');
}
void loop() {
    UserCommand();
}
//#####################################
void SendOut() {
    Serial.print(":");
    rxptr = &rxbuffer[0];
    while(rxcount > 0) {
        Serial.print(char(*rxptr++));
        rxcount--;
    }
    Serial.print("\n\r>");
}
void UserCommand() {
    int rb;
    while (Serial.available() > 0) {
        rb = Serial.read();
        if (rb == 0x0A) {
            SendOut();
            rxcount = 0;
            rxptr = &rxbuffer[0];
        }
        else if (rb >= 0x20) {
            if (rxcount < 32) {
                *rxptr++ = (char)rb;
                rxcount++;
            }
        }
    }
}