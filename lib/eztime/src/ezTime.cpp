#include <Arduino.h>

#include <ezTime.h>

#ifdef EZTIME_NETWORK_ENABLE
#ifdef EZTIME_CACHE_NVS
#include <Preferences.h> // For timezone lookup cache
#endif
#ifdef EZTIME_CACHE_EEPROM
#include <EEPROM.h>
#endif
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#elif defined(EZTIME_ETHERNET)
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#else
#include <WiFi.h>
#include <WiFiUdp.h>
#endif
#endif

#if defined(EZTIME_MAX_DEBUGLEVEL_NONE)
#define err(args...) ""
#define errln(args...) ""
#define info(args...) ""
#define infoln(args...) ""
#define debug(args...) ""
#define debugln(args...) ""
#elif defined(EZTIME_MAX_DEBUGLEVEL_ERROR)
#define err(args...)                                                           \
  if (_debug_level >= ERROR)                                                   \
  _debug_device->print(args)
#define errln(args...)                                                         \
  if (_debug_level >= ERROR)                                                   \
  _debug_device->println(args)
#define info(args...) ""
#define infoln(args...) ""
#define debug(args...) ""
#define debugln(args...) ""
#elif defined(EZTIME_MAX_DEBUGLEVEL_INFO)
#define err(args...)                                                           \
  if (_debug_level >= ERROR)                                                   \
  _debug_device->print(args)
#define errln(args...)                                                         \
  if (_debug_level >= ERROR)                                                   \
  _debug_device->println(args)
#define info(args...)                                                          \
  if (_debug_level >= INFO)                                                    \
  _debug_device->print(args)
#define infoln(args...)                                                        \
  if (_debug_level >= INFO)                                                    \
  _debug_device->println(args)
#define debug(args...) ""
#define debugln(args...) ""
#else // nothing specified compiles everything in.
#define err(args...)                                                           \
  if (_debug_level >= ERROR)                                                   \
  _debug_device->print(args)
#define errln(args...)                                                         \
  if (_debug_level >= ERROR)                                                   \
  _debug_device->println(args)
#define info(args...)                                                          \
  if (_debug_level >= INFO)                                                    \
  _debug_device->print(args)
#define infoln(args...)                                                        \
  if (_debug_level >= INFO)                                                    \
  _debug_device->println(args)
#define debug(args...)                                                         \
  if (_debug_level >= DEBUG)                                                   \
  _debug_device->print(args)
#define debugln(args...)                                                       \
  if (_debug_level >= DEBUG)                                                   \
  _debug_device->println(args)
#endif

const uint8_t monthDays[] = {
    31, 28, 31, 30, 31, 30, 31,
    31, 30, 31, 30, 31}; // API starts months from 1, this array starts from 0

// The private things go in an anonymous namespace
namespace {

ezError_t _last_error = NO_ERROR;
String _server_error = "";
ezDebugLevel_t _debug_level = NONE;
Print *_debug_device = (Print *)&Serial;
ezEvent_t _events[MAX_EVENTS];
time_t _last_sync_time = 0;
time_t _last_read_t = 0;
uint32_t _last_sync_millis = 0;
uint16_t _last_read_ms;
timeStatus_t _time_status;
bool _initialised = false;
#ifdef EZTIME_NETWORK_ENABLE
uint16_t _ntp_interval = NTP_INTERVAL;
String _ntp_server = NTP_SERVER;
ntpUpdateHandler_t _ntp_update_handler = nullptr;
void *_ntp_update_handler_args = nullptr;
#endif

void triggerError(const ezError_t err) {
  _last_error = err;
  if (_last_error) {
    err(F("ERROR: "));
    errln(ezt::errorString(err));
  }
}

String debugLevelString(const ezDebugLevel_t level) {
  switch (level) {
  case NONE:
    return F("NONE");
  case ERROR:
    return F("ERROR");
  case INFO:
    return F("INFO");
  default:
    return F("DEBUG");
  }
}

time_t nowUTC(const bool update_last_read = true) {
  time_t t;
  uint32_t m = millis();
  t = _last_sync_time + ((m - _last_sync_millis) / 1000);
  if (update_last_read) {
    _last_read_t = t;
    _last_read_ms = (m - _last_sync_millis) % 1000;
  }
  return t;
}

} // namespace

