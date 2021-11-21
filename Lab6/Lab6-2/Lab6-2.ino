#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <analogWrite.h>

// Pins
#define APDS9960_INT    18 // Needs to be an interrupt pin
// Constants
// Global Variables
SparkFun_APDS9960 apds = SparkFun_APDS9960();
int isr_flag = 0;

void interruptRoutine() {
  isr_flag = 1;
}

char handleGesture() {
    if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        Serial.println("U"); return 'U';
        break;
      case DIR_DOWN:
        Serial.println("D"); return 'D';
        break;
      case DIR_LEFT:
        Serial.println("L"); return 'L';
        break;
      case DIR_RIGHT:
        Serial.println("R"); return 'R';
        break;
      case DIR_NEAR:
        Serial.println("N"); return 'N';
        break;
      case DIR_FAR:
        Serial.println("F"); return 'F';
        break;
      default:
        Serial.println("n"); return 'n';
    }
  }
}


QueueHandle_t  q = NULL;

int one_min = 1000/portTICK_PERIOD_MS;
int half_min = 500/portTICK_PERIOD_MS;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Set interrupt pin as input
  pinMode(APDS9960_INT, INPUT);
  // Initialize Serial port
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("--------------------------------"));
  Serial.println(F("SparkFun APDS-9960 - GestureTest"));
  Serial.println(F("--------------------------------"));
  
  // Initialize interrupt service routine
  attachInterrupt(digitalPinToInterrupt(APDS9960_INT ), interruptRoutine, FALLING);

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
  
  q=xQueueCreate(20,sizeof(unsigned long));
    if(q != NULL){
        Serial.printf("Queue is created\n");
        xTaskCreate(&producer_task,"producer_task",2048,NULL,5,NULL);
        Serial.printf("producer task  started\n");
        xTaskCreate(&consumer_task,"consumer_task",2048,NULL,5,NULL);
        Serial.printf("consumer task  started\n");
    }else{
        Serial.printf("Queue creation failed");
    }    

}

void loop() {}

void producer_task(void *pvParameter){
    char _direction = ' ';
    if(q == NULL){
        Serial.printf("Queue is not ready \n");
        return;
    }
    while(1){
      _direction = ' ';
      if( isr_flag == 1 ) {
        detachInterrupt(0);
        _direction = handleGesture();
        isr_flag = 0;
        attachInterrupt(digitalPinToInterrupt(APDS9960_INT), interruptRoutine, FALLING);
      }
      if(_direction != ' '){
        Serial.printf("value sent on queue: %c\n",_direction);
        xQueueSend(q,(void *)&_direction,(TickType_t )(1000/portTICK_PERIOD_MS));
        vTaskDelay(one_min); //wait for a second
      }    
    }
}

void consumer_task(void *pvParameter)
{
  typedef struct {
  char name[16];  // task name
  int  led_pin;   // LED pin number
  bool  turn;     // LED Turn on-off
  int bright;     // LED brightness 0-100 %
  } LEDParams;
  LEDParams ledParams[4] = {
   {"LED1", 2,false,100},
   {"LED2", 12,false,100},
   {"LED3", 2,false,100},
   {"LED4", 12,false,100}
  };
  char _direction = ' ';
    if(q == NULL){
        Serial.printf("Queue is not ready");
        return;
    }
    int led_target = 0;
    pinMode(2,OUTPUT);
    pinMode(12,OUTPUT);
    int brightness_value=(led_target/100.0)*255.0;
    while(1){
        xQueueReceive(q,&_direction,(TickType_t )(5000/portTICK_PERIOD_MS)); 
        //U->ON-OFF L->Decrease brightness R->Increase brightness D->Change LED
        Serial.printf("value received on queue: %c \n",_direction);
        if(_direction != ' '){
          switch(_direction){
            case 'U':
              if(ledParams[led_target].turn==true){
                Serial.print("Turn off: ");
                ledParams[led_target].turn = false;
              }
              else{ 
                Serial.print("Turn on: ");
                ledParams[led_target].turn = true;
              }
              Serial.printf("%s\n",ledParams[led_target].name);
              break;
            case 'L':
              Serial.printf("%s",ledParams[led_target].name);
              Serial.print(": ");
              if(ledParams[led_target].bright-10>=0){
                ledParams[led_target].bright = ledParams[led_target].bright-10;
                Serial.print("Decrease brightness to: ");
              }else{
                Serial.print("Can't decrease less than 0%: ");  
              }
              Serial.println(ledParams[led_target].bright);
              break;
            case 'R':
              Serial.printf("%s",ledParams[led_target].name);
              Serial.print(": ");
              if(ledParams[led_target].bright+10<=100){
                ledParams[led_target].bright = ledParams[led_target].bright+10;
                Serial.print("Increase brightness to: ");
              }else{
                Serial.print("Can't increase less than 0%: ");  
              }
              Serial.println(ledParams[led_target].bright);
              break;
            case 'D':
              led_target++;
              if(led_target==4) led_target = 0;
              Serial.print("Change target led to ");
              Serial.println(led_target);
              break;
          }
          vTaskDelay(half_min); //wait for 500 ms
        }else{
          Serial.println("Don't recieve anything yet");  
        }
        for(int i=0;i<4;i++){
            digitalWrite(ledParams[i].led_pin,ledParams[i].turn);
            if(ledParams[i].turn==true){
              brightness_value=(led_target/100.0)*255.0;
              analogWrite(ledParams[i].led_pin,ledParams[i].bright);
            }
        }
    }
}
