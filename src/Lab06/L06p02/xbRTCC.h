#ifndef XBRTCC_H
#define XBRTCC_H
#include <Arduino.h>
#include <Wire.h>

#define DS3231address 0x68

struct alarmStatus
{
    uint8_t bConnected;
    uint8_t bValid;
    uint8_t bAlarmEnable;
};

struct DateTime
{
    uint8_t bcdYear;
    uint8_t bcdMonth;
    uint8_t bcdDate;
    uint8_t bcdHour;
    uint8_t bcdMinute;
    uint8_t bcdSecond;
};

class xbRTCC
{
public:
    xbRTCC();
    alarmStatus getAlarmStatus(void);
    String GetTime(void);
    uint8_t SetTime(String s);
    void ClearAlarm(void);
    void SetAlarm(char en);
    bool alarmIsReady(bool alarmPin);

private:
    alarmStatus alarmStatus;
    DateTime datetime;
    String sTime;
    void DateTimeToString(void);
    void xbRTCC::readDateTimeFromDS3231(void);
};

#endif
