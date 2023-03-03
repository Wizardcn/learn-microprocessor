#include <Arduino.h>
const int pingPin = 15; // Trig
const int inPin = 13;   // Echo
#define PERIOD_TASK 2000
unsigned long tick_last;
byte rqtask;

//-------------------------------------
void readHCSR04();
void myschedule();
//=====================================
void setup()
{
  delay(3000);
  // H/W initialize
  Serial.begin(115200);
  pinMode(inPin, INPUT);
  pinMode(pingPin, OUTPUT);
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
    readHCSR04();
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
void readHCSR04()
{
  int i = 0;
  Serial.print("Task: read data from HCSR04, ");
  unsigned long duration;
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  duration = pulseIn(inPin, HIGH);
  Serial.print(calculateDistanceFromDuration(duration));
  Serial.print(" cm");
  Serial.print("\n\r");
}

unsigned long calculateDistanceFromDuration(unsigned long duration)
{
  // s = v x t = 340 m/s x duration microsecond x 10^-6 x 10^2 / 2 =  0.034 x t / 2 (cm)
  return duration * 0.034 / 2;
}