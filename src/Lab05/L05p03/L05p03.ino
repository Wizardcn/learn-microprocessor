#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "myMAX7219.h"
#include <TaskScheduler.h>

#define BH1750address  0x23

const int CS_PIN   = 12;
const int DIN_PIN  = 13;
const int CLK_PIN  = 14;

byte vDriver;
unsigned int level, target, half_band;
signed int state;
//-----------------------------------------------
void Process();
void DriveOutput();
void CalFunction();
Task t1(1000, TASK_FOREVER, &Process);
Scheduler runner;
//===============================================
void setup() {
  delay(3000);
  // H/W initialize -----------------------------
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(BH1750address);
  Wire.write(0x10);
  if (Wire.endTransmission() != 0) {
    Serial.println("\n\r I2C fails!!!!");
    while (1);
  }
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  SPI.setDataMode(SPI_MODE0);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  MAX7219_init();
  // Variables and state initialize -------------
  runner.init();
  runner.addTask(t1);
  delay(500);
  t1.enable();
  target = 200;
  half_band = 10;
  state = 0;
  vDriver = 0;
  // End of setup -------------------------------
  Serial.println("\n\r\n\r#### End of setup ####");
}
void loop() {
    runner.execute();
}
//###############################################
void Process() {
  int buff[2], i = 0;
  
  //Read back sensor ----------------------------
  Wire.beginTransmission(BH1750address);
  Wire.requestFrom(BH1750address, 2);
  while (Wire.available()) {
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
  if (i == 2)
    level = buff[0] * 256 + buff[1];
  else {
    Serial.println("!Fails");
    Serial.print("\n\r>");
    return;
  }
  //-------------------------------------------
  Serial.print("S: ");
  Serial.print(state);
  Serial.print(", Target: ");
  Serial.print(target);
  CalFunction();
  //-------------------------------------------
  DriveOutput();
  //-------------------------------------------
  Serial.print(", Level: ");
  Serial.print(level);
  Serial.print(", Drive: ");
  Serial.print(vDriver);
  Serial.print(", S: ");
  Serial.print(state);
  Serial.print("\n\r\n\r>");
}
void DriveOutput() {
  //input: vDriver (global variable)
  const byte cenix[8] = {0x14, 0x15, 0x13, 0x16, 0x12, 0x17, 0x11, 0x18};
  int i, j, c;
  byte pat, bg;
  pat = vDriver;

  for (i = 0; i < 8; i++) {
    bg = 0;
    if (j < vDriver) {
      for (c = 0; c < 8; c++) {
        bg = bg << 1;
        if (j < vDriver) bg |= 1;
        j++;
      }
      pat = bg;
    }
    else {
      pat = 0;
      j += 8;
    }
    MAX7219_write_reg(cenix[i], pat);
  }
}

void CalFunction() {

  switch (state)
  {
  case 1: //over  
    if (level < target)
      state = 0;
    else
      vDriver--;  
    break;
  case 0: //in band
    if (level < (target - half_band)) 
      state = -1;
    else if (level > (target + half_band)) 
      state = 1; 
    break;
  case -1: //under
    if (level > target)
      state = 0;
    else
      vDriver++;
    break;
  }
}