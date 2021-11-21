// Import required libraries
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "time.h"

#define ldrPin 36

// Replace with your network credentials
const char* ssid = "POOM";
const char* password = "0873883368";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 6*3600;
const int   daylightOffset_sec = 3600;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

//Set up variable
int n =0;
int valLDR = 0;
int valstore[6];
struct tm timeinfo;
String timeStrStore[6];
String timeStr;

void printLocalTime()
{
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  valLDR = analogRead(ldrPin);
  timeStr = String(timeinfo.tm_hour)+":"+String(timeinfo.tm_min)+":"+String(timeinfo.tm_sec)+
  " "+String(timeinfo.tm_mday)+"/"+String(timeinfo.tm_mon)+"/"+String(1900+timeinfo.tm_year);
  //timeinfo.tm_min%10==0 for every 10 minutes 
  if(timeinfo.tm_sec == 0){
    valstore[n] = valLDR;
    timeStrStore[n] = timeStr;
    Serial.print("value of LDR(Resistance): ");
    Serial.println(valLDR);
    if(n==5)
      n=0;
    else
      n++;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
</head>
<body>
  <h2>ESP32 LDR Server</h2>
  <p>
    <i class="far fa-clock"></i>
    <span>Time</span> 
    <span id="Time">%Time%</span>
  </p>
  <p>
    <i class="fas fa-sun"></i>
    <span>LDR: </span> 
    <span id="LDR">%LDR%</span>
    <span> Ohm</span>
  </p>
</body>
<script>
//Set LDR value in web
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200)
      document.getElementById("LDR").innerHTML = this.responseText;
  };
  xhttp.open("GET", "/LDR", true);
  xhttp.send();
}, 1000 ) ;
//Set Time value in web
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200)
      document.getElementById("Time").innerHTML = this.responseText;
  };
  xhttp.open("GET", "/Time", true);
  xhttp.send();
}, 1000 ) ;
  
</script>
</html>)rawliteral";

// Replaces placeholder
// Running in GET /
String processor(const String& var){
  if(var == "LDR"){
    return String(valLDR);
  }
  else if(var == "Time"){
    return timeStr;  
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/LDR", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(valLDR).c_str());
  });
  server.on("/Time", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", timeStr.c_str());
  });  
  // Start server
  server.begin();
}
 
void loop(){
  printLocalTime();
  delay(1000);
}