namespace ezt {

////////// Error handing

String errorString(const ezError_t err /* = LAST_ERROR */) {
  switch (err) {
  case NO_ERROR:
    return F("OK");
  case LAST_ERROR:
    return errorString(_last_error);
  case NO_NETWORK:
    return F("No network");
  case TIMEOUT:
    return F("Timeout");
  case CONNECT_FAILED:
    return F("Connect Failed");
  case DATA_NOT_FOUND:
    return F("Data not found");
  case LOCKED_TO_UTC:
    return F("Locked to UTC");
  case NO_CACHE_SET:
    return F("No cache set");
  case CACHE_TOO_SMALL:
    return F("Cache too small");
  case TOO_MANY_EVENTS:
    return F("Too many events");
  case INVALID_DATA:
    return F("Invalid data received from NTP server");
  case SERVER_ERROR:
    return _server_error;
  default:
    return F("Unkown error");
  }
}

ezError_t error(const bool reset /* = false */) {
  ezError_t tmp = _last_error;
  if (reset)
    _last_error = NO_ERROR;
  return tmp;
}

void setDebug(const ezDebugLevel_t level) { setDebug(level, *_debug_device); }

void setDebug(const ezDebugLevel_t level, Print &device) {
  _debug_level = level;
  _debug_device = &device;
  info(F("\r\nezTime debug level set to "));
  infoln(debugLevelString(level));
}

//

timeStatus_t timeStatus() { return _time_status; }

void events() {
  if (!_initialised) {
    for (uint8_t n = 0; n < MAX_EVENTS; n++)
      _events[n] = {0, NULL};
#ifdef EZTIME_NETWORK_ENABLE
    if (_ntp_interval)
      updateNTP(); // Start the cycle of updateNTP running and then setting an
                   // event for its next run
#endif
    _initialised = true;
  }
  // See if any events are due
  for (uint8_t n = 0; n < MAX_EVENTS; n++) {
    if (_events[n].function && nowUTC(false) >= _events[n].time) {
      debug(F("Running event (#"));
      debug(n + 1);
      debug(F(") set for "));
      void (*tmp)() = _events[n].function;
      _events[n] = {0, NULL}; // reset the event
      (tmp)();                // execute the function
    }
  }
  yield();
}

void deleteEvent(const uint8_t event_handle) {
  if (event_handle && event_handle <= MAX_EVENTS) {
    debug(F("Deleted event (#"));
    debug(event_handle);
    debug(F("), set for "));
    _events[event_handle - 1] = {0, NULL};
  }
}

void deleteEvent(void (*function)()) {
  for (uint8_t n = 0; n < MAX_EVENTS; n++) {
    if (_events[n].function == function) {
      debug(F("Deleted event (#"));
      debug(n + 1);
      debug(F("), set for "));
      _events[n] = {0, NULL};
    }
  }
}

void breakTime(const time_t timeInput, tmElements_t &tm) {
  // break the given time_t into time components
  // this is a more compact version of the C library localtime function
  // note that year is offset from 1970 !!!

  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)timeInput;
  tm.Second = time % 60;
  time /= 60; // now it is minutes
  tm.Minute = time % 60;
  time /= 60; // now it is hours
  tm.Hour = time % 24;
  time /= 24;                     // now it is days
  tm.Wday = ((time + 4) % 7) + 1; // Sunday is day 1

  year = 0;
  days = 0;
  while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  tm.Year = year; // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time -= days; // now it is days in this year, starting at 0

