#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "Arduino.h"
#include "RTClib.h"
#include "time.h"
#include "NetworkController.h"
#include <ezTime.h>

class RealTime {
    public:
        explicit RealTime();
        void loop();
        int getHour();
        int getMinute();
        long getTime();

        bool hasValidTime();

    private:
        long _lastSync;
        int _timezoneOffset;
        bool _hasValidTime;
        RTC_DS3231* _rtc;
        Timezone* _timezone;

        bool _shouldSync();
        void _sync();

};

#endif // REAL_TIME_H