#include "ReaderClock.h"

ReaderClock::ReaderClock() {
    randomSeed(analogRead(0));
    this->_display = new DisplayController();
    this->_time = new RealTime();
    this->_currentHour = -1;
    this->_currentMinute = -1;
    this->_quotes = QuoteList::fromAllQuotes();
}

void ReaderClock::loop() {
    int newHour = this->_time->getHour();
    int newMinute = this->_time->getMinute();

    if(this->_currentHour != newHour || this->_currentMinute != newMinute) {
        this->_currentHour = newHour;
        this->_currentMinute = newMinute;
        this->_updateDisplay();
    }

    sleep(10000);
}

void ReaderClock::_updateDisplay() {
    Quote* quoteToDisplay = this->_findQuoteMatchingQuote(this->_currentHour, this->_currentMinute, 10);
    this->_display->showQuote(quoteToDisplay);
    delete quoteToDisplay;
}

Quote* ReaderClock::_findQuoteMatchingQuote(uint8_t hour, uint8_t minute, int accuracy) {
    // find quote
    Serial.printf("Trying to find quote for %d:%d\n", hour, minute);
    QuoteList* fittingQuotes = new QuoteList(0);
    QuoteData result = {"|No quote| found :(", "-", "-", 0, 0, false};

    for (int i = 0; i < this->_quotes->length(); i++) {
        QuoteData thisQuote = this->_quotes->at(i);
        if(thisQuote.hour == hour && thisQuote.minute == minute) {
            fittingQuotes->append(thisQuote);
        }
    }

    if(fittingQuotes->length() > 0) {
        int index = random(0, fittingQuotes->length()-1);
        result = fittingQuotes->at(index);
    } else if(accuracy > 0) {
        return this->_findQuoteMatchingQuote(hour, minute, accuracy-1);
    }

    Serial.print("Selected quote: ");
    Serial.println(result.text);

    // free memeory
    delete fittingQuotes;

    return new Quote(result);
}