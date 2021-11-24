#include "Quote.h"
#include "../Quotes/quoteObject.h"
/*#ifdef ARDUINO
Quote QuoteDatas[] PROGMEM = {
#else
Quote QuoteDatas[] = {
#endif
Quote{"Zeit: ", "00:00", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 0, false},
Quote{"Zeit: ", "00:01", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 1, false},
Quote{"Zeit: ", "00:10", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 10, false},
Quote{"Zeit: ", "00:30", " Ende.", "Oscar Wilde", "The Picture of Dorian Gray ", 0, 30, false},
};*/

QuoteList::QuoteList(size_t initialSize) {
    this->_array = (Quote*)malloc(initialSize * sizeof(Quote));
    this->_used = 0;
    this->_size = initialSize;
}

QuoteList::~QuoteList() {
  free(this->_array);
  this->_array = NULL;
  this->_used = this->_size = 0;
}

void QuoteList::append(Quote quote) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (this->_used == this->_size) {
    this->_size += sizeof(Quote);
    this->_array = (Quote*)realloc(this->_array, this->_size * sizeof(Quote));
  }
  this->_array[this->_used++] = quote;
}

Quote* QuoteList::at(int index) {
    if(index < this->length()) {
        return &this->_array[index];
    }
    return nullptr;
}

int QuoteList::length() {
    return this->_used;
}

QuoteList* QuoteList::fromAllQuotes() {
    QuoteList* tmp = new QuoteList(sizeof(QuoteDatas) / sizeof(Quote));
    tmp->_array = QuoteDatas;
    tmp->_used = tmp->_size;
    return tmp;
}

Quote* QuoteList::findQuoteMatchingTime(uint8_t hour, uint8_t minute, int accuracy) {
    Quote* result = this->_findQuoteMatchingTime(hour, minute, accuracy);
    if(result == nullptr) {
        char *timeString = new char[6];
        sprintf(timeString, "%02d:%02d", hour, minute);
        return new Quote{"No qoute found, the time is: ", timeString, "", "---", "---", hour, minute, false};
    }
    return result;
}    

Quote* QuoteList::_findQuoteMatchingTime(uint8_t hour, uint8_t minute, int accuracy) {
    // find quote
    #ifdef ARDUINO
    Serial.printf("Trying to find quote for %d:%d\n", hour, minute);
    #else
    printf("Trying to find quote for %d:%d\n", hour, minute);
    #endif
    
    int matchingQuoteCount = 0;

    for (int i = 0; i < this->length(); i++) {
        Quote* thisQuote = this->at(i);
        if(thisQuote->hour == hour && thisQuote->minute == minute) {
            matchingQuoteCount ++;
        }
    }

    if(matchingQuoteCount == 0 && accuracy > 0) {
        printf("No qoute found, trying older...\n");
        return this->_findQuoteMatchingTime(hour, minute-1, accuracy-1);
    }
    else if(matchingQuoteCount == 0 && accuracy == 0) {
        #ifdef ARDUINO
        Serial.println("No qoute found.");
        #else
        printf("No qoute found.\n");
        #endif
        return nullptr;
    }

    #ifdef ARDUINO
    int chosenQuoteNumber = random(1, matchingQuoteCount);
    #else
    int chosenQuoteNumber = 1;
    #endif

    Quote* result = nullptr;
    for (int i = 0; i < this->length() && chosenQuoteNumber > 0; i++) {
        result = this->at(i);
        if(result->hour == hour && result->minute == minute) {
            chosenQuoteNumber --;
        }
    }

#ifdef ARDUINO
    Serial.printf("Selected quote: %s%s%s\n", result->textBeforeTime, result->timeText, result->textAfterTime);
#else
    printf("Selected quote: %s%s%s\n", result->textBeforeTime, result->timeText, result->textAfterTime);
#endif

    return result;
}