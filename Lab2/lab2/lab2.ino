#include <WiFi.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include "time.h"
#define LM73_ADDR 0x4D

const char* ssid       = "POOM";
const char* password   = "0873883368";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 6*3600;
const int   daylightOffset_sec = 3600;

int valueLDR = 0;
double temp=0;
int LDR_pin = 36;
struct tm timeinfo;
int n=0;

double tempStore[6];
int valueLDRStore[6];
String timeinfoStore[6];

// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
#define APDS9960_INT    18 // Needs to be an interrupt pin
int isr_flag = 0;

//
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
void printLocalTime()
{
  valueLDR = analogRead(LDR_pin);
  temp = readTemperature();
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  // for every 10 minutes && timeinfo.tm_min %10==0 
  if(timeinfo.tm_sec == 0){
    String timeStr = String(timeinfo.tm_hour)+":"+String(timeinfo.tm_min)+":"+String(timeinfo.tm_sec)+
    " "+String(timeinfo.tm_mday)+"/"+String(timeinfo.tm_mon)+"/"+String(1900+timeinfo.tm_year);
    Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S: ");
    Serial.print("Record data->Value of LDR: ");
    Serial.print(valueLDR);
    Serial.print(" ,Temperture: ");
    Serial.println(temp);
    tempStore[n] = temp;
    valueLDRStore[n] = valueLDR;
    timeinfoStore[n] = timeStr;
    if(n==5)
      n=0;
    else
      n++;
  }
}



void setup() {
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
//  WiFi.disconnect(true);
//  WiFi.mode(WIFI_OFF);
  
  // Initialize interrupt service routine
  pinMode(APDS9960_INT, INPUT);
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  
  Wire1.begin(4, 5);
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 gesture sensor engine
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
}

void loop() {
  printLocalTime();
  delay(1000);
  if( isr_flag == 1 ) {
    detachInterrupt(digitalPinToInterrupt(APDS9960_INT));
    handleGesture();
    isr_flag = 0;
    attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
  }
}


void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        //Time
        Serial.println("UP");
        Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
        break;
      case DIR_DOWN:
        Serial.println("DOWN");
        showRecord();
        break;
      case DIR_LEFT:
        //Temperature
        Serial.println("LEFT");
        Serial.print("Temp:");Serial.println(temp);
        break;
      case DIR_RIGHT:
        //LDR
        Serial.println("RIGHT");
        Serial.print("value of LDR(Resistance): ");
        Serial.println(valueLDR);
        break;
      case DIR_NEAR:
        Serial.println("NEAR, Try again!!");
        break;
      case DIR_FAR:
        Serial.println("FAR, Try again!!");
        break;
      default:
        Serial.println("NONE,Error, Try again!!!");
    }
  }
}

void showRecord(){
    if(n==0)
      Serial.println("Don't record yet!!!");
    else{
      for(int i=0;i<n;i++){
        Serial.print("[");Serial.print(i);Serial.print("]: ");
        Serial.print(&timeinfo, "%d %B %Y %H:%M:%S");Serial.print("| Temp:");
        Serial.print(tempStore[i]);Serial.print(" C| Resistor:");
        Serial.print(valueLDRStore[i]);Serial.println("Ohm");
      }   
    }
}
