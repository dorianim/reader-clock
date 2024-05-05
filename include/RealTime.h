#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "Arduino.h"
#include "RTClib.h"
#include "time.h"

class RealTime
{
public:
  enum RealTimeState
  {
    Unavailable,
    NeedsUpdate,
    Available
  };

  explicit RealTime();
  tm *getTime();
  int getHour();
  int getMinute();
  int getSecond();

  void setTime(time_t time);
  bool setTimezone(String timezone);
  RealTimeState state();

private:
  RealTimeState _state;
  RTC_DS3231 *_rtc;

  void _init();
  void _handleNtpUpdate();
  void _setTime(time_t time, bool isValid);
  bool _setTimezone(String timezone, bool store);

  static void _ntpUdateHandler(void *args);
};

#endif // REAL_TIME_H