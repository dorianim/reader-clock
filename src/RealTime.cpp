#include "RealTime.h"

#include <FS.h>
#include <LittleFS.h>

RealTime::RealTime() : _state(Unavailable)
{
  this->_rtc = new RTC_DS3231();

  this->_init();
}

// --- fs helpers ---
String _readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);
  if (!fs.exists(path))
  {
    Serial.println("- file does not exist");
    return String();
  }
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

bool _writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return false;
  }

  bool ret = file.print(message);
  file.close();
  return ret;
}

void RealTime::_init()
{
  if (!this->_rtc->begin())
  {
    Serial.println("[ERROR] RTC offline");
    _state = Unavailable;
    this->_setTime(0, false);
    return;
  }

  if (!LittleFS.begin(true))
  {
    Serial.println("[ERROR] LittleFS offline");
    _state = Unavailable;
    this->_setTime(0, false);
    return;
  }

  this->_setTimezone(_readFile(LittleFS, "/timezone").c_str(), false);

  Serial.println("[I] RTC online");

  DateTime now = this->_rtc->now();

  if (this->_rtc->lostPower() || now.year() < 2024)
  {
    Serial.println("[W] RTC lost power, we don't have a valid time");
    _state = NeedsUpdate;
    this->_setTime(0, false);
    return;
  }

  Serial.printf("[I] RTC has valid time: %d-%d-%d %d:%d:%d\n", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  this->_setTime(now.unixtime(), false);
  _state = Available;
}

int RealTime::getHour()
{
  return getTime()->tm_hour;
}

int RealTime::getMinute() { return getTime()->tm_min; }

int RealTime::getSecond() { return getTime()->tm_sec; }

tm *RealTime::getTime()
{
  timeval tv;
  gettimeofday(&tv, nullptr);
  return localtime(&tv.tv_sec);
}

void RealTime::setTime(time_t time)
{
  this->_setTime(time, true);
}

bool RealTime::setTimezone(String timezone)
{
  return _setTimezone(timezone.c_str(), true);
}

bool RealTime::_setTimezone(String timezone, bool store)
{
  setenv("TZ", timezone.c_str(), 1);
  tzset();

  if (!store)
    return true;

  return _writeFile(LittleFS, "/timezone", timezone.c_str());
}

void RealTime::_setTime(time_t time, bool store)
{
  timeval tv = {
      .tv_sec = time,
      .tv_usec = 0,
  };
  settimeofday(&tv, nullptr);

  if (store)
  {
    this->_rtc->adjust(DateTime(time));
    this->_state = Available;
  }
}

RealTime::RealTimeState RealTime::state()
{
  return _state;
}
