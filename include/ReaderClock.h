#ifndef READER_CLOCK_H
#define READER_CLOCK_H

#include "DisplayController.h"
#include "RealTime.h"
#include "Quotes.h"
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
        // If no matching Quote is found, an older Quote is used
        // It is up to accuracy minutes old
        Quote* _findQuoteMatchingQuote(uint8_t hour, uint8_t minute, int accuracy);
};

#endif // READER_CLOCK_H