  days = 0;
  month = 0;
  monthLength = 0;
  for (month = 0; month < 12; month++) {
    if (month == 1) { // february
      if (LEAP_YEAR(year)) {
        monthLength = 29;
      } else {
        monthLength = 28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
      break;
    }
  }
  tm.Month = month + 1; // jan is month 1
  tm.Day = time + 1;    // day of month
}

time_t makeTime(const uint8_t hour, const uint8_t minute, const uint8_t second,
                const uint8_t day, const uint8_t month, const uint16_t year) {
  tmElements_t tm;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;
  tm.Day = day;
  tm.Month = month;
  if (year > 68) { // time_t cannot reach beyond 68 + 1970 anyway, so if bigger
                   // user means actual years
    tm.Year = year - 1970;
  } else {
    tm.Year = year;
  }
  return makeTime(tm);
}

time_t makeTime(tmElements_t &tm) {
  // assemble time elements into time_t
  // note year argument is offset from 1970 (see macros in time.h to convert to
  // other formats) previous version used full four digit year (or digits since
  // 2000),i.e. 2009 was 2009 or 9

  int i;
  uint32_t seconds;

  // seconds from 1970 till 1 jan 00:00:00 of the given year
  seconds = tm.Year * SECS_PER_DAY * 365UL;

  for (i = 0; i < tm.Year; i++) {
    if (LEAP_YEAR(i)) {
      seconds += SECS_PER_DAY; // add extra days for leap years
    }
  }

  // add days for this year, months start from 1
  for (i = 1; i < tm.Month; i++) {
    if ((i == 2) && LEAP_YEAR(tm.Year)) {
      seconds += SECS_PER_DAY * 29UL;
    } else {
      seconds += SECS_PER_DAY *
                 (uint32_t)monthDays[i - 1]; // monthDay array starts from 0
    }
  }

  seconds += (tm.Day - 1) * SECS_PER_DAY;
  seconds += tm.Hour * 3600UL;
  seconds += tm.Minute * 60UL;
  seconds += tm.Second;

  return (time_t)seconds;
}

// makeOrdinalTime allows you to resolve "second thursday in September in 2018"
// into a number of seconds since 1970 (Very useful for the timezone
// calculations that ezTime does internally) If ordinal is 0 or 5 it is taken to
// mean "the last $wday in $month"
time_t makeOrdinalTime(const uint8_t hour, const uint8_t minute,
                       uint8_t const second, uint8_t ordinal,
                       const uint8_t wday, const uint8_t month, uint16_t year) {
  if (year <= 68)
    year = 1970 + year; // fix user intent
  uint8_t m = month;
  uint8_t w = ordinal;
  if (w == 5) {
    ordinal = 0;
    w = 0;
  }
  if (w == 0) {     // is this a "Last week" rule?
    if (++m > 12) { // yes, for "Last", go to the next month
      m = 1;
      ++year;
    }
    w = 1; // and treat as first week of next month, subtract 7 days later
  }
  time_t t = makeTime(hour, minute, second, 1, m, year);
  // add offset from the first of the month to weekday, and offset for the given
  // week
  t += ((wday - UTC.weekday(t) + 7) % 7 + (w - 1) * 7) * SECS_PER_DAY;
  // back up a week if this is a "Last" rule
  if (ordinal == 0)
    t -= 7 * SECS_PER_DAY;
  return t;
}

String zeropad(const uint32_t number, const uint8_t length) {
  String out;
  out.reserve(length);
  out = String(number);
  while (out.length() < length)
    out = "0" + out;
  return out;
}

bool secondChanged() {
  time_t t = nowUTC(false);
  if (_last_read_t != t)
    return true;
  return false;
}

bool minuteChanged() {
  time_t t = nowUTC(false);
  if (_last_read_t / 60 != t / 60)
    return true;
  return false;
}

#ifdef EZTIME_NETWORK_ENABLE

void updateNTP() {
  deleteEvent(
      updateNTP); // Delete any events pointing here, in case called manually
  time_t t;
  unsigned long measured_at;

  if (!queryNTP(_ntp_server, t, measured_at)) {

    if (nowUTC(false) > _last_sync_time + _ntp_interval + NTP_STALE_AFTER) {
      _time_status = timeNeedsSync;
    }
    UTC.setEvent(updateNTP, nowUTC(false) + NTP_RETRY);
    return;
  }

  int32_t correction =
      ((t - _last_sync_time) * 1000) - (measured_at - _last_sync_millis);
  _last_sync_time = t;
  _last_sync_millis = measured_at;
  _last_read_ms = (millis() - measured_at) % 1000;

  info(F("Received time: "));
  if (_time_status != timeNotSet) {
    info(F(" (internal clock was "));
    if (!correction) {
      infoln(F("spot on)"));
    } else {
      info(String(abs(correction)));
      if (correction > 0) {
        infoln(F(" ms fast)"));
      } else {
        infoln(F(" ms slow)"));
      }
    }
  } else {
    infoln("");
  }

  if (_ntp_interval)
    UTC.setEvent(updateNTP, t + _ntp_interval);

  _time_status = timeSet;

  if (_ntp_update_handler != nullptr) {
    _ntp_update_handler(_ntp_update_handler_args);
  }
}

// This is a nice self-contained NTP routine if you need one: feel free to use
// it. It gives you the seconds since 1970 (unix epoch) and the millis() on your
// system when that happened (by deducting fractional seconds and estimated
// network latency).
bool queryNTP(const String server, time_t &t, unsigned long &measured_at) {
  info(F("Querying "));
  info(server);
  info(F(" ... "));

  if (WiFi.status() != WL_CONNECTED) {
    triggerError(NO_NETWORK);
    return false;
  }
  WiFiUDP udp;

  // Send NTP packet
  byte buffer[NTP_PACKET_SIZE];
  memset(buffer, 0, NTP_PACKET_SIZE);
  buffer[0] = 0b11100011; // LI, Version, Mode
  buffer[1] = 0;          // Stratum, or type of clock
  buffer[2] = 9;    // Polling Interval (9 = 2^9 secs = ~9 mins, close to our 10
                    // min default)
  buffer[3] = 0xEC; // Peer Clock Precision
                    // 8 bytes of zero for Root Delay & Root Dispersion
  buffer[12] = 'X'; // "kiss code", see RFC5905
  buffer[13] = 'E'; // (codes starting with 'X' are not interpreted)
  buffer[14] = 'Z';
  buffer[15] = 'T';

  udp.flush();
  udp.begin(NTP_LOCAL_PORT);
  unsigned long started = millis();
  udp.beginPacket(server.c_str(), 123); // NTP requests are to port 123
  udp.write(buffer, NTP_PACKET_SIZE);
  udp.endPacket();

  // Wait for packet or return false with timed out
  while (!udp.parsePacket()) {
    delay(1);
    if (millis() - started > NTP_TIMEOUT) {
      udp.stop();
      triggerError(TIMEOUT);
      return false;
    }
  }
  udp.read(buffer, NTP_PACKET_SIZE);
  udp.stop(); // On AVR there's only very limited sockets, we want to free them
              // when done.

  // print out received packet for debug
  int i;
  debug(F("Received data:"));
  for (i = 0; i < NTP_PACKET_SIZE; i++) {
    if ((i % 4) == 0) {
      debugln();
      debug(String(i) + ": ");
    }
    debug(buffer[i], HEX);
    debug(F(", "));
  }
  debugln();

  // prepare timestamps
  uint32_t highWord, lowWord;
  highWord = (buffer[16] << 8 | buffer[17]) & 0x0000FFFF;
  lowWord = (buffer[18] << 8 | buffer[19]) & 0x0000FFFF;
  uint32_t reftsSec = highWord << 16 | lowWord; // reference timestamp seconds

  highWord = (buffer[32] << 8 | buffer[33]) & 0x0000FFFF;
  lowWord = (buffer[34] << 8 | buffer[35]) & 0x0000FFFF;
  uint32_t rcvtsSec = highWord << 16 | lowWord; // receive timestamp seconds

  highWord = (buffer[40] << 8 | buffer[41]) & 0x0000FFFF;
  lowWord = (buffer[42] << 8 | buffer[43]) & 0x0000FFFF;
  uint32_t secsSince1900 =
      highWord << 16 | lowWord; // transmit timestamp seconds

  highWord = (buffer[44] << 8 | buffer[45]) & 0x0000FFFF;
  lowWord = (buffer[46] << 8 | buffer[47]) & 0x0000FFFF;
  uint32_t fraction = highWord << 16 | lowWord; // transmit timestamp fractions

  // check if received data makes sense
  // buffer[1] = stratum - should be 1..15 for valid reply
  // also checking that all timestamps are non-zero and receive timestamp
  // seconds are <= transmit timestamp seconds
  if ((buffer[1] < 1) or (buffer[1] > 15) or (reftsSec == 0) or
      (rcvtsSec == 0) or (rcvtsSec > secsSince1900)) {
    // we got invalid packet
    triggerError(INVALID_DATA);
    return false;
  }

  // Set the t and measured_at variables that were passed by reference
  uint32_t done = millis();
  info(F("success (round trip "));
  info(done - started);
  infoln(F(" ms)"));
  t = secsSince1900 -
      2208988800UL; // Subtract 70 years to get seconds since 1970
  uint16_t ms =
      fraction /
      4294967UL; // Turn 32 bit fraction into ms by dividing by 2^32 / 1000
  measured_at = done - ((done - started) / 2) -
                ms; // Assume symmetric network latency and return when we think
                    // the whole second was.

  return true;
}

void setInterval(const uint16_t seconds /* = 0 */) {
  deleteEvent(updateNTP);
  _ntp_interval = seconds;
  if (seconds)
    UTC.setEvent(updateNTP, nowUTC(false) + _ntp_interval);
}

void setServer(const String ntp_server /* = NTP_SERVER */) {
  _ntp_server = ntp_server;
}

bool waitForSync(const uint16_t timeout /* = 0 */) {

  unsigned long start = millis();

  if (WiFi.status() != WL_CONNECTED) {
    info(F("Waiting for WiFi ... "));
    while (WiFi.status() != WL_CONNECTED) {
      if (timeout && (millis() - start) / 1000 > timeout) {
        triggerError(TIMEOUT);
        return false;
      };
      events();
      delay(25);
    }
    infoln(F("connected"));
  }

  if (_time_status != timeSet) {
    infoln(F("Waiting for time sync"));
    while (_time_status != timeSet) {
      if (timeout && (millis() - start) / 1000 > timeout) {
        triggerError(TIMEOUT);
        return false;
      };
      delay(250);
      events();
    }
    infoln(F("Time is in sync"));
  }
  return true;
}

time_t lastNtpUpdateTime() { return _last_sync_time; }

void setNtpUpdateHandler(ntpUpdateHandler_t handler, void *args) {
  _ntp_update_handler = handler;
  _ntp_update_handler_args = args;
}

#endif // EZTIME_NETWORK_ENABLE

} // namespace ezt

