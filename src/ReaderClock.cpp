#include "ReaderClock.h"

ReaderClock::ReaderClock() : _state(Initing), _oldState(Initing) {
    randomSeed(analogRead(0));
    this->_display = new DisplayController();
    this->_networkController = new NetworkController();
    this->_time = new RealTime();
    this->_currentHour = -1;
    this->_currentMinute = -1;
    this->_quotes = QuoteList::fromAllQuotes();
}

void ReaderClock::loop() {
    this->_updateDisplay();
    this->_networkController->loop();
    this->_time->loop();
}

void ReaderClock::_updateDisplay() {
    this->_updateState();
    this->_drawQuoteToDisplay();
    this->_drawTimeErrorToDisplay();
}

void ReaderClock::_setState(State state) {
    this->_oldState = this->_state;
    this->_state = state;
}

void ReaderClock::_updateState() {
    this->_oldState = this->_state;
    if(this->_time->hasValidTime()) {
        this->_setState(ShowingQuotes);
    }
    else {
        this->_setState(NoValidTime);
    }
}

void ReaderClock::_drawQuoteToDisplay() {
    if(this->_state != ShowingQuotes) return;

    int newHour = this->_time->getHour();
    int newMinute = this->_time->getMinute();

    if(this->_currentHour != newHour || this->_currentMinute != newMinute) {
        Serial.printf("Current time: %d:%d\n", newHour, newMinute);
        this->_currentHour = newHour;
        this->_currentMinute = newMinute;
        Quote quoteToDisplay = this->_quotes->findQuoteMatchingTime(this->_currentHour, this->_currentMinute);
        this->_display->showQuote(&quoteToDisplay);
    }
}

void ReaderClock::_drawTimeErrorToDisplay() {
    if(this->_state != NoValidTime || this->_oldState == NoValidTime) return;
    this->_display->showWarning("No valid time set! Please connect to a wifi network in order to sync the time.");
}