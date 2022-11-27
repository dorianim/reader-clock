#include "NetworkController.h"

NetworkController::NetworkController()
{
    this->_server = new WebServer();

    this->_server->on("/", [this]()
                      { this->_server->send(200, "text/json", "{ \"message\": \"Hello, World!\" }"); });
}

void NetworkController::loop()
{
    this->_server->handleClient();
}

void NetworkController::_handleOnConnect(IPAddress &ipaddr)
{
    Serial.printf("WiiFi connected with %s, IP:%s\n", WiFi.SSID().c_str(), ipaddr.toString().c_str());
    if (WiFi.getMode() & WIFI_AP)
    {
        WiFi.softAPdisconnect(true);
        WiFi.enableAP(false);
    }
}

bool NetworkController::connected()
{
    return WiFi.status() == WL_CONNECTED;
}