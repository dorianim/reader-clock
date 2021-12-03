#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "Arduino.h"
#include "RTClib.h"

class RealTime {
    public:
        explicit RealTime();
        int getHour();
        int getMinute();

    private:
        RTC_DS3231* _rtc;
};

#endif // REAL_TIME_H