#ifndef QUOTE_H
#define QUOTE_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#endif

typedef struct Quote {
    const char* textBeforeTime;
    const char* timeText;
    const char* textAfterTime;
    const char* author;
    const char* title;
    uint8_t hour;
    uint8_t minute;
    bool notSafeForWork;
} quote_t;

class QuoteList {
    public:
        explicit QuoteList(size_t initialSize = 0);
        ~QuoteList();

        static QuoteList* fromAllQuotes();

        void append(Quote quote);
        Quote* at(int index);
        int length();

        // If no matching Quote is found, an older Quote is used
        // It is up to accuracy minutes old
        Quote* findQuoteMatchingTime(uint8_t hour, uint8_t minute, int accuracy);
    private:
        int _used;
        int _size;

        Quote* _array;

        Quote* _findQuoteMatchingTime(uint8_t hour, uint8_t minute, int accuracy);
};

#endif // QUOTE_H