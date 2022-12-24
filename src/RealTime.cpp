#include "RealTime.h"

RealTime::RealTime()
{

    this->_rtc = new RTC_DS3231();
    this->_timezone = new Timezone();
    this->_timezone->setLocation("Europe/Berlin");

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
    this->_timezone->setTime(
        now.hour(),
        now.minute(),
        now.second(),
        now.day(),
        now.month(),
        now.year()
    );
}

void RealTime::loop() {
    this->_sync();
}

int RealTime::getHour() {
    return this->_timezone->hour();
}

int RealTime::getMinute() {
    return this->_timezone->minute();
}

long RealTime::getTime() {
    return this->_timezone->tzTime();
}

bool RealTime::hasValidTime() {
    return timeStatus() != timeStatus_t::timeNotSet;
}

bool RealTime::_shouldSync() {
    return timeStatus() == timeStatus_t::timeNeedsSync
        && NetworkController::connected();
}

void RealTime::_sync() {
    if(!this->_shouldSync()) return;

    Serial.println("Starting timesync");

    if(!waitForSync()){
        Serial.println("Timesync failed!");
    }
    else {
        this->_rtc->adjust(DateTime(
            this->_timezone->year(), 
            this->_timezone->month(),
            this->_timezone->dayOfYear(), 
            this->_timezone->hour(), 
            this->_timezone->minute(), 
            this->_timezone->second()
            ));
        Serial.println("Timesync successfull!");
        Serial.printf("The time is now: %d:%d\n", this->getHour(), this->getMinute());
    }
}