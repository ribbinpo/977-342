#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#define LM73_ADDR 0x4D


#define LED_ON   LOW
#define LED_OFF  HIGH 


const char* ssid     = "POOM"; // Change to yours
const char* password = "0873883368"; // Change to yours

const char* mqtt_server = "192.168.1.10"; // Change to yours

/* create an instance of PubSubClient client */
WiFiClient espClient;
PubSubClient client(espClient);

/*LED GPIO pin*/
const char led1 = 2;
const char led2 = 12;
const char ldr = 36;

/* topics */
#define TEMP_TOPIC    "room/temp"
#define LED1_TOPIC     "room/led1" /* 1=on, 0=off */
#define LED2_TOPIC     "room/led2" /* 1=on, 0=off */
#define LDR_TOPIC     "room/ldr"

long lastMsg = 0;

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if(String(topic).equals("room/led1")){ 
    if ((char)payload[0] == '1') { 
      Serial.println("Your LED1 is on");
      digitalWrite(led1, LED_ON);  /* got '1' -> on state */
    }else{ 
      Serial.println("Your LED1 is off");
      digitalWrite(led1, LED_OFF);
    } /* we got '0' -> off state */
  }else if(String(topic).equals("room/led2")){
    if ((char)payload[0] == '1') { 
      Serial.println("Your LED2 is on");
      digitalWrite(led2, LED_ON);  /* got '1' -> on state */
    }else{ 
      Serial.println("Your LED2 is off");
      digitalWrite(led2, LED_OFF);
    } /* we got '0' -> off state */    
  }
}

void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(LED1_TOPIC);
      client.subscribe(LED2_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

float readTemp() {
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


void sendData(){
//  snprintf (msg, 20, "%d", cnt++);
  char msgLDR[20];
  char msgTEMP[20];
  snprintf(msgLDR,20,"%d",analogRead(ldr));
  Serial.printf("LDR: %d",analogRead(ldr));
  Serial.println(" Ohms");
  snprintf(msgTEMP,20,"%.2f",readTemp());
  Serial.printf("Temp: %.2f",readTemp());
  Serial.println(" C");
  /* publish the message */
  client.publish(LDR_TOPIC, msgLDR);
  client.publish(TEMP_TOPIC, msgTEMP); 
}

void setup() {
  Serial.begin(115200);
  Wire1.begin(4, 5);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  /* set led as output to control led on-off */
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(ldr, OUTPUT);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqtt_server, 1883); /* configure the MQTT server with IPaddress and port */
  client.setCallback(receivedCallback); /* client received subscribed topic */

}
void loop() {
  
  if (!client.connected()) { /* try to reconnect */
    mqttconnect();
  }
 
  client.loop(); /* listen for incomming subscribed receivedCallback */
  
  /* every 3 secs count increament for a published message */
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    sendData();
  }
}
