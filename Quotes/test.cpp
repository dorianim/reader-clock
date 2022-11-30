#define PROGMEM
#include "../src/Quote.cpp"
#include <cassert>

int main(int argc, char ** argv) {
    QuoteList* list = QuoteList::fromAllQuotes();

    for(int hour = 0; hour < 24; hour++) {
        for(int minute = 0; minute < 60; minute++) {
            Quote q = list->findQuoteMatchingTime(hour, minute);
            assert(q.hour == hour);
            assert(q.minute == minute);
            assert(strcmp(q.author, "no quote found") != 0);
        }
    }
}