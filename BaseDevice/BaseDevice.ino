#include "BaseDevice.h"
#include "LDR.h"

BaseDevice bd("PORTGAS D. ACE", "monkeydluffy232", "http://192.168.1.12:8080");
LDR l1(A0, 10000);

void setup() {
  bd.connectToWiFi();
  bd.addSensor();
}

void loop() {
  bd.heartbeat();
  bd.server->handleClient();
  bd.handleSensors();
}
