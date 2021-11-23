#ifndef QUOTE_H
#define QUOTE_H

typedef struct Quote {
    const char* text;
    const char* author;
    const char* title;
    int hour;
    int minute;
} quote_t;

#endif // QUOTE_H