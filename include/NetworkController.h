#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include <WiFi.h>
#include <WebServer.h>
#include "FirmwareVersion.h"

class NetworkController
{

public:
    explicit NetworkController();

    void loop();
    static bool connected();

private:
    WebServer *_server;

    void _handleOnConnect(IPAddress &ipaddr);
};

#endif