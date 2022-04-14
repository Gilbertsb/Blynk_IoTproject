
#include <SPI.h>
//#include <Ethernet.h>
//#include <BlynkSimpleEthernet.h>
#include <DHT.h>
#include <FS.h>  // Include the SPIFFS library


// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "******"
#define BLYNK_DEVICE_NAME "******"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

#define USE_NODE_MCU_BOARD

#include "BlynkEdgent.h"

#define DHTPIN 14  //  (2 D4) (14 D5) What digital pin we're connected to

//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22       // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21     // DHT 21, AM2301


DHT dht(DHTPIN, DHTTYPE);

int uptimeCounter;

int SetTemp;     // variable to hold Heat Index Input value
int Hyst;        // variable to hold Hysteresis Input value
int TimeInterval;    // test time interval

const int relay1 = 12;        //GPIO D6 
boolean flagRelay1 = false;   // Keep track of whether the relay is on (true) / off (false)



// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V3, V4, V5);  //get data stored in virtual pin V3 & V4 from server
}

BLYNK_WRITE(V3)
{
  SetTemp = param.asInt();
  
 // File Data = SPIFFS.open("/SetTemp.text", "w");  //Create file and write
 // Data.print(SetTemp);                            //Value from Numeric Input Widget
 // Data.close();                     
 // Data = SPIFFS.open("/SetTemp.text", "r");    //Open file and read
 // Serial.print("SetTemp: ");
 // Serial.println(Data.read());
 // Data.close();
  
  uptimeCounter = param.asInt(); 
}
BLYNK_WRITE(V4)
{
  Hyst = param.asInt();
  uptimeCounter = param.asInt(); 
}
BLYNK_WRITE(V5)
{
  TimeInterval = param.asInt();
  uptimeCounter = param.asInt(); 
}



void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();       // or dht.readTemperature(true) for Fahrenheit
  float hi = dht.computeHeatIndex(t, h); // (t, h, true); for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.  
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, hi);

  if (t >= SetTemp+Hyst && flagRelay1 == false)
  {
    digitalWrite(relay1, 1);    // Relay1 on
    Blynk.virtualWrite(V6, 1);  // Blynk V6 on
    Blynk.notify(String("Temperature is Higher than") + SetTemp + Hyst);
    flagRelay1 = true;          // flag set Off
    
  }
  else if (t < SetTemp-Hyst && flagRelay1 == true)
  {
    digitalWrite(relay1, 0);    // Relay1 Off
    Blynk.virtualWrite(V6, 0);  // Blynk V6 off
    Blynk.notify(String("Temperature is Lower than") + SetTemp);
    flagRelay1 = false;         // flag set Off
  }

} 

void TestCondition()
{
}
  
void setup()
{
  Serial.begin(115200);
  delay(100);

  BlynkEdgent.begin();

  dht.begin();

  SPIFFS.begin();

  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, 0);
  
  timer.setInterval(4000L, sendSensor);  // Setup a function to be called every second (1000L)
}

void loop() {
  BlynkEdgent.run();
  timer.run();
}
