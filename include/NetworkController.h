#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include <WiFi.h>
#include <WebServer.h>
#include "FirmwareVersion.h"
#include "Task.h"

class NetworkController: Task
{

public:
    explicit NetworkController();

    static bool connected();

protected:
    void run() override;

private:
    WebServer *_server;

    void _handleOnConnect(IPAddress &ipaddr);
};

#endif