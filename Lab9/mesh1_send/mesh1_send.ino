#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;

// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

//Press 0 
const char button = 16;
int valButton = 1;
const char led1 = 2;
const char led2 = 12;

void sendMessage() {
  int checkButton = digitalRead(button);
  if(checkButton != valButton){
    String msg = "";
    valButton = checkButton;
    msg += checkButton;
    msg += "from node: ";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
    Serial.println("Send message complete: "+msg);
  }
//  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  taskSendMessage.setInterval(TASK_SECOND * 1);
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  String msgData = msg.c_str();
  int msgLED = msgData.substring(0,1).toInt();
  String Data = msgData.substring(1);
  Serial.println(msgLED);
  Serial.println(Data);
  if(msgLED == 1){
   digitalWrite(led1,HIGH); 
   digitalWrite(led2,HIGH); 
  }else{
   digitalWrite(led1,LOW); 
   digitalWrite(led2,LOW); 
  }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u", nodeId);
    Serial.println();
}

void changedConnectionCallback() {
  Serial.printf("Changed connections");
  Serial.println();
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d", mesh.getNodeTime(),offset);
    Serial.println();
}

void setup() {
  Serial.begin(115200);
  pinMode(button,INPUT_PULLUP);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
