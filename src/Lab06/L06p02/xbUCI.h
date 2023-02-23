#include <Arduino.h>

class xbUCI
{
public:
    xbUCI(char *allcommand);
    void initialize(void);
    void run(char rb);
    u_int8_t getCmdId(void);
    bool isUserCmd(void);
    void resetCmdId(void);
    String getParam(void);

private:
    char rxbuffer[32];
    char rxcount;
    char *rxptr;
    String parameter;
    String vSupportCommand;
    uint8_t cmdID;
    char check(void);
};
