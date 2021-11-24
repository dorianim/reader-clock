#ifndef QUOTE_H
#define QUOTE_H

#include <Arduino.h>

typedef struct Quote {
    const char* text;
    const char* author;
    const char* title;
    byte hour;
    byte minute;
} quote_t;

class QuoteList {
    public:
        explicit QuoteList(size_t initialSize = 0);
        ~QuoteList();

        static QuoteList* fromAllQuotes();

        void append(Quote quote);
        Quote at(int index);
        int length();

    private:
        int _used;
        int _size;

        Quote* _array;
};

#endif // QUOTE_H