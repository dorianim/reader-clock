#include "RealTime.h"

RealTime::RealTime() {
    this->_rtc = new RTC_DS3231();

    if (!this->_rtc->begin()) {
        Serial.println("Couldn't find RTC");
    }

    if (this->_rtc->lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        // When time needs to be set on a new device, or after a power loss, the
        // following line sets the RTC to the date & time this sketch was compiled
        this->_rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
        // This line sets the RTC with an explicit date & time, for example to set
        // January 21, 2014 at 3am you would call:
        // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
}

int RealTime::getHour() {
    DateTime now = this->_rtc->now();
    return now.hour();
}

int RealTime::getMinute() {
    DateTime now = this->_rtc->now();
    return now.minute();
}