/*

   SmartUFCManager.h

   Autor: Michel Sales
   Maio de 2023


*/

#ifndef SmartUFCManager_h
#define SmartUFCManager_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager 
#include "PubSubClient.h"

#define SERVER_IP "200.19.187.67" // Defined. Both MQTT and config servers
#define SERVER_PORT "50002" // Defined
#define PATH_CONFIG "/device?id=" // Fixed

#ifdef ESP8266
#include <functional>
#define MQTT_CALLBACK_SIGNATURE std::function<void(char*, uint8_t*, unsigned int)> callback
#else
#define MQTT_CALLBACK_SIGNATURE void (*callback)(char*, uint8_t*, unsigned int)
#endif

class SmartUFCManager
{
   public:
        SmartUFCManager(const String device_type);
        void setup();
        void beginLoop();
        void endLoop();
        void initSerial();
        void initWiFi();
        void initConfig();
        int httpRequest(String path);
        void reconnect();
        void mqtt_publish(const char* payload);
        void setupSubscription(MQTT_CALLBACK_SIGNATURE);
        // Getters
        String getDeviceId() {
            return device_id;
        }
   private:
        String api_key; //recovered from web server
        String device_id; //recovered from web server
        int mqtt_port; //recovered from web server
        String id_mqtt; // defined
        String topic_publish; // defined
        String topic_subscribe; // defined
        String device_type; // defined
        bool hasSubscription; // indicates if device must subscribe to topic_subscribe
        WiFiClient mqttClient;
        PubSubClient mqtt;
        WiFiClient httpClient;
        HTTPClient http;  
};

#endif
