#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "Arduino.h"

class RealTime {
    public:
        explicit RealTime();
        int getHour();
        int getMinute();
};

#endif // REAL_TIME_H