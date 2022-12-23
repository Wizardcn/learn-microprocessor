#include <Arduino.h>
#include <SPI.h>
#include "myMAX7219.h"

const int CS_PIN = 12;
const int DIN_PIN = 13;
const int CLK_PIN = 14;

#define PERIOD_TASK 1000
unsigned long tick_last;
byte rqtask;

//-------------------------------------
void taskDrawBlock();
void myschedule();
//=====================================
void setup()
{
  delay(3000);
  // H/W initialize
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE0);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  MAX7219_init();
  // Variables and state initialize
  tick_last = millis();
  // End of setup
  MAX7219_write_reg(REG_SHUTDOWN, 0x01);
}
void loop()
{
  myschedule();
  if (rqtask == 1)
  {
    rqtask = 0;
    taskDrawBlock();
  }
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
  //-----------------------------------
  tick_last += PERIOD_TASK;
  rqtask = 1;
}
const byte pattern[8] = {
    0B00111000,
    0B00111000,
    0B00111000,
    0B00111000,
    0B11111110,
    0B01111100,
    0B00111000,
    0B00010000};
void taskDrawBlock()
{
  int i;
  for (i = 0; i < 8; i++)
  {
    MAX7219_write_reg(REG_DIGIT(i), pattern[i]);
  }
}
