#include "BaseDevice.h"

BaseDevice::BaseDevice(const char* ssid, const char* password, String serverIp){

  Serial.begin(9600);
  this->server = new ESP8266WebServer(80);
  this->ssid = ssid;
  this->password = password;
  this->serverIp = serverIp;
  server->on("/", std::bind(&BaseDevice::handleRoot, this));
  server->begin();
  Serial.println("HTTP server started");
  }

void BaseDevice::connectToWiFi(){
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  }

void BaseDevice::heartbeat(){
  if((millis() - pulseDelay) >= 10000){
    pulseDelay = millis();
   Serial.println("sending pulse");
  sendPulse();
  }
}

void BaseDevice::sendPulse(){
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    Serial.println("sending to: " +serverIp + "/api/heartbeat");
    http.begin(serverIp + "/api/heartbeat");
    http.addHeader("Content-Type", "application/json");
    const int capacity = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> doc;
    doc["mac"]=WiFi.macAddress();
    doc["ip"]=WiFi.localIP().toString();
    char output[128];
    serializeJson(doc, output);
    int httpCode = http.POST(output);
      if(httpCode > 0) {
        Serial.printf("HTTP code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          retryPulseCount = 0;
        }
      }
      else {
        Serial.printf("HTTP failed, error: %s\n", http.errorToString(httpCode).c_str());
        if(retryPulseCount < 10){
        Serial.printf("Retrying..");
        retryPulseCount++;
        sendPulse();
        }
      else{
        Serial.printf("Max retry count reached");
        retryPulseCount = 0;
        }
      }
    http.end();
  }
  else{
    connectToWiFi();
  }
}

void BaseDevice::sendValue(char* value){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverIp + "/api/sensors");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(value);
    if (httpCode > 0) {
      Serial.printf("HTTP code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        retryValueCount = 0;
      }
    }
    else {
      Serial.printf("HTTP failed, error: %s\n", http.errorToString(httpCode).c_str());
      if(retryValueCount < 10){
        Serial.printf("Retrying..");
        retryValueCount++;
        sendValue(value);
        }
      else{
        Serial.printf("Max retry count reached");
        retryValueCount = 0;
        }
    }
    http.end();
  }
  else {
    connectToWiFi();
  }
}

void BaseDevice::handleRoot(){
    server->send(200, "text/html", String("Up!"));
}

void BaseDevice::handleSensors(){
  for(Sensor* s: vec){
      s->execute();
    }
}

void BaseDevice::addSensor(Sensor* sensor){
  vec.push_back(sensor);
}