//
// Timezone class
//

Timezone::Timezone(const bool locked_to_UTC /* = false */) {
  _locked_to_UTC = locked_to_UTC;
  _posix = "UTC";
#ifdef EZTIME_NETWORK_ENABLE
  _olson = "";
#endif
}

bool Timezone::setPosix(const String posix) {
  if (_locked_to_UTC) {
    triggerError(LOCKED_TO_UTC);
    return false;
  }
  _posix = posix;
#ifdef EZTIME_NETWORK_ENABLE
  _olson = "";
#endif
  return true;
}

time_t Timezone::now() { return tzTime(); }

time_t Timezone::tzTime(time_t t /* = TIME_NOW */,
                        ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  if (_locked_to_UTC)
    return nowUTC(); // just saving some time and memory
  String tzname;
  bool is_dst;
  int16_t offset;
  return tzTime(t, local_or_utc, tzname, is_dst, offset);
}

time_t Timezone::tzTime(time_t t, ezLocalOrUTC_t local_or_utc, String &tzname,
                        bool &is_dst, int16_t &offset) {

  if (t == TIME_NOW) {
    t = nowUTC();
    local_or_utc = UTC_TIME;
  } else if (t == LAST_READ) {
    t = _last_read_t;
    local_or_utc = UTC_TIME;
  }

  int8_t offset_hr = 0;
  uint8_t offset_min = 0;
  int8_t dst_shift_hr = 1;
  uint8_t dst_shift_min = 0;

  uint8_t start_month = 0, start_week = 0, start_dow = 0, start_time_hr = 2,
          start_time_min = 0;
  uint8_t end_month = 0, end_week = 0, end_dow = 0, end_time_hr = 2,
          end_time_min = 0;

  enum posix_state_e {
    STD_NAME,
    OFFSET_HR,
    OFFSET_MIN,
    DST_NAME,
    DST_SHIFT_HR,
    DST_SHIFT_MIN,
    START_MONTH,
    START_WEEK,
    START_DOW,
    START_TIME_HR,
    START_TIME_MIN,
    END_MONTH,
    END_WEEK,
    END_DOW,
    END_TIME_HR,
    END_TIME_MIN
  };
  posix_state_e state = STD_NAME;

  bool ignore_nums = false;
  char c = 1; // Dummy value to get while(newchar) started
  uint8_t strpos = 0;
  uint8_t stdname_end = _posix.length() - 1;
  uint8_t dstname_begin = _posix.length();
  uint8_t dstname_end = _posix.length();

  while (strpos < _posix.length()) {
    c = (char)_posix[strpos];

    // Do not replace the code below with switch statement: evaluation of state
    // that changes while this runs. (Only works because this state can only go
    // forward.)

    if (c && state == STD_NAME) {
      if (c == '<')
        ignore_nums = true;
      if (c == '>')
        ignore_nums = false;
      if (!ignore_nums && (isDigit(c) || c == '-' || c == '+')) {
        state = OFFSET_HR;
        stdname_end = strpos - 1;
      }
    }
    if (c && state == OFFSET_HR) {
      if (c == '+') {
        // Ignore the plus
      } else if (c == ':') {
        state = OFFSET_MIN;
        c = 0;
      } else if (c != '-' && !isDigit(c)) {
        state = DST_NAME;
        dstname_begin = strpos;
      } else {
        if (!offset_hr)
          offset_hr = atoi(_posix.c_str() + strpos);
      }
    }
    if (c && state == OFFSET_MIN) {
      if (!isDigit(c)) {
        state = DST_NAME;
        ignore_nums = false;
      } else {
        if (!offset_min)
          offset_min = atoi(_posix.c_str() + strpos);
      }
    }
    if (c && state == DST_NAME) {
      if (c == '<')
        ignore_nums = true;
      if (c == '>')
        ignore_nums = false;
      if (c == ',') {
        state = START_MONTH;
        c = 0;
        dstname_end = strpos - 1;
      } else if (!ignore_nums && (c == '-' || isDigit(c))) {
        state = DST_SHIFT_HR;
        dstname_end = strpos - 1;
      }
    }
    if (c && state == DST_SHIFT_HR) {
      if (c == ':') {
        state = DST_SHIFT_MIN;
        c = 0;
      } else if (c == ',') {
        state = START_MONTH;
        c = 0;
      } else if (dst_shift_hr == 1)
        dst_shift_hr = atoi(_posix.c_str() + strpos);
    }
    if (c && state == DST_SHIFT_MIN) {
      if (c == ',') {
        state = START_MONTH;
        c = 0;
      } else if (!dst_shift_min)
        dst_shift_min = atoi(_posix.c_str() + strpos);
    }
    if (c && state == START_MONTH) {
      if (c == '.') {
        state = START_WEEK;
        c = 0;
      } else if (c != 'M' && !start_month)
        start_month = atoi(_posix.c_str() + strpos);
    }
    if (c && state == START_WEEK) {
      if (c == '.') {
        state = START_DOW;
        c = 0;
      } else
        start_week = c - '0';
    }
    if (c && state == START_DOW) {
      if (c == '/') {
        state = START_TIME_HR;
        c = 0;
      } else if (c == ',') {
        state = END_MONTH;
        c = 0;
      } else
        start_dow = c - '0';
    }
    if (c && state == START_TIME_HR) {
      if (c == ':') {
        state = START_TIME_MIN;
        c = 0;
      } else if (c == ',') {
        state = END_MONTH;
        c = 0;
      } else if (start_time_hr == 2)
        start_time_hr = atoi(_posix.c_str() + strpos);
    }
    if (c && state == START_TIME_MIN) {
      if (c == ',') {
        state = END_MONTH;
        c = 0;
      } else if (!start_time_min)
        start_time_min = atoi(_posix.c_str() + strpos);
    }
    if (c && state == END_MONTH) {
      if (c == '.') {
        state = END_WEEK;
        c = 0;
      } else if (c != 'M')
        if (!end_month)
          end_month = atoi(_posix.c_str() + strpos);
    }
    if (c && state == END_WEEK) {
      if (c == '.') {
        state = END_DOW;
        c = 0;
      } else
        end_week = c - '0';
    }
    if (c && state == END_DOW) {
      if (c == '/') {
        state = END_TIME_HR;
        c = 0;
      } else
        end_dow = c - '0';
    }
    if (c && state == END_TIME_HR) {
      if (c == ':') {
        state = END_TIME_MIN;
        c = 0;
      } else if (end_time_hr == 2)
        end_time_hr = atoi(_posix.c_str() + strpos);
    }
    if (c && state == END_TIME_MIN) {
      if (!end_time_min)
        end_time_min = atoi(_posix.c_str() + strpos);
    }
    strpos++;
  }

  int16_t std_offset = (offset_hr < 0) ? offset_hr * 60 - offset_min
                                       : offset_hr * 60 + offset_min;

  tzname = _posix.substring(
      0, stdname_end + 1); // Overwritten with dstname later if needed
  if (!start_month) {
    if (tzname == "UTC" && std_offset)
      tzname = "???";
    is_dst = false;
    offset = std_offset;
  } else {
    int16_t dst_offset = std_offset - dst_shift_hr * 60 - dst_shift_min;
    // to find the year
    tmElements_t tm;
    ezt::breakTime(t, tm);

    // in local time
    time_t dst_start =
        ezt::makeOrdinalTime(start_time_hr, start_time_min, 0, start_week,
                             start_dow + 1, start_month, tm.Year + 1970);
    time_t dst_end =
        ezt::makeOrdinalTime(end_time_hr, end_time_min, 0, end_week,
                             end_dow + 1, end_month, tm.Year + 1970);

    if (local_or_utc == UTC_TIME) {
      dst_start += std_offset * 60LL;
      dst_end += dst_offset * 60LL;
    }

    if (dst_end > dst_start) {
      is_dst = (t >= dst_start && t < dst_end); // northern hemisphere
    } else {
      is_dst = !(t >= dst_end && t < dst_start); // southern hemisphere
    }

    if (is_dst) {
      offset = dst_offset;
      tzname = _posix.substring(dstname_begin, dstname_end + 1);
    } else {
      offset = std_offset;
    }
  }

  if (local_or_utc == LOCAL_TIME) {
    return t + offset * 60LL;
  } else {
    return t - offset * 60LL;
  }
}

