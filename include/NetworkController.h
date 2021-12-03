#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include <WiFi.h>
#include <WebServer.h>
#include "AutoConnect.h"

class NetworkController {

public:
    explicit NetworkController();

    void loop();

private:
    WebServer* _server;
    AutoConnect* _autoConnect;
    AutoConnectConfig* _config;

    void _handleOnConnect(IPAddress& ipaddr);
};

#endif