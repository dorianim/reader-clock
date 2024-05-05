#ifndef NETWORK_CONTROLLER_H
#define NETWORK_CONTROLLER_H

#include "FirmwareVersion.h"
#include "Task.h"
#include "RealTime.h"

#include <DNSServer.h>
#include <WebServer.h>

class ConfigPortal : Task
{

public:
  explicit ConfigPortal(RealTime *time);

  bool running();
  String networkName();

  void start();
  void stop();

protected:
  void setup() override;
  void loop() override;

private:
  RealTime *_time;
  WebServer *_webServer;
  DNSServer *_dnsServer;
  bool _running;
  String _id;
  String _generateId();

  void _redirectToRoot();
  void _handleIndex();
};

#endif