String Timezone::getPosix() { return _posix; }

#ifdef EZTIME_NETWORK_ENABLE

bool Timezone::setLocation(const String location /* = "GeoIP" */) {

  info(F("Timezone lookup for: "));
  info(location);
  info(F(" ... "));
  if (_locked_to_UTC) {
    triggerError(LOCKED_TO_UTC);
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    triggerError(NO_NETWORK);
    return false;
  }
  WiFiUDP udp;

  udp.flush();
  udp.begin(TIMEZONED_LOCAL_PORT);
  unsigned long started = millis();
  udp.beginPacket(TIMEZONED_REMOTE_HOST, TIMEZONED_REMOTE_PORT);
  udp.write((const uint8_t *)location.c_str(), location.length());
  udp.endPacket();

  // Wait for packet or return false with timed out
  while (!udp.parsePacket()) {
    delay(1);
    if (millis() - started > TIMEZONED_TIMEOUT) {
      udp.stop();
      triggerError(TIMEOUT);
      return false;
    }
  }
  // Stick result in String recv
  String recv;
  recv.reserve(60);
  while (udp.available())
    recv += (char)udp.read();
  udp.stop();
  info(F("(round-trip "));
  info(millis() - started);
  info(F(" ms)  "));
  if (recv.substring(0, 6) == "ERROR ") {
    _server_error = recv.substring(6);
    ezt::errorString(SERVER_ERROR);
    return false;
  }
  if (recv.substring(0, 3) == "OK ") {
    _olson = recv.substring(3, recv.indexOf(" ", 4));
    _posix = recv.substring(recv.indexOf(" ", 4) + 1);
    infoln(F("success."));
    info(F("  Olson: "));
    infoln(_olson);
    info(F("  Posix: "));
    infoln(_posix);
    return true;
  }
  ezt::errorString(DATA_NOT_FOUND);
  return false;
}
#endif
String Timezone::getOlson() { return _olson; }

