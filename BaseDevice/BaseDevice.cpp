#include "BaseDevice.h"

BaseDevice::BaseDevice(const char* ssid, const char* password, String serverIp){

  Serial.begin(9600);
  this->server = new ESP8266WebServer(80);
  this->ssid = ssid;
  this->password = password;
  this->serverIp = serverIp;
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
    if(eepromRead() != WiFi.localIP().toString()){
      eepromWrite();
    }
   Serial.println("sending pulse");
  sendPulse();
  }
}

String BaseDevice::eepromRead(){
  String ip;
  int len = EEPROM.read(0);
  for(int i = 1; i<=len; i++){
    char c = EEPROM.read(i);
    ip += String(c);
  }
  Serial.println("Stored IP: " +ip);
  return ip;
}

void BaseDevice::eepromWrite(){
  String ip = WiFi.localIP().toString();
  EEPROM.write(0, ip.length());
  for(int i=0; i<ip.length(); i++){
    EEPROM.write(i+1, ip[i]);
  }
  EEPROM.commit();
  Serial.println("updated IP: " +ip);
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
        }
      }
      else {
        Serial.printf("HTTP failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
    http.end();
  }
  else{
    connectToWiFi();
  }
}

void BaseDevice::handleSensors(){
  //loop through every sensor's execute method
}

void BaseDevice::addSensor(){
  //register a sensor
}
