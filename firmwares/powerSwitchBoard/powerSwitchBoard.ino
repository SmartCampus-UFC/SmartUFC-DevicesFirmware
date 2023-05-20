#include <SmartUFCManager.h>

// ############### CONSTANTS ################# //

//General
const int PERIODICITY=30000; // Defined milliseconds

//Device-specific
#define pinRELE 3 //PINO DIGITAL UTILIZADO PELO RELE
const String DEVICE_TYPE = "PSwitch";

// ############### DEFAULT VARIABLES ################# //

//Device-specific
String status = "OFF";
String statusMsg;

// ############### OBJECTS ################# //

//Manager
SmartUFCManager manager(DEVICE_TYPE);

//Device-specific

// ############# PROTOTYPES ############### //

//Device-specific
void deviceControl(); // publish device state
void processCommands(char* topic, byte* payload, unsigned int length); // receive and process commands

// ############## SETUP ################# //

void setup() {    
  
  manager.setup();
  manager.setupSubscription(processCommands);

  // ## JUST CHANGE IN THIS PART ## //
  
   
  
  // ############################## //
}

// ############### LOOP ################# //

void loop() { 

  manager.beginLoop();

  // ## JUST CHANGE IN THIS PART ## //
  
  deviceControl();

  // ############################## //
  
  manager.endLoop();
  delay(PERIODICITY);    
}

// ## DEFINE YOUR FUNCTIONS STARTING HERE ## //

void deviceControl() 
{
  statusMsg = "s|" + String(status); 
  Serial.println(statusMsg);
  manager.mqtt_publish(statusMsg.c_str());    
}

void processCommands(char* topic, byte* payload, unsigned int length) 
{
    String msg;

    //recover command from payload
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    Serial.println(msg);
    if(msg == manager.getDeviceId()+"@on|"){
      digitalWrite(pinRELE, HIGH);
      manager.mqtt_publish((manager.getDeviceId()+"@on| On ok").c_str());
      manager.mqtt_publish("s|ON");
      status = "ON";
    } else if(msg == manager.getDeviceId()+"@off|"){
      digitalWrite(pinRELE, LOW);
      manager.mqtt_publish((manager.getDeviceId()+"@off| Off ok").c_str());
      manager.mqtt_publish("s|OFF");
      status = "OFF";
    }
}
