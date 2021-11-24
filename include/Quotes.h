#ifndef QUOTE_H
#define QUOTE_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include "stdlib.h"
#include "string.h"
#endif

typedef struct QuoteData {
    const char* text;
    const char* author;
    const char* title;
    uint8_t hour;
    uint8_t minute;
    bool notSafeForWork;
} quote_t;

class Quote {
    public:
        explicit Quote(QuoteData data);

        const char* text();
        const char* author();
        const char* title();

        const char* textBeforeTime();
        const char* timeText();
        const char* textAfterTime();

        uint8_t hour();
        uint8_t minute();
        bool notSafeForWork();

    private:
        QuoteData _data;

        char* _splitString(const char* string, const char* separator, int index, bool before=true);
};

class QuoteList {
    public:
        explicit QuoteList(size_t initialSize = 0);
        ~QuoteList();

        static QuoteList* fromAllQuotes();

        void append(QuoteData quote);
        QuoteData at(int index);
        int length();

    private:
        int _used;
        int _size;

        QuoteData* _array;
};

#endif // QUOTE_H