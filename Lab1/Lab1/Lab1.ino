#include <WiFi.h>
#include "time.h"

const int LDR_pin = 36;
int val = 0;

const char* ssid       = "POOM";
const char* password   = "0873883368";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 6*3600;
const int   daylightOffset_sec = 3600;

// Print LDR data in every 10 minutes
void printLDRData()
{
  struct tm timeinfo;
  val = analogRead(LDR_pin);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S: ");
  if(timeinfo.tm_sec == 0 && timeinfo.tm_min%10==0){
    Serial.print("value of LDR(Resistance): ");
    Serial.println(val); 
  }else{
    Serial.println();  
  }
}

void setup()
{
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void loop()
{
  delay(1000);
  printLDRData();
}
