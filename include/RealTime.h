#ifndef REAL_TIME_H
#define REAL_TIME_H

#include "Arduino.h"
#include "NetworkController.h"
#include "RTClib.h"
#include "Task.h"
#include "time.h"
#undef EZTIME_CACHE_EEPROM
#include <ezTime.h>

class RealTime : Task {
public:
  explicit RealTime();
  int getHour();
  int getMinute();
  long getTime();

  bool hasValidTime();

protected:
  void run() override;

private:
  bool _rtcOnline;
  RTC_DS3231 *_rtc;
  Timezone *_timezone;

  void _init();
  void _handleNtpUpdate();

  static void _ntpUdateHandler(void *args);
};

#endif // REAL_TIME_H