#include "NetworkController.h"

NetworkController::NetworkController() {
    // Timeout immidiately. The code will work without network connection
    this->_config = new AutoConnectConfig();
    this->_config->portalTimeout = 1;
    this->_config->retainPortal = true;
    this->_config->apid = "Reader Clock";
    this->_config->psk = "R3aderC7ock";
    this->_config->title = "Reader Clock";
    this->_config->autoReconnect = true;

    this->_server = new WebServer();
    this->_autoConnect = new AutoConnect(*this->_server);
    this->_autoConnect->config(*this->_config);
    this->_autoConnect->begin();
    this->_autoConnect->onConnect([this](IPAddress& ipaddr){
        this->_handleOnConnect(ipaddr);
    });
}

void NetworkController::loop() {
  this->_autoConnect->handleClient();
}

void NetworkController::_handleOnConnect(IPAddress& ipaddr) {
    Serial.printf("WiiFi connected with %s, IP:%s\n", WiFi.SSID().c_str(), ipaddr.toString().c_str());
    if (WiFi.getMode() & WIFI_AP) {
        WiFi.softAPdisconnect(true);
        WiFi.enableAP(false);
    }
}