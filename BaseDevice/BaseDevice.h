#ifndef BaseDevice_H
#define BaseDevice_H

#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include<ArduinoJson.h>

class BaseDevice{
  
  private:
        const char* ssid;
        const char* password;
        String serverIp;
        unsigned long pulseDelay = 0;
        
  public:
        ESP8266WebServer* server;
        BaseDevice(const char* ssid, const char* password, String serverIp);
        void heartbeat();
        void connectToWiFi();
        String eepromRead();
        void eepromWrite();
        void sendPulse();
        void handleSensors();
        void addSensor();
};

#endif
