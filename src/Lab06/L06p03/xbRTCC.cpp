#include "xbRTCC.h"

xbRTCC::xbRTCC()
{
    bConnected = 0;
    bValid = 0;
    bAlarmEnable = 0;
}

void xbRTCC::Check(void)
{
    byte res;
    Wire.beginTransmission(DS3231address);
    Wire.write(0x0F);
    res = Wire.endTransmission();
    if (res == 0)
        bConnected = 1;
    else
        return;
    
    delay(2);
    Wire.requestFrom(DS3231address, 1);
    res = Wire.read();
    Wire.endTransmission();
    if (res & 0x80)
        bValid = 0;
    else
        bValid = 1;
}
void xbRTCC::GetTime(void)
{
    if (bConnected == 0) return;
    if (bValid == 0) return;
    Wire.beginTransmission(DS3231address);
    Wire.write(0x00);
    Wire.endTransmission();
    delay(2);
    Wire.requestFrom(DS3231address, 7);
    bcdSecond = Wire.read();
    bcdMinute = Wire.read();
    bcdHour = Wire.read();
    Wire.read();
    bcdDate = Wire.read();
    bcdMonth = Wire.read();
    bcdYear = Wire.read();
    Wire.endTransmission();
}
uint8_t xbRTCC::SetTime(String s)
{
    if (bConnected == 0) return 0;
    if (s.length() != 19) return 0;
    if (s.charAt(4) != '-') return 0;
    if (s.charAt(7) != '-') return 0;
    if (s.charAt(10) != ' ') return 0;
    if (s.charAt(13) != ':') return 0;
    if (s.charAt(16) != ':') return 0;
    bcdYear = 16*(s.charAt(2) & 0x0F);
    bcdYear += s.charAt(3) & 0x0F;
    bcdMonth = 16*(s.charAt(5) & 0x0F);
    bcdMonth += s.charAt(6) & 0x0F;
    bcdDate = 16*(s.charAt(8) & 0x0F);
    bcdDate += s.charAt(9) & 0x0F;
    bcdHour = 16*(s.charAt(11) & 0x0F);
    bcdHour += s.charAt(12) & 0x0F;
    bcdMinute = 16*(s.charAt(14) & 0x0F);
    bcdMinute += s.charAt(15) & 0x0F;
    bcdSecond = 16*(s.charAt(17) & 0x0F);
    bcdSecond += s.charAt(18) & 0x0F;

    Wire.beginTransmission(DS3231address);
    Wire.write(0x00);
    Wire.write(bcdSecond);
    Wire.write(bcdMinute);
    Wire.write(bcdHour);
    Wire.write(0x01);
    Wire.write(bcdDate);
    Wire.write(bcdMonth);
    Wire.write(bcdYear);
    Wire.endTransmission();
    delay(2);
    bValid = 1;
    ClearAlarm();
    return 1;
}
void xbRTCC::ClearAlarm(void)
{
    if (bConnected == 0) return;
    if (bValid == 0) return;
    Wire.beginTransmission(DS3231address);
    Wire.write(0x0F);
    Wire.write(0x00);
    Wire.endTransmission();
}
void xbRTCC::SetAlarm(char en)
{
    byte mode;
    if (bConnected == 0) {
        bAlarmEnable = 0;
        return;
    }
    if (bValid == 0) {
        bAlarmEnable = 0;
        return;
    }
    if (en == 0) {
        mode = 0x1C;
        bAlarmEnable = 0;
    }
    else {
        mode = 0x1D;
        bAlarmEnable = 1;
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

void xbRTCC::TimeToString(void) {
    char bin[20];
    char *p;
    p = &bin[0];
    *p++ = '2'; *p++ = '0';
    *p++ = (bcdYear >> 4) | 0x30;
    *p++ = (bcdYear & 0x0F) | 0x30;
    *p++ = '-';
    *p++ = (bcdMonth >> 4) | 0x30;
    *p++ = (bcdMonth & 0x0F) | 0x30;
    *p++ = '-';
    *p++ = (bcdDate >> 4) | 0x30;
    *p++ = (bcdDate & 0x0F) | 0x30;
    *p++ = ' ';
    *p++ = (bcdHour >> 4) | 0x30;
    *p++ = (bcdHour & 0x0F) | 0x30;
    *p++ = ':';
    *p++ = (bcdMinute >> 4) | 0x30;
    *p++ = (bcdMinute & 0x0F) | 0x30;
    *p++ = ':';
    *p++ = (bcdSecond >> 4) | 0x30;
    *p++ = (bcdSecond & 0x0F) | 0x30;
    *p = 0;
    sTime = String(bin);
}