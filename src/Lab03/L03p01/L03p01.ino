#include <Arduino.h>
//-------------------------------------
void UserCommand();
//=====================================
void setup()
{
    delay(3000);
    // H/W initialize
    Serial.begin(115200);
    // Variables and state initialize

    // End of setup
    Serial.println("\n\r\n\r\n\r#### End of setup ####");
}
void loop()
{
    UserCommand();
}
// #####################################
void UserCommand()
{
    int rb;
    if (Serial.available() > 0)
    {
        rb = Serial.read();
        Serial.print("\n> ");
        Serial.print(char(rb));
        Serial.print(" ");
        Serial.println(rb, HEX);
    }
}
