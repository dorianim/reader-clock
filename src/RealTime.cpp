#include "RealTime.h"

RealTime::RealTime() : 
    _lastSync(-1),
    _timezoneOffset(1) 
{

    this->_rtc = new RTC_DS3231();
    this->_timezone = new Timezone();
    this->_timezone->setLocation("Europe/Berlin");

    if (!this->_rtc->begin()) {
        Serial.println("Couldn't find RTC");
    }

    if (this->_rtc->lostPower()) {
        Serial.println("RTC lost power, we don't have a valid time!");
        this->_hasValidTime = false;
    }
    else {
        this->_hasValidTime = true;
    }
}

void RealTime::loop() {
    this->_sync();
}

int RealTime::getHour() {
    DateTime now = this->_rtc->now();
    return now.hour();
}

int RealTime::getMinute() {
    DateTime now = this->_rtc->now();
    return now.minute();
}

long RealTime::getTime() {
    DateTime now = this->_rtc->now();
    return now.unixtime();
}

bool RealTime::hasValidTime() {
    return this->_hasValidTime;
}

bool RealTime::_shouldSync() {
    return (
            !this->hasValidTime() || 
            this->_lastSync < 0 || 
            this->getTime() - this->_lastSync > 60 * 60
        ) 
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
        this->_hasValidTime = true;
        this->_lastSync = this->getTime();
        Serial.println("Timesync successfull!");
        Serial.printf("The time is now: %d:%d\n", this->getHour(), this->getMinute());
    }
}