#ifndef RoboCoop_Defs
#define RoboCoop_Defs

#include <Arduino.h>

enum WakeUpReason {
  ALARM_WAKEUP = 0,
  DOOR_OVERRIDE_WAKEUP,
  WATCHDOG_TIMER_WAKEUP,
  UNKNOWN_WAKEUP
};

enum CoopDoorType {
  CHICKEN,
  TURKEY
};

struct Time {
  int hour;
  int minute;
  int seconds;
};

typedef struct Time Time;

struct Date {
  int month;
  int date;
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
  int sunriseDelta;
  int sunsetDelta;
};

typedef struct SunRiseAndSetData SunRiseAndSetData;

#endif
