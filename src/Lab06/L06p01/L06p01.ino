#include <Arduino.h>
#include "xbUCI.h"
xbUCI uci("check_get_set_"); // initialize xbUCI object name "uci"
// ###############################################
void service(void);

void setup()
{
    delay(3000);
    // H/W initialize ---------------------------
    Serial.begin(115200);
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
}

void service(void)
{
    switch (uci.getCmdId())
    {
    case 1: // check
        Serial.println("# Service for 'check' command");
        break;
    case 2: // get
        Serial.println("# Service for 'get' command");
        break;
    case 3: // set
        Serial.println("# Service for 'set' command");
        break;
    }
    Serial.print("\n\r>");
    uci.resetCmdId();
}