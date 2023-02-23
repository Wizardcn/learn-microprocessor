#include "xbRTCC.h"

xbRTCC::xbRTCC()
{
    alarmStatus.bConnected = 0;
    alarmStatus.bValid = 0;
    alarmStatus.bAlarmEnable = 0;
}

alarmStatus xbRTCC::getAlarmStatus(void)
{
    byte res;
    Wire.beginTransmission(DS3231address);
    Wire.write(0x0F);
    res = Wire.endTransmission();
    if (res == 0)
        alarmStatus.bConnected = 1;
    else
        return;

    delay(2);
    Wire.requestFrom(DS3231address, 1);
    res = Wire.read();
    Wire.endTransmission();
    if (res & 0x80)
        alarmStatus.bValid = 0;
    else
        alarmStatus.bValid = 1;
    return alarmStatus;
}
String xbRTCC::GetTime(void)
{
    if (alarmStatus.bConnected == 0)
        return;
    if (alarmStatus.bValid == 0)
        return;

    readDateTimeFromDS3231();
    DateTimeToString();
    return sTime;
}

void xbRTCC::readDateTimeFromDS3231(void)
{
    Wire.beginTransmission(DS3231address);
    Wire.write(0x00);
    Wire.endTransmission();
    delay(2);
    Wire.requestFrom(DS3231address, 7);
    datetime.bcdSecond = Wire.read();
    datetime.bcdMinute = Wire.read();
    datetime.bcdHour = Wire.read();
    Wire.read();
    datetime.bcdDate = Wire.read();
    datetime.bcdMonth = Wire.read();
    datetime.bcdYear = Wire.read();
    Wire.endTransmission();
}
uint8_t xbRTCC::SetTime(String s)
{
    if (alarmStatus.bConnected == 0)
        return 0;
    if (s.length() != 19)
        return 0;
    if (s.charAt(4) != '-')
        return 0;
    if (s.charAt(7) != '-')
        return 0;
    if (s.charAt(10) != ' ')
        return 0;
    if (s.charAt(13) != ':')
        return 0;
    if (s.charAt(16) != ':')
        return 0;
    datetime.bcdYear = 16 * (s.charAt(2) & 0x0F);
    datetime.bcdYear += s.charAt(3) & 0x0F;
    datetime.bcdMonth = 16 * (s.charAt(5) & 0x0F);
    datetime.bcdMonth += s.charAt(6) & 0x0F;
    datetime.bcdDate = 16 * (s.charAt(8) & 0x0F);
    datetime.bcdDate += s.charAt(9) & 0x0F;
    datetime.bcdHour = 16 * (s.charAt(11) & 0x0F);
    datetime.bcdHour += s.charAt(12) & 0x0F;
    datetime.bcdMinute = 16 * (s.charAt(14) & 0x0F);
    datetime.bcdMinute += s.charAt(15) & 0x0F;
    datetime.bcdSecond = 16 * (s.charAt(17) & 0x0F);
    datetime.bcdSecond += s.charAt(18) & 0x0F;

    Wire.beginTransmission(DS3231address);
    Wire.write(0x00);
    Wire.write(datetime.bcdSecond);
    Wire.write(datetime.bcdMinute);
    Wire.write(datetime.bcdHour);
    Wire.write(0x01);
    Wire.write(datetime.bcdDate);
    Wire.write(datetime.bcdMonth);
    Wire.write(datetime.bcdYear);
    Wire.endTransmission();
    delay(2);
    alarmStatus.bValid = 1;
    ClearAlarm();
    return 1;
}
void xbRTCC::ClearAlarm(void)
{
    if (alarmStatus.bConnected == 0)
        return;
    if (alarmStatus.bValid == 0)
        return;
    Wire.beginTransmission(DS3231address);
    Wire.write(0x0F);
    Wire.write(0x00);
    Wire.endTransmission();
}
void xbRTCC::SetAlarm(char en)
{
    byte mode;
    if (alarmStatus.bConnected == 0)
    {
        alarmStatus.bAlarmEnable = 0;
        return;
    }
    if (alarmStatus.bValid == 0)
    {
        alarmStatus.bAlarmEnable = 0;
        return;
    }
    if (en == 0)
    {
        mode = 0x1C;
        alarmStatus.bAlarmEnable = 0;
    }
    else
    {
        mode = 0x1D;
        alarmStatus.bAlarmEnable = 1;
        Wire.beginTransmission(DS3231address);
        Wire.write(0x07);
        Wire.write(0x00);
        Wire.write(0x81);
        Wire.write(0x81);
        Wire.write(0x81);
        Wire.endTransmission();
        delay(2);
    }
    Wire.beginTransmission(DS3231address);
    Wire.write(0x0E);
    Wire.write(mode);
    Wire.write(0x00);
    Wire.endTransmission();
}

void xbRTCC::DateTimeToString(void)
{
    char bin[20];
    char *p;
    p = &bin[0];
    *p++ = '2';
    *p++ = '0';
    *p++ = (datetime.bcdYear >> 4) | 0x30;
    *p++ = (datetime.bcdYear & 0x0F) | 0x30;
    *p++ = '-';
    *p++ = (datetime.bcdMonth >> 4) | 0x30;
    *p++ = (datetime.bcdMonth & 0x0F) | 0x30;
    *p++ = '-';
    *p++ = (datetime.bcdDate >> 4) | 0x30;
    *p++ = (datetime.bcdDate & 0x0F) | 0x30;
    *p++ = ' ';
    *p++ = (datetime.bcdHour >> 4) | 0x30;
    *p++ = (datetime.bcdHour & 0x0F) | 0x30;
    *p++ = ':';
    *p++ = (datetime.bcdMinute >> 4) | 0x30;
    *p++ = (datetime.bcdMinute & 0x0F) | 0x30;
    *p++ = ':';
    *p++ = (datetime.bcdSecond >> 4) | 0x30;
    *p++ = (datetime.bcdSecond & 0x0F) | 0x30;
    *p = 0;
    sTime = String(bin);
}

bool xbRTCC::alarmIsReady(bool alarmPin)
{
    return (alarmStatus.bAlarmEnable == 1) && (digitalRead(alarmPin) == 0);
}