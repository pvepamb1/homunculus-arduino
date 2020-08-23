#include "BaseDevice.h"

void(* resetFunc) (void) = 0;

const char* HEARTBEAT_URL = "/api/heartbeat";
const char* SENSOR_URL = "/api/sensors";
const char* endpoints[2] = {HEARTBEAT_URL, SENSOR_URL};

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
    Serial.println("\n----------BEGIN HEARTBEAT----------\n");
    
    const int capacity = JSON_OBJECT_SIZE(2) + 37;
    StaticJsonDocument<capacity> doc;
    doc["mac"]=WiFi.macAddress();
    doc["ip"]=WiFi.localIP().toString();
    char output[128];
    serializeJson(doc, output);

    send(0, String(output));

    previousMillis = millis();

    Serial.println("\n----------END HEARTBEAT----------");
  }
}

void BaseDevice::send(int urlArrayIndex, String payload){ //couldn't make payload char* since it was going out of scope

  const char* endPoint = endpoints[urlArrayIndex];

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverIp + endPoint);
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(payload);

    Serial.println(payload);

    while(httpCode < 0 && retryCount < 3){
      Serial.printf("HTTP failed, error: %s\n", http.errorToString(httpCode).c_str());
      Serial.println("Retrying..");
      retryCount++;
      httpCode = http.POST(payload);
    }

    if(retryCount >= 3){
      Serial.println("Max retry count reached");
    }

    retryCount = 0;
    
    if (httpCode > 0) {
      Serial.printf("HTTP code: %d\n", httpCode);
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

  Serial.println("\n----------BEGIN CONFIG----------\n");

  do{
    if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      Serial.println("sending to: " + serverIp + "/api/sensors/config");
      http.begin(serverIp + "/api/sensors/config");
      http.addHeader("Content-Type", "application/json");

      Serial.printf("Config request: %s\n", output);
      Serial.println("Config response:");
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

  Serial.println("\n----------END CONFIG----------");
}

void BaseDevice::handleRoot(){
  Serial.print(server->arg("id"));
  for(Sensor* s: vec){
    if(s->id == server->arg("id").toInt()){
      server->send(200, "text/html", s->handleRoot());
      return;
    }
  }
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
