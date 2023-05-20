/*

   SmartUFCManager.h

   Autor: Michel Sales
   Maio de 2023


*/

#include "SmartUFCManager.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager 
#include "PubSubClient.h"


SmartUFCManager::SmartUFCManager(const String _device_type) 
{
    WiFiClient _mqttClient;
    this->mqttClient = _mqttClient;
    PubSubClient _mqtt(this->mqttClient);
    this->mqtt = _mqtt;
    this->device_type = _device_type;
    this->hasSubscription = false;
}

void SmartUFCManager::setup() 
{
    initSerial();
    initWiFi();
    initConfig();
    mqtt.setServer(SERVER_IP, mqtt_port);
}

void SmartUFCManager::beginLoop() 
{
    if (!mqtt.connected()) {
        reconnect();
    }
}

void SmartUFCManager::endLoop() 
{
    mqtt.loop();
}

void SmartUFCManager::initSerial() {
    Serial.begin(115200);
}

void SmartUFCManager::initWiFi()
{
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wm;
    wm.setConnectTimeout(180); // how long to try to connect for before continuing
    wm.setConfigPortalTimeout(180); // auto close configportal after n seconds

    while (!wm.autoConnect((device_type+ESP.getChipId()).c_str(),"password")) {
        Serial.println("Failed to connect and hit timeout. Trying again...");
        delay(3000);
    }

    Serial.println("Connected!");
}

void SmartUFCManager::initConfig()
{
    int code = httpRequest(PATH_CONFIG+ESP.getChipId());
    if(code!=200) {
        Serial.println("Device configuration failed, reseting device...");
        delay(3000);
        ESP.reset();
        delay(5000);
    } else {
        //if you get here you have connected to the WiFi    
        id_mqtt = device_id+"_device";
        topic_publish = "/"+api_key+"/"+device_id+"/attrs";
        topic_subscribe = "/"+api_key+"/"+device_id+"/cmd";
        Serial.print("id_mqtt:");
        Serial.println(id_mqtt);
        Serial.print("topic_publish:");
        Serial.println(topic_publish);
        Serial.print("topic_subscribe:");
        Serial.println(topic_subscribe);
        Serial.println("Device configured!");
    }
}

int SmartUFCManager::httpRequest(String path) 
{
  String address = SERVER_IP;
  String baseURL = "http://"+address+":"+SERVER_PORT+path;    
  Serial.print("Trying to get device config data from... ");
  Serial.println(baseURL);

  if(WiFi.status()== WL_CONNECTED){      
    http.begin(httpClient,(baseURL).c_str());
  
    // Send HTTP GET request
    int httpResponseCode = http.GET();
  
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      if (httpResponseCode==200) {        
        DynamicJsonDocument doc(1024);
        // Parse JSON object
        DeserializationError error = deserializeJson(doc, http.getString());
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return -9999;          
        } else {
          JsonObject obj = doc.as<JsonObject>();   
          api_key = obj["api_key"].as<const char*>(); 
          device_id = obj["device_id"].as<const char*>(); 
          mqtt_port = obj["mqtt_port"].as<int>();    
          Serial.print("api_key:");
          Serial.println(api_key);
          Serial.print("device_id:");
          Serial.println(device_id);
          Serial.print("mqtt_port:");
          Serial.println(mqtt_port);
        }        
      } else {        
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    return httpResponseCode;
  } else {
    Serial.println("WiFi Disconnected");
    return -9999;
  }  
}

void SmartUFCManager::reconnect() 
{
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT Server connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (mqtt.connect(id_mqtt.c_str())) {
      Serial.println("MQTT Server Connected!");
      if (this->hasSubscription) {
        Serial.print("Subscribing to");
        Serial.println(topic_subscribe.c_str());
        mqtt.subscribe(topic_subscribe.c_str());
      }
    } else {
      Serial.print("MQTT Server connection failed, rc=");
      Serial.print(mqtt.state());
      Serial.println("... Try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void SmartUFCManager::mqtt_publish(const char* payload){
    mqtt.publish(topic_publish.c_str(), payload);
}

void SmartUFCManager::setupSubscription(MQTT_CALLBACK_SIGNATURE){
    mqtt.setCallback(callback);
    this->hasSubscription = true;
}