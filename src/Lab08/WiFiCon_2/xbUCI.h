#ifndef XBUCI_H
#define XBUCI_H
#include <Arduino.h>

class xbUCI
{
public:
    xbUCI(char *allcommand);
    void initialize(void);
    void run(char rb);
    uint8_t cmdID;
    String parameter;
private:
    char rxbuffer[32];
    char rxcount;
    char *rxptr;
    String vSupportCommand;
    char check(void);
};

#endif
