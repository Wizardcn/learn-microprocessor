#include <Arduino.h>
#include <SPI.h>
#include "myMAX7219.h"
#include "minifont.h"

const int CS_PIN = 12;
const int DIN_PIN = 13;
const int CLK_PIN = 14;

#define PERIOD_TASK 250
unsigned long tick_last;
byte rqtask, rqCount, preCount;
byte sym_idx, tblink;

//-------------------------------------
void taskSelectSymbol();
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
  rqtask = 0;
  rqCount = 0;
  preCount = 0;
  sym_idx = 2;
  tblink = 0;
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
  if (rqCount == 1)
  {
    rqCount = 0;
    taskSelectSymbol();
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

  // do this statement below if tdiff is greater than or equal to PERIOD_TASK
  tick_last += PERIOD_TASK;
  rqtask = 1;
  preCount++;
  if (preCount == 8)
  {
    preCount = 0;
    rqCount = 1;
  }
}

void taskDrawBlock()
{
  int i;
  if (tblink == 1)
  {
    for (i = 0; i < 8; i++)
    {
      MAX7219_write_reg(REG_DIGIT(i), pattern[sym_idx][i]);
    }
  }
  else
  {
    for (i = 0; i < 8; i++)
    {
      MAX7219_write_reg(REG_DIGIT(i), 0);
    }
  }
  tblink ^= 1; // tblick = tblink xor 1 (bitwise)
}
void taskSelectSymbol()
{
  sym_idx++;
  if (sym_idx == 6)
    sym_idx = 2;
}
