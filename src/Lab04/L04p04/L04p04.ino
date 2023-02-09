#include <Arduino.h>
#include <DHT.h>
const int DHTPIN = 13;
#define PERIOD_TASK 2000
unsigned long tick_last;
byte rqtask;
DHT dht(DHTPIN, DHT22);
//-------------------------------------
void readDHT22();
void myschedule();
//=====================================
void setup()
{
  delay(3000);
  // H/W initialize
  Serial.begin(115200);
  dht.begin();
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
    readDHT22();
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
void readDHT22()
{
  int i = 0;
  float hum, temp;
  Serial.print("Task: read data from DHT22, ");
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(hum) || isnan(temp))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
}
