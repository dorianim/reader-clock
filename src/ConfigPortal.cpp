#include "ConfigPortal.h"

#include <WiFi.h>

#define EMBEDDED_FILE(file)                                                       \
  extern const uint8_t file##_start[] asm("_binary_web_dist_" #file "_gz_start"); \
  extern const uint8_t file##_end[] asm("_binary_web_dist_" #file "_gz_end");
#define EMBEDDED_FILE_HANDLER(path, type, file) this->_webServer->on(path, HTTP_GET, [this]() { \
    this->_webServer->sendHeader("Content-Encoding", "gzip"); \
    this->_webServer->send_P(200, type, (const char *)file##_start, (size_t)file##_end - (size_t)file##_start); });

EMBEDDED_FILE(index_html)
EMBEDDED_FILE(pico_css)
EMBEDDED_FILE(moment_min_js)
EMBEDDED_FILE(moment_timezone_min_js)
EMBEDDED_FILE(posix_timezone_min_js)

ConfigPortal::ConfigPortal(RealTime *time) : Task("ConfigPortal"), _webServer(nullptr), _dnsServer(nullptr), _running(false), _id(""), _time(time)
{
  this->_id = "ReaderClock-" + this->_generateId();

  this->_webServer = new WebServer(80);
  this->_dnsServer = new DNSServer();

  this->_constructorDone = true;
}

void ConfigPortal::setup()
{
}

void ConfigPortal::loop()
{
  if (!_running)
  {
    vTaskDelay(100 / portTICK_PERIOD_MS);
    return;
  }

  this->_webServer->handleClient();
  this->_dnsServer->processNextRequest();
  vTaskDelay(10 / portTICK_PERIOD_MS);
}

String ConfigPortal::networkName() { return this->_id; }

void ConfigPortal::_redirectToRoot()
{
  this->_webServer->sendHeader("Location", "http://192.168.4.1/");
  this->_webServer->send(302, "text/plain", "");
}

void ConfigPortal::_handleIndex()
{
}

void ConfigPortal::start()
{

  WiFi.mode(WIFI_AP);
  WiFi.softAP(this->_id.c_str());

  EMBEDDED_FILE_HANDLER("/", "text/html", index_html);
  EMBEDDED_FILE_HANDLER("/pico.css", "text/css", pico_css);
  EMBEDDED_FILE_HANDLER("/moment.min.js", "application/javascript", moment_min_js);
  EMBEDDED_FILE_HANDLER("/moment-timezone.min.js", "application/javascript", moment_timezone_min_js);
  EMBEDDED_FILE_HANDLER("/posix-timezone.min.js", "application/javascript", posix_timezone_min_js);

  this->_webServer->on("/favicon.ico", HTTP_GET, [this]()
                       { this->_webServer->send(404); });

  this->_webServer->on("/form/datetime", HTTP_POST, [this]
                       { 
                        Serial.println("Received POST request");

                          Serial.println("Timestamp: " + this->_webServer->arg("timestamp"));
                          Serial.println("Timezone: " + this->_webServer->arg("timezone"));

                          this->_time->setTimezone(this->_webServer->arg("timezone"));
                          // TODO: is this safe?
                          unsigned long timestamp = strtol(this->_webServer->arg("timestamp").c_str(), NULL, 10);
                          this->_time->setTime(timestamp);
                          
                          this->_redirectToRoot(); });

  this->_webServer->onNotFound([this]()
                               { this->_redirectToRoot(); });

  this->_webServer->begin();
  this->_dnsServer->start(53, "*", WiFi.softAPIP());
  _running = true;
}

void ConfigPortal::stop()
{
  WiFi.mode(WIFI_OFF);
  if (this->_webServer)
  {
    this->_webServer->stop();
  }

  if (this->_dnsServer)
  {
    this->_dnsServer->stop();
  }

  _running = false;
}

bool ConfigPortal::running() { return this->_running; }

String ConfigPortal::_generateId()
{
  String id = WiFi.macAddress();
  id.replace(":", "");
  return id.substring(id.length() - 5);
}