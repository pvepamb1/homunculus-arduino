#ifndef BaseDevice_H
#define BaseDevice_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include<ArduinoJson.h>
#include "Sensor.h"
#include <vector>

class BaseDevice{
  
  private:
        const char* ssid;
        const char* password;
        String serverIp;
        unsigned long previousMillis = 0;
        unsigned long heartbeatDelay = 10000;
        std::vector<Sensor*> vec;
        int retryPulseCount = 0;
        int retryValueCount = 0;
        
  public:
        ESP8266WebServer* server;
        BaseDevice(const char* ssid, const char* password, String serverIp);
        void heartbeat();
        void connectToWiFi();
        void sendPulse();
        void sendValue(char* value);
        void handleRoot();
        void handleReset();
        void handleSensors();
        void addSensor(Sensor* sensor);
        void getConfig();
};

#endif
