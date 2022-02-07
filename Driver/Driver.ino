#include "BaseDevice.h"
#include "LDR.h"

BaseDevice bd("Kiruba Kiruba", "Passwordillabro", "http://192.168.1.23:8080");
Sensor* l1 = new LDR(&bd, 1, A0);

void setup() {
  bd.connectToWiFi();
  bd.addSensor(l1);
  bd.getConfig();
}

void loop() {
  bd.heartbeat();
  bd.server->handleClient();
  bd.handleSensors();
}
