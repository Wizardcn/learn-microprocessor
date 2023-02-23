#include "xbUCI.h"

xbUCI::xbUCI(char *allcommand)
{
    vSupportCommand = String(allcommand);
}

void xbUCI::initialize(void)
{
    rxcount = 0;
    rxptr = &rxbuffer[0];
    cmdID = 0;
    Serial.print("\n\r******** User Command Interface ********\n\r");
    Serial.print("\n\r>");
}

void xbUCI::run(char rb)
{
    if (rb == 0x0A)
    {
        *rxptr = 0;
        rxcount = 0;
        rxptr = &rxbuffer[0];
        cmdID = check();
        if (cmdID == 0)
            Serial.print("\n\r>");
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

char xbUCI::check(void)
{
    String vcmd, rcmd;
    int i, j, m;
    rcmd = String(rxbuffer);
    rcmd.trim();
    i = rcmd.indexOf(' ');
    if (i > -1)
    {
        vcmd = rcmd.substring(0, i);
        rcmd = rcmd.substring(i + 1);
        rcmd.trim();
        parameter = rcmd;
    }
    else
    {
        vcmd = rcmd;
    }
    m = 1;
    i = 0;
    do
    {
        j = vSupportCommand.indexOf('_', i);
        if (j < 0)
            return 0;
        rcmd = vSupportCommand.substring(i, j);
        if (rcmd.compareTo(vcmd) == 0)
            return m;
        i = j + 1;
        m++;
    } while (j > 0);
    return 0;
}
