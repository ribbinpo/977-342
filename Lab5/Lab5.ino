#include <Arduino.h>
#include <HTTPClient.h>
#include <Wire.h>
#define LM73_ADDR 0x4D

int LDR_pin = 36;

const char* ssid     = "POOM";
const char* password = "0873883368";

float readTemperature() {
  Wire1.beginTransmission(LM73_ADDR);
  Wire1.write(0x00); // Temperature Data Register
  Wire1.endTransmission();
  uint8_t count = Wire1.requestFrom(LM73_ADDR, 2);
  float temp = 0.0;
  if (count == 2) {
    byte buff[2];
    buff[0] = Wire1.read();
    buff[1] = Wire1.read();
    temp += (int)(buff[0]<<1);
    if (buff[1]&0b10000000) temp += 1.0;
    if (buff[1]&0b01000000) temp += 0.5;
    if (buff[1]&0b00100000) temp += 0.25;
    if (buff[0]&0b10000000) temp *= -1.0;
  }
  return temp;
}


// Accessed Google Script Settings
//const char* APP_SERVER = "script.google.com";
const char* KEY = "AKfycbylgXYJGTOPk4rv2kI3ZUZ0dy5Adsyroz7PoZwkriLI4K5c2mnnoO6INsgCjhDO5416BQ"; // GAS ID
//https://script.google.com/macros/s/AKfycbylgXYJGTOPk4rv2kI3ZUZ0dy5Adsyroz7PoZwkriLI4K5c2mnnoO6INsgCjhDO5416BQ/exec

void UpdatSheets(int ldr, int temp) {
  HTTPClient http;
  String URL = "https://script.google.com/macros/s/";

  URL += KEY;
  URL += "/exec?";
  URL += "ldr=";
  URL += ldr;
  URL += "&temp=";
  URL += temp;
  Serial.println(URL);
  // access to your Google Sheets
  // configure target server and url
  http.begin(URL.c_str());
  Serial.println("[HTTP] GET...");
  // start connection and send HTTP header
  int httpCode = http.GET();

  if (httpCode == 200) {
    String content = http.getString();
    Serial.println("Content ---------");
    Serial.println(content);
    Serial.println("-----------------");
  } else {
    Serial.println("Fail. error code " + String(httpCode));
  }
  Serial.println("END");
}

void setup() {
  Serial.begin(115200);
  Wire1.begin(4, 5);
  pinMode(LDR_pin,OUTPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
}

void loop() {
  float temp = readTemperature();
  int ldr = analogRead(LDR_pin);
  UpdatSheets(ldr, temp);
  delay(10000); // set the interval for a half second
}
