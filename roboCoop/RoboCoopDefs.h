#ifndef RoboCoop_Defs
#define RoboCoop_Defs

#include <Arduino.h>

enum WakeUpReason {
  ALARM_WAKEUP = 0,
  DOOR_OVERRIDE_WAKEUP,
  WATCHDOG_TIMER_WAKEUP,
  UNKNOWN_WAKEUP
};

struct Time {
  byte hour;
  byte minute;
  byte seconds;
};

typedef struct Time Time;

struct Date {
  byte month;
  byte date;
};

typedef struct Date Date;

struct DateTime {
  Time time;
  Date date;
};

typedef struct DateTime DateTime;

struct SunRiseAndSetData {
  Date startDate;
  Time sunrise;
  Time sunset;
  int8_t sunriseDelta;
  int8_t sunsetDelta;
};

typedef struct SunRiseAndSetData SunRiseAndSetData;

#endif
