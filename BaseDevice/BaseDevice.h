#ifndef BaseDevice_H
#define BaseDevice_H

#include <Arduino.h>
#include <EEPROM.h>
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
        unsigned long pulseDelay = 0;
        std::vector<Sensor*> vec;
        
  public:
        ESP8266WebServer* server;
        BaseDevice(const char* ssid, const char* password, String serverIp);
        void heartbeat();
        void connectToWiFi();
        static String eepromRead();
        static void eepromWrite();
        void sendPulse();
        void handleSensors();
        void addSensor(Sensor* sensor);
};

#endif
