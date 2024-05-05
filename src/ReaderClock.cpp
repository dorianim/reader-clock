#include "ReaderClock.h"

#define AWAKE_SECONDS_AFTER_FIRST_BOOT (1 * 60)

ReaderClock::ReaderClock() : _state(Initing), _oldState(Initing)
{
  randomSeed(analogRead(0));
  this->_display = new DisplayController();
  this->_time = new RealTime();
  this->_configPortal = new ConfigPortal(this->_time);
  this->_currentHour = -1;
  this->_currentMinute = -1;
  this->_quotes = QuoteList::fromAllQuotes();
}

void ReaderClock::loop()
{
  this->_updateState();

  this->_updateDisplay();
  vTaskDelay(100);

  this->_goToSleepIfNecessary();
  this->_startConfigPortalIfNecessary();
}

void ReaderClock::_updateDisplay()
{
  switch (this->_state)
  {
  case RtcError:
    // only do this once
    if (this->_oldState == RtcError)
      break;

    this->_display->showWarning("Internal error detected! Please restart the "
                                "device or contact support.");
    break;

  case NoValidTime:
  {
    // only do this once
    if (this->_oldState == NoValidTime)
      break;

    String message = "No valid time set! Please connect to the wifi network \"" + this->_configPortal->networkName() + "\" to sync the time.";
    this->_display->showWarning(message.c_str());
    break;
  }
  case ShowingQuotes:
    this->_drawQuoteToDisplay();
    break;

  default:
    break;
  }
}

void ReaderClock::_setState(State state)
{
  this->_oldState = this->_state;
  this->_state = state;
}

void ReaderClock::_updateState()
{
  switch (this->_time->state())
  {
  case RealTime::Unavailable:
    this->_setState(RtcError);
    break;
  case RealTime::NeedsUpdate:
    this->_setState(NoValidTime);
    break;
  case RealTime::Available:
    this->_setState(ShowingQuotes);
    break;
  }
}

void ReaderClock::_drawQuoteToDisplay()
{
  int newHour = this->_time->getHour();
  int newMinute = this->_time->getMinute();

  if (this->_currentHour != newHour || this->_currentMinute != newMinute)
  {
    Serial.printf("[I] Current time: %d:%d\n", newHour, newMinute);
    this->_currentHour = newHour;
    this->_currentMinute = newMinute;
    Quote quoteToDisplay = this->_quotes->findQuoteMatchingTime(
        this->_currentHour, this->_currentMinute);
    this->_display->showQuote(&quoteToDisplay);
  }
}

void ReaderClock::_startConfigPortalIfNecessary()
{
  if ((this->_state == NoValidTime || esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER) && !this->_configPortal->running())
  {
    this->_configPortal->start();
  }
  else if (this->_state == ShowingQuotes && this->_configPortal->running() && esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER)
  {
    this->_configPortal->stop();
  }
}

void ReaderClock::_goToSleepIfNecessary()
{
  // Rules:
  // - If there is an RTC Error, go to sleep
  // - If there is no valid timer after AWAKE_SECONDS_AFTER_FIRST_BOOT, go to sleep
  // - If we are showing quotes
  //   - If it is the first boot, go to slep after AWAKE_SECONDS_AFTER_FIRST_BOOT and wake up in time for the next minute
  //   - If it is not the first boot, go to sleep and wake up in time for the next minute

  if (this->_state != RtcError && millis() < 1000 * AWAKE_SECONDS_AFTER_FIRST_BOOT && esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_TIMER)
    return;

  Serial.println("[I] Going to deepsleep...");

  if (this->_state == ShowingQuotes)
  {
    int wakeUpInSeconds = 60 - this->_time->getSecond();
    esp_sleep_enable_timer_wakeup(wakeUpInSeconds * 1000 * 1000);
  }

  if (this->_state == NoValidTime)
    this->_display->showWarning("No valid time set! Please disconnect the power and connect it again to configure.");

  esp_deep_sleep_start();
}