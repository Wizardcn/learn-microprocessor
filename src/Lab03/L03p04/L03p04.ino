#include <Arduino.h>
char rxbuffer[32], rxcount, *rxptr;
char fgCommandIn;
//-------------------------------------
void UserCommand();
void UserService();
//=====================================
void setup() {
    delay(3000);
    // H/W initialize
    Serial.begin(115200);
    // Variables and state initialize
    rxcount = 0;
    rxptr = &rxbuffer[0];
    // End of setup   
    Serial.println("\n\r\n\r\n\r#### End of setup ###");
    Serial.print('>');
}
void loop() {
    UserCommand();
    if (fgCommandIn == 1) UserService();
}
//#####################################
void UserCommand() {
    int rb;
    while (Serial.available() > 0) {
        rb = Serial.read();
        if (fgCommandIn == 1) return;
        if (rb == 0x0A) {
            *rxptr = 0;
            rxcount = 0;
            rxptr = &rxbuffer[0];
            fgCommandIn = 1;
        }
        else if (rb == 0x08) {
            if (rxcount > 0) {
                Serial.print(' ');
                Serial.print(char(8));
                rxcount--;
                rxptr--;
            }
            else {
                Serial.print('>');
            }
        }
        else if (rb >= 0x20) {
            if (rxcount < 32) {
                *rxptr++ = (char)rb;
                rxcount++;
            }
        }
    }
}
const char NoOfSupportCMD = 4;
const char *support_cmd[] = {
    "blink",
    "off",
    "on",
    "get"
};
void UserService() {
    String vcmd;
    char i, m;
    fgCommandIn = 0;

    vcmd = String(rxbuffer);
    m = 0;
    i = 0;
    while ((i < NoOfSupportCMD) && (m == 0)) {
        if (vcmd.compareTo(support_cmd[i]) == 0) 
            m = i+1;
        else
            i++;
    }
    switch (m) {
        case 1:
        case 2:
        case 3:
        case 4:
            Serial.print(": Command no. = ");
            Serial.println(m, DEC);
            break;
        default:
            Serial.println(": Invalid command");
            break;
    }
    Serial.print("\n\r>");
}
