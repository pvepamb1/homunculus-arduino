#include "BaseDevice.h"

void(* resetFunc) (void) = 0;

BaseDevice::BaseDevice(const char* ssid, const char* password, String serverIp){

  Serial.begin(9600);
  this->server = new ESP8266WebServer(80);
  this->ssid = ssid;
  this->password = password;
  this->serverIp = serverIp;
  server->on("/", std::bind(&BaseDevice::handleRoot, this));
  server->on("/reset", std::bind(&BaseDevice::handleReset, this));
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
  if((millis() - previousMillis) >= heartbeatDelay){
    previousMillis = millis();
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
    const int capacity = JSON_OBJECT_SIZE(2) + 37;
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

void BaseDevice::getConfig(){

  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(3) + 37;
  DynamicJsonDocument doc(capacity);

  doc["mac"] = WiFi.macAddress();
  doc["ip"] = WiFi.localIP().toString();
  JsonArray ids = doc.createNestedArray("ids");
  
  for(Sensor* s: vec){
    ids.add(s->id);
  }

  char output[128];
  serializeJson(doc, output);
  int httpCode = 0;

  do{
    if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      Serial.println("sending to: " +serverIp + "/api/sensors/config");
      http.begin(serverIp + "/api/sensors/config");
      http.addHeader("Content-Type", "application/json");

      Serial.print(output);

      httpCode = http.POST(output);
      if(httpCode > 0) {
        Serial.printf("HTTP code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          const size_t capacity2 = JSON_ARRAY_SIZE(2) + 2*JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(5) + 140; //needs to be abstracted
          DynamicJsonDocument doc2(capacity2);
          deserializeJson(doc2, payload);
          JsonArray arr = doc2.as<JsonArray>();
          for(Sensor* s: vec){
            for (JsonObject value : arr) {
              if(s->id == atoi(value["id"]["id"]))
                s->setConfig(value);
            }
          }
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
    if(httpCode != HTTP_CODE_OK){
          delay(10000);
      }
  }while (httpCode != HTTP_CODE_OK);
}

void BaseDevice::handleRoot(){
    server->send(200, "text/html", String("Up!"));
}

void BaseDevice::handleReset(){
    server->send(200, "text/html", String("Resetting..."));
    delay(2000);
    resetFunc();
}

void BaseDevice::handleSensors(){
  for(Sensor* s: vec){
      s->execute();
    }
}

void BaseDevice::addSensor(Sensor* sensor){
  vec.push_back(sensor);
}
