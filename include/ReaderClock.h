#ifndef READER_CLOCK_H
#define READER_CLOCK_H

#include "DisplayController.h"
#include "ConfigPortal.h"
#include "RealTime.h"
#include "Quote.h"
// #include "Arduino.h"

class ReaderClock
{
public:
    explicit ReaderClock();
    void loop();

    typedef enum State
    {
        Initing,
        NoValidTime,
        RtcError,
        ShowingQuotes
    } state_t;

private:
    DisplayController *_display;
    ConfigPortal *_configPortal;
    RealTime *_time;
    QuoteList *_quotes;

    byte _currentHour;
    byte _currentMinute;
    State _state;
    State _oldState;

    void _setState(State state);
    void _updateState();
    void _updateDisplay();
    void _drawQuoteToDisplay();
    void _startConfigPortalIfNecessary();
    void _goToSleepIfNecessary();
};

#endif // READER_CLOCK_H