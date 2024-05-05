#include "RealTime.h"

RealTime::RealTime() : _state(Unavailable)
{
  this->_rtc = new RTC_DS3231();

  this->_init();
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

  Serial.println("[I] RTC online");
  _state = Available;

  _state = Available;
  DateTime now = this->_rtc->now();

  if (this->_rtc->lostPower() || now.year() < 2024)
  {
    Serial.println("[W] RTC lost power, we don't have a valid time");
    _state = NeedsUpdate;
    this->_setTime(0, false);
    return;
  }

  Serial.printf("[I] RTC has valid time: %d-%d-%d %d:%d:%d\n", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  this->_setTime(now.unixtime(), true);
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

void RealTime::setTimezone(String timezone)
{
  setenv("TZ", timezone.c_str(), 1);
  tzset();
}

void RealTime::_setTime(time_t time, bool isValid)
{
  timeval tv = {
      .tv_sec = time,
      .tv_usec = 0,
  };
  settimeofday(&tv, nullptr);

  if (isValid)
  {
    this->_rtc->adjust(DateTime(time));
    this->_state = Available;
  }
}

RealTime::RealTimeState RealTime::state()
{
  return _state;
}
