#include "NetworkController.h"

NetworkController::NetworkController() : Task("NetworkController") {
  this->_wifiManager.setTitle("ReaderClock");
  this->_id = "ReaderClock-" + this->_generateId();
  this->_wifiManager.setHostname(this->_id.c_str());
}

void NetworkController::run() {
  this->_wifiManager.autoConnect(this->_id.c_str());
  for (;;) {
    if (!this->_wifiManager.getWebPortalActive()) {
      this->_wifiManager.startWebPortal();
    } else {
      this->_wifiManager.process();
    }
  }
}

void NetworkController::_handleOnConnect(IPAddress &ipaddr) {
  Serial.printf("WiFi connected with %s, IP:%s\n", WiFi.SSID().c_str(),
                ipaddr.toString().c_str());
  if (WiFi.getMode() & WIFI_AP) {
    WiFi.softAPdisconnect(true);
    WiFi.enableAP(false);
  }
}

bool NetworkController::connected() { return WiFi.status() == WL_CONNECTED; }

String NetworkController::_generateId() {
  String id = WiFi.macAddress();
  id.replace(":", "");
  return id.substring(id.length() - 5);
}