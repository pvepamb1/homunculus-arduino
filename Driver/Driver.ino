#include "BaseDevice.h"
#include "LDR.h"

BaseDevice bd("SSID", "Password", "ServerUrl:Port");
Sensor* l1 = new LDR(&bd, 1, A0, 10000);

void setup() {
  bd.connectToWiFi();
  bd.addSensor(l1);
}

void loop() {
  bd.heartbeat();
  bd.server->handleClient();
  bd.handleSensors();
}
