#include <TaskScheduler.h>
#include <TaskSchedulerDeclarations.h>
#include <TaskSchedulerSleepMethods.h>

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "myMAX7219.h"

#define BH1750address 0x23

const int CS_PIN = 12;
const int DIN_PIN = 13;
const int CLK_PIN = 14;

char rxbuffer[32], rxcount, *rxptr;
byte fgCommandIn, fgUpdate, vDriver;
int parameter;
unsigned int level;
//-----------------------------------------------
void Process();
void UserCommand();
void UserService();
void DriveOutput();
//===============================================
void setup()
{
  delay(3000);
  // H/W initialize -----------------------------
  Serial.begin(115200);

  // Initialize I2C
  Wire.begin();
  Wire.beginTransmission(BH1750address);
  Wire.write(0x10);
  if (Wire.endTransmission() != 0)
  {
    Serial.println("\n\r I2C fails!!!!");
    while (1)
      ;
  }

  // Initialize SPI
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE0);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  MAX7219_init();

  // Variables and state initialize -------------
  rxcount = 0;
  rxptr = &rxbuffer[0];

  // End of setup -------------------------------
  Serial.println("\n\r\n\r#### End of setup ####");
}

void loop()
{
  UserCommand();
  if (fgCommandIn == 1)
    UserService();
  if (fgUpdate == 1)
    Process();
}

// ###############################################
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

const char *support_cmd[] = {
    "set",
    "get"};

void UserService()
{
  String vcmd, rcmd;
  char i, m;
  fgCommandIn = 0;

  rcmd = String(rxbuffer);
  rcmd.trim();
  i = rcmd.indexOf(' ');
  if (i > -1)
  {
    vcmd = rcmd.substring(0, i);
    rcmd = rcmd.substring(i + 1);
    rcmd.trim();
    parameter = rcmd.toInt();
  }
  else
  {
    vcmd = rcmd;
  }

  m = 0;
  i = 0;
  while ((i < 2) && (m == 0))
  {
    if (vcmd.compareTo(support_cmd[i]) == 0)
      m = i + 1;
    else
      i++;
  }

  switch (m)
  {
  case 1:
    if (parameter > 64)
      parameter = 64;
    else if (parameter < 0)
      parameter = 0;
    vDriver = parameter;
    fgUpdate = 1;
    break;
  case 2:
    Serial.println("get command");
    Serial.print("\n\r>");
    break;
  default:
    Serial.println("! Invalid command");
    Serial.print("\n\r>");
    break;
  }
}
void Process()
{
  int buff[2], i = 0;
  fgUpdate = 0;
  // Read back sensor ----------------------------
  Wire.beginTransmission(BH1750address);
  Wire.requestFrom(BH1750address, 2);
  while (Wire.available())
  {
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
  if (i == 2)
    level = buff[0] * 256 + buff[1];
  else
  {
    Serial.println("!Fails");
    Serial.print("\n\r>");
    return;
  }
  //-------------------------------------------
  // function
  //-------------------------------------------
  DriveOutput();
  //-------------------------------------------
  Serial.print("Get Level = ");
  Serial.print(level);
  Serial.print(", then Drive Output = ");
  Serial.print(vDriver);
  Serial.print("\n\r\n\r>");
}
void DriveOutput()
{
  // input: vDriver (global variable)
  const byte cenix[8] = {0x14, 0x15, 0x13, 0x16, 0x12, 0x17, 0x11, 0x18};
  int i, j, c;
  byte pat, bg;
  pat = vDriver;

  for (i = 0; i < 8; i++)
  {
    bg = 0;
    if (j < vDriver)
    {
      for (c = 0; c < 8; c++)
      {
        bg = bg << 1;
        if (j < vDriver)
          bg |= 1;
        j++;
      }
      pat = bg;
    }
    else
    {
      pat = 0;
      j += 8;
    }
    MAX7219_write_reg(cenix[i], pat);
  }
}
