#include "ReaderClock.h"

ReaderClock::ReaderClock() {
    randomSeed(analogRead(0));
    this->_display = new DisplayController();
    this->_networkController = new NetworkController();
    this->_time = new RealTime();
    this->_currentHour = -1;
    this->_currentMinute = -1;
    this->_quotes = QuoteList::fromAllQuotes();
}

void ReaderClock::loop() {
    int newHour = this->_time->getHour();
    int newMinute = this->_time->getMinute();

    if(this->_currentHour != newHour || this->_currentMinute != newMinute) {
        Serial.printf("Current time: %d:%d\n", newHour, newMinute);
        this->_currentHour = newHour;
        this->_currentMinute = newMinute;
        this->_updateDisplay();
    }

    this->_networkController->loop();
}

void ReaderClock::_updateDisplay() {
    Quote* quoteToDisplay = this->_quotes->findQuoteMatchingTime(this->_currentHour, this->_currentMinute, 10);
    this->_display->showQuote(quoteToDisplay);
}