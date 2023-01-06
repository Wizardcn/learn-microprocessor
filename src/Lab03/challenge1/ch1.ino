#include <Arduino.h>
#include <SPI.h>
#include "myMAX7219.h"
#include "minifont.h"

const int CS_PIN = 12;
const int DIN_PIN = 13;
const int CLK_PIN = 14;

#define PERIOD_TASK 250
unsigned long tick_last;
byte rqtask;
byte tblink, dismode, symbol;

char rxbuffer[32], rxcount, *rxptr;
char fgCommandIn;
//-------------------------------------
void taskDrawBlock();
void myschedule();
void UserCommand();
void UserService();
//=====================================
void setup()
{
    delay(3000);
    // H/W initialize
    Serial.begin(115200);
    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV16);
    SPI.setDataMode(SPI_MODE0);
    pinMode(CS_PIN, OUTPUT);
    digitalWrite(CS_PIN, HIGH);
    MAX7219_init();
    // Variables and state initialize
    rxcount = 0;
    rxptr = &rxbuffer[0];
    tick_last = millis(); // for sceduler
    rqtask = 0;
    tblink = 0;
    dismode = 0;
    // End of setup
    MAX7219_write_reg(REG_SHUTDOWN, 0x01);
    Serial.println("\n\r\n\r\n\r#### End of setup ####");
    Serial.print('>');
}
void loop()
{
    UserCommand();
    myschedule();
    if (rqtask == 1)
    {
        rqtask = 0;
        taskDrawBlock();
    }
    if (fgCommandIn == 1)
        UserService();
}
// #####################################
void myschedule()
{
    unsigned long tnow, tdiff;
    tnow = millis();
    if (tnow > tick_last)
    {
        tdiff = tnow - tick_last;
    }
    else if (tnow < tick_last)
    {
        tdiff = 0xFFFFFFFF;
        tdiff -= tick_last;
        tdiff += tnow;
        tdiff++;
    }
    else
        return;
    if (tdiff < PERIOD_TASK)
        return;
    // schedule of task
    tick_last += PERIOD_TASK;
    rqtask = 1;
}

void taskDrawBlock()

{

    int i;
    switch (dismode)
    {
    case 1:
        tblink ^= 1;
        break;
    case 2:
        tblink = 0;
        break;
    case 3:
        tblink = 1;
        break;
    }
    if (tblink == 1)
    {
        for (i = 0; i < 8; i++)
        {
            MAX7219_write_reg(REG_DIGIT(i), pattern[symbol][i]);
        }
    }
    else
    {
        for (i = 0; i < 8; i++)
        {
            MAX7219_write_reg(REG_DIGIT(i), 0);
        }
    }
}

void UserCommand()
{
    int rb;
    while (Serial.available() > 0)
    {
        rb = Serial.read();
        if (fgCommandIn == 1)
            return;
        if (rb == 0x0A)
        {
            *rxptr = 0;
            rxcount = 0;
            rxptr = &rxbuffer[0];
            fgCommandIn = 1;
        }
        else if (rb == 0x08)
        {
            if (rxcount > 0)
            {
                Serial.print(' ');
                Serial.print(char(8));
                rxcount--;
                rxptr--;
            }
            else
            {
                Serial.print('>');
            }
        }
        else if (rb >= 0x20)
        {
            if (rxcount < 32)
            {
                *rxptr++ = (char)rb;
                rxcount++;
            }
        }
    }
}

const char NoOfSupportCMD = 7;

const char *support_cmd[] = {
    "blink",
    "off",
    "on",
    "get",
    "next",
    "back",
    "help"};

void UserService()
{
    String vcmd;
    char i, m;
    fgCommandIn = 0;
    vcmd = String(rxbuffer);
    m = 0;
    i = 0;
    while ((i < NoOfSupportCMD) && (m == 0))
    {
        if (vcmd.compareTo(support_cmd[i]) == 0)
            m = i + 1;
        else
            i++;
    }
    switch (m)
    {
    case 1:
        dismode = 1;
        Serial.println(": Blinking LED");
        break;
    case 2:
        dismode = 2;
        Serial.println(": Off LED");
        break;
    case 3:
        dismode = 3;
        Serial.println(": On LED");
        break;
    case 4:
        Serial.print(": Current mode is ");
        if (dismode == 1)
            Serial.println("Blinking.");
        else if (dismode == 2)
            Serial.println("Off.");
        else
            Serial.println("On.");
        break;
    case 5:
        Serial.println(": Next symbol");
        // Serial.println(sizeof(pattern));
        if (symbol < 5 && symbol >= 0)
        {
            symbol++;
        }
        else
        {
            symbol = 0;
        }

        break;
    case 6:
        Serial.println(": Back to last symbol");
        // Serial.println(sizeof(pattern));
        if (symbol < 6 && symbol >= 1)
        {
            symbol--;
        }
        else
        {
            symbol = 5;
        }
        break;
    case 7:
        Serial.println(": Commands");
        Serial.println("Commands are available:\n\r\
                        - blink: use to blink the LED\n\
                        - on: use to on the LED\n\
                        - off: use to switch off the LED\n\
                        - next: use to skip to the next symbol\n\
                        - back: use to back to the last symbol");
        break;
    default:
        Serial.println(": Invalid command");
        break;
    }
    Serial.print("\n\r>");
}