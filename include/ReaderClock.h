#ifndef READER_CLOCK_H
#define READER_CLOCK_H

#include "DisplayController.h"
#include "RealTime.h"
#include "Quote.h"
//#include "Arduino.h"

class ReaderClock {
    public:
        explicit ReaderClock();
        void loop();

    private:
        DisplayController* _display;
        RealTime* _time;
        QuoteList* _quotes;

        byte _currentHour;
        byte _currentMinute;

        void _updateDisplay();
};

#endif // READER_CLOCK_H