String Timezone::getOlsen() { return _olson; }

bool Timezone::isDST(time_t t /*= TIME_NOW */,
                     const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  String tzname;
  bool is_dst;
  int16_t offset;
  t = tzTime(t, local_or_utc, tzname, is_dst, offset);
  return is_dst;
}

String Timezone::getTimezoneName(
    time_t t /*= TIME_NOW */,
    const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  String tzname;
  bool is_dst;
  int16_t offset;
  t = tzTime(t, local_or_utc, tzname, is_dst, offset);
  return tzname;
}

int16_t
Timezone::getOffset(time_t t /*= TIME_NOW */,
                    const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  String tzname;
  bool is_dst;
  int16_t offset;
  t = tzTime(t, local_or_utc, tzname, is_dst, offset);
  return offset;
}

uint8_t Timezone::setEvent(void (*function)(), const uint8_t hr,
                           const uint8_t min, const uint8_t sec,
                           const uint8_t day, const uint8_t mnth, uint16_t yr) {
  time_t t = ezt::makeTime(hr, min, sec, day, mnth, yr);
  return setEvent(function, t);
}

uint8_t
Timezone::setEvent(void (*function)(), time_t t /* = TIME_NOW */,
                   const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  for (uint8_t n = 0; n < MAX_EVENTS; n++) {
    if (!_events[n].function) {
      _events[n].function = function;
      _events[n].time = t;
      debug(F("Set event (#"));
      debug(n + 1);
      debug(F(") to trigger on: "));
      return n + 1;
    }
  }
  triggerError(TOO_MANY_EVENTS);
  return 0;
}

void Timezone::setTime(const time_t t, const uint16_t ms /* = 0 */) {
  int16_t offset;
  offset = getOffset(t);
  _last_sync_time = t + offset * 60;
  _last_sync_millis = millis() - ms;
  _time_status = timeSet;
}

void Timezone::setTime(const uint8_t hr, const uint8_t min, const uint8_t sec,
                       const uint8_t day, const uint8_t mnth, uint16_t yr) {
  tmElements_t tm;
  // year can be given as full four digit year or two digts (2010 or 10 for
  // 2010); it is converted to years since 1970
  if (yr > 99) {
    yr = yr - 1970;
  } else {
    yr += 30;
  }
  tm.Year = yr;
  tm.Month = mnth;
  tm.Day = day;
  tm.Hour = hr;
  tm.Minute = min;
  tm.Second = sec;
  setTime(ezt::makeTime(tm));
}

String
Timezone::militaryTZ(time_t t /*= TIME_NOW */,
                     const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  int16_t o = getOffset(t);
  if (o % 60)
    return "?"; // If it's not a whole hour from UTC, it's not a timezone with a
                // military letter code
  o = o / 60;
  if (o > 0)
    return String((char)('M' + o));
  if (o < 0 && o >= -9)
    return String((char)('A' - o - 1)); // Minus a negative number == plus 1
  if (o < -9)
    return String((char)('A' - o)); // Crazy, they're skipping 'J'
  return "Z";
}

uint8_t Timezone::hour(time_t t /*= TIME_NOW */,
                       const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  return t / 3600 % 24;
}

uint8_t Timezone::minute(time_t t /*= TIME_NOW */,
                         const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  return t / 60 % 60;
}

uint8_t Timezone::second(time_t t /*= TIME_NOW */,
                         const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  return t % 60;
}

uint16_t Timezone::ms(time_t t /*= TIME_NOW */) {
  // Note that here passing anything but TIME_NOW or LAST_READ is pointless
  if (t == TIME_NOW) {
    nowUTC();
    return _last_read_ms;
  }
  if (t == LAST_READ)
    return _last_read_ms;
  return 0;
}

uint8_t Timezone::day(time_t t /*= TIME_NOW */,
                      const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  tmElements_t tm;
  ezt::breakTime(t, tm);
  return tm.Day;
}

uint8_t
Timezone::weekday(time_t t /*= TIME_NOW */,
                  const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  tmElements_t tm;
  ezt::breakTime(t, tm);
  return tm.Wday;
}

uint8_t Timezone::month(time_t t /*= TIME_NOW */,
                        const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  tmElements_t tm;
  ezt::breakTime(t, tm);
  return tm.Month;
}

uint16_t Timezone::year(time_t t /*= TIME_NOW */,
                        const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  tmElements_t tm;
  ezt::breakTime(t, tm);
  return tm.Year + 1970;
}

uint16_t
Timezone::dayOfYear(time_t t /*= TIME_NOW */,
                    const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  time_t jan_1st = ezt::makeTime(0, 0, 0, 1, 1, year(t));
  return (t - jan_1st) / SECS_PER_DAY;
}

uint8_t
Timezone::hourFormat12(time_t t /*= TIME_NOW */,
                       const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  uint8_t h = t / 3600 % 12;
  if (h)
    return h;
  return 12;
}

bool Timezone::isAM(time_t t /*= TIME_NOW */,
                    const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  return (t / 3600 % 24 < 12);
}

bool Timezone::isPM(time_t t /*= TIME_NOW */,
                    const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  return (t / 3600 % 24 >= 12);
}

// Now this is where this gets a little obscure. The ISO year can be different
// from the actual (Gregorian) year. That is: you can be in january and still be
// in week 53 of past year, _and_ you can be in december and be in week one of
// the next. The ISO 8601 definition for week 01 is the week with the Gregorian
// year's first Thursday in it. See https://en.wikipedia.org/wiki/ISO_week_date
//
#define startISOyear(year...)                                                  \
  ezt::makeOrdinalTime(0, 0, 0, FIRST, THURSDAY, JANUARY, year) -              \
      3UL * SECS_PER_DAY;
uint8_t
Timezone::weekISO(time_t t /*= TIME_NOW */,
                  const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  int16_t yr = year(t);
  time_t this_year = startISOyear(yr);
  time_t prev_year = startISOyear(yr - 1);
  time_t next_year = startISOyear(yr + 1);
  if (t < this_year)
    this_year = prev_year;
  if (t >= next_year)
    this_year = next_year;
  return (t - this_year) / (SECS_PER_DAY * 7UL) + 1;
}

uint16_t
Timezone::yearISO(time_t t /*= TIME_NOW */,
                  const ezLocalOrUTC_t local_or_utc /* = LOCAL_TIME */) {
  t = tzTime(t, local_or_utc);
  int16_t yr = year(t);
  time_t this_year = startISOyear(yr);
  time_t next_year = startISOyear(yr + 1);
  if (t < this_year)
    return yr - 1;
  if (t >= next_year)
    return yr + 1;
  return yr;
}

Timezone UTC;
