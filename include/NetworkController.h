#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include "FirmwareVersion.h"
#include "Task.h"
#include <WiFiManager.h>

class NetworkController : Task {

public:
  explicit NetworkController();

  static bool connected();

protected:
  void run() override;

private:
  WiFiManager _wifiManager;
  String _id;

  void _handleOnConnect(IPAddress &ipaddr);
  String _generateId();
};

#endif