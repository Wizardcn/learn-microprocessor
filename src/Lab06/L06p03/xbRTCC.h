#ifndef XBRTCC_H
#define XBRTCC_H
#include <Arduino.h>
#include <Wire.h>

#define DS3231address 0x68

class xbRTCC
{
public:
    xbRTCC();
    void Check(void);
    void GetTime(void);
    uint8_t SetTime(String s);
    void ClearAlarm(void);
    void SetAlarm(char en);
    void TimeToString(void);
    uint8_t bAlarmEnable;
    uint8_t bValid;
    uint8_t bcdYear;
    uint8_t bcdMonth;
    uint8_t bcdDate;
    uint8_t bcdHour;
    uint8_t bcdMinute;
    uint8_t bcdSecond;
    uint8_t bConnected;
    String sTime;
private:
    
};

#endif
