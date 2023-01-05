#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "Arduino.h"
#include "RTClib.h"
#include "time.h"
#include "NetworkController.h"
#include "Task.h"
#include <ezTime.h>

class RealTime: Task {
    public:
        explicit RealTime();
        int getHour();
        int getMinute();
        long getTime();

        bool hasValidTime();

    protected:
        void run() override;

    private:
        bool _rtcOnline;
        RTC_DS3231* _rtc;
        Timezone* _timezone;

        void _init();
        bool _shouldSync();
        void _sync();

};

#endif // REAL_TIME_H