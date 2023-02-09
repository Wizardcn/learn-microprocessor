#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "myMAX7219.h"
#include <TaskScheduler.h>

#define BH1750address  0x23

const int CS_PIN   = 12;
const int DIN_PIN  = 13;
const int CLK_PIN  = 14;

char rxbuffer[32], rxcount, *rxptr;
byte fgCommandIn;
int parameter;
byte vDriver;
unsigned int level, target, half_band;
signed int state;
int outcount;
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
  rxcount = 0; rxptr = &rxbuffer[0];
  runner.init();
  runner.addTask(t1);
  delay(500);
  t1.disable();
  target = 200;
  half_band = 10;
  state = -1;
  vDriver = 0;
  outcount = 0;
  DriveOutput();
  // End of setup -------------------------------
  Serial.println("\n\r\n\r#### End of setup ####\n\r>");
}
void loop() {
    UserCommand();
    if (fgCommandIn == 1) UserService();
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
  Serial.print("S:");
  Serial.print(state);
  Serial.print(", Target: ");
  Serial.print(target);
  Serial.print(char(241));
  Serial.print(half_band);
  CalFunction();
  //-------------------------------------------
  DriveOutput();
  //-------------------------------------------
  Serial.print(", Level: ");
  Serial.print(level);
  Serial.print(", Drive: ");
  Serial.print(vDriver);
  Serial.print(", S:");
  Serial.print(state);
  Serial.print(", ** ");
  Serial.print(outcount);
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
  case 2: //over  
    if (level < target)
      state = 1;
    else {
      vDriver--;  
      outcount++;
    }
    break;
  case 1: //in band
    if (level < (target - half_band)) {
      state = 0;
      outcount = 0;
    }
    else if (level > (target + half_band)) {
      state = 2; 
      outcount = 0;
    }
    break;
  case 0: //under
    if (level > target)
      state = 1;
    else {
      vDriver++;
      outcount++;
    }
    break;
  }
}

void UserCommand() {
    int rb;
    while (Serial.available() > 0) {
        rb = Serial.read();
        t1.disable();
        state = -1;
        if (fgCommandIn == 1) return;
        if (rb == 0x0A) {
            *rxptr = 0;
            rxcount = 0;
            rxptr = &rxbuffer[0];
            fgCommandIn = 1;
        }
        else if (rb == 0x08) {
            if (rxcount > 0) {
                Serial.print(' ');
                Serial.print(char(8));
                rxcount--;
                rxptr--;
            }
            else {
                Serial.print('>');
            }
        }
        else if (rb >= 0x20) {
            if (rxcount < 32) {
                *rxptr++ = (char)rb;
                rxcount++;
            }
        }
    }
}
const char *support_cmd[] = {
    "stop",
    "run",
    "target",   //target
    "halfband" //half_band
};
void UserService() {
    String vcmd, rcmd;
    char i, m;
    fgCommandIn = 0;

    rcmd = String(rxbuffer);
    rcmd.trim();
    i = rcmd.indexOf(' ');
    if (i > -1) {
      vcmd = rcmd.substring(0, i);
      rcmd = rcmd.substring(i+1);
      rcmd.trim();
      parameter = rcmd.toInt();
    }
    else {
      vcmd = rcmd;
    }

    m = 0;
    i = 0;
    while ((i < 4) && (m == 0)) {
        if (vcmd.compareTo(support_cmd[i]) == 0) 
            m = i+1;
        else
            i++;
    }

    switch (m) {
      case 1: //stop
        t1.disable();
        state = -1;
        Serial.println("... Stop ....");
        Serial.print("\n\r>");
        vDriver = 0;
        DriveOutput();
        break;
      case 2: // run
        t1.enable();
        state = 0;
        Serial.println("*** Start ***");
        Serial.print("\n\r>");
        break;
      case 3: // target
        target = parameter;
        Serial.print("target = ");
        Serial.print(target);
        Serial.print("\n\r>");
        state = -1;
        t1.disable();
        break;
      case 4: // halfband
        half_band = parameter;
        Serial.print("half band = ");
        Serial.print(half_band);
        Serial.print("\n\r>");
        state = -1;
        t1.disable();
        break;
      default:
        Serial.println("! Invalid command");
        Serial.print("\n\r>");
        break;
    }
}