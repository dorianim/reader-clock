#include "RealTime.h"

RealTime::RealTime() : Task("RealTime") {

  this->_rtc = new RTC_DS3231();
  this->_timezone = new Timezone();
  this->_timezone->setPosix("CET-1CEST,M3.5.0,M10.5.0/3");
  ezt::setDebug(INFO);
  uint16_t ntpInterval = 60U * 60U * 2U;
  ezt::setInterval(ntpInterval);
  ezt::setNtpUpdateHandler(RealTime::_ntpUdateHandler, this);

  this->_init();
}

void RealTime::_init() {
  if (!this->_rtc->begin()) {
    Serial.println("[W] RTC offline");
    this->_rtcOnline = false;
    return;
  }

  Serial.println("[I] RTC online");
  this->_rtcOnline = true;

  if (this->_rtc->lostPower()) {
    Serial.println("[W] RTC lost power, we don't have a valid time");
    return;
  }

  Serial.println("[I] RTC has valid time");
  DateTime now = this->_rtc->now();
  this->_timezone->setTime(now.hour(), now.minute(), now.second(), now.day(),
                           now.month(), now.year());
}

void RealTime::setup() {}

void RealTime::loop() { ezt::events(); }

int RealTime::getHour() { return this->_timezone->hour(); }

int RealTime::getMinute() { return this->_timezone->minute(); }

long RealTime::getTime() { return this->_timezone->tzTime(); }

bool RealTime::hasValidTime() {
  return ezt::timeStatus() == timeStatus_t::timeSet;
}

void RealTime::_handleNtpUpdate() {
  this->_rtc->adjust(
      DateTime(this->_timezone->year(), this->_timezone->month(),
               this->_timezone->dayOfYear(), this->_timezone->hour(),
               this->_timezone->minute(), this->_timezone->second()));
  Serial.println("[I] Timesync successfull!");
  Serial.printf("[I] The time is now: %d:%d\n", this->getHour(),
                this->getMinute());
}

void RealTime::_ntpUdateHandler(void *args) {
  RealTime *that = (RealTime *)args;
  that->_handleNtpUpdate();
}