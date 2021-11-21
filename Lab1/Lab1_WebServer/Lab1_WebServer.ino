#include <WiFi.h>
#include "time.h"

const char* ssid       = "POOM";
const char* password   = "0873883368";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 6*3600;
const int   daylightOffset_sec = 3600;

WiFiServer server(80);

const int LDR_pin = 36;
int val = 0;
struct tm timeinfo;
// Print LDR data in every 10 minutes
int valstore[6];
struct tm timeinfostore[6];
int n = 0;
void printLDRData()
{
  val = analogRead(LDR_pin);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S: ");
  // for every 10 minutes 
  if(timeinfo.tm_sec == 0){
    valstore[n] = val;
    timeinfostore[n] = timeinfo;
    Serial.print("value of LDR(Resistance): ");
    Serial.println(val);
    if(n==5)
      n=0;
    else
      n++;
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  printLDRData();
  delay(1000);
}
  
