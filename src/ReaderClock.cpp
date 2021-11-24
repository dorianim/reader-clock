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
    // find quote
    Serial.printf("Trying to find quote for %d:%d\n", this->_currentHour, this->_currentMinute);
    QuoteList* fittingQuotes = new QuoteList(0);
    Quote result = {"|No quote| found :(", "-", "-", 0, 0};

    for (int i = 0; i < this->_quotes->length(); i++) {
        Quote thisQuote = this->_quotes->at(i);
        if(thisQuote.hour == this->_currentHour && thisQuote.minute == this->_currentMinute) {
            fittingQuotes->append(thisQuote);
        }
    }

    if(fittingQuotes->length() > 0) {
        int index = random(0, fittingQuotes->length()-1);
        result = fittingQuotes->at(index);
    }

    Serial.print("Selected quote: ");
    Serial.println(result.text);
    fittingQuotes->~QuoteList();
    
    this->_display->showQuote(&result);
}