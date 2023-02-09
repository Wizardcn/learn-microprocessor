#include <Arduino.h>
#include <Wire.h>
#define BH1750address 0x23
#define PERIOD_TASK 2000
unsigned long tick_last;
byte rqtask;
//-------------------------------------
void readBH1750();
void myschedule();
//=====================================
void setup()
{
  delay(3000);
  // H/W initialize
  Serial.begin(115200);

  Wire.begin(); // initail i2c
  // this three lines below is to initail BH1750
  Wire.beginTransmission(BH1750address);
  Wire.write(0x10);       // set up mode (data sheet)
  Wire.endTransmission(); //
  // Variables and state initialize
  tick_last = millis(); // for sceduler
  // End of setup
  Serial.println("\n\r\n\r\n\r#### End of setup ####");
}
void loop()
{
  myschedule();
  if (rqtask == 1)
  {
    rqtask = 0;
    readBH1750();
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
  // schedule of task
  tick_last += PERIOD_TASK;
  rqtask = 1;
}
void readBH1750()
{
  byte i, buff[2];
  unsigned int rodata;
  Serial.print("Task: read data from BH1750, ");
  Wire.beginTransmission(BH1750address);
  Wire.requestFrom(BH1750address, 2); // request 2 bytes
  i = 0;
  while (Wire.available()) //
  {
    buff[i] = Wire.read(); // receive one byte
    i++;
  }
  Wire.endTransmission();
  if (i == 2)
  {
    rodata = buff[0] * 256 + buff[1];
    Serial.print(" Readout = ");
    Serial.print(rodata, HEX);
    Serial.print("\n\r");
  }
  else
  {
    Serial.println("!reading fails! \n\r");
  }
}
