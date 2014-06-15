#include <stdint.h>
#include <stdio.h> // for function sprintf
#include "RoboCoopDefs.h"
#include <DS3231.h>
#include <Wire.h>

DS3231 Clock;

#define ALRM1_MATCH_EVERY_SEC  0b1111  // once a second
#define ALRM1_MATCH_SEC        0b1110  // when seconds match
#define ALRM1_MATCH_MIN_SEC    0b1100  // when minutes and seconds match
#define ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match
byte ALRM1_SET = ALRM1_MATCH_HR_MIN_SEC;

#define ALRM2_ONCE_PER_MIN     0b111   // once per minute (00 seconds of every minute)
#define ALRM2_MATCH_MIN        0b110   // when minutes match
#define ALRM2_MATCH_HR_MIN     0b100   // when hours and minutes match
byte ALRM2_SET = ALRM2_MATCH_HR_MIN;

byte AlarmBits;

extern byte gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond;

void setupAlarmControl(void)
{
  Wire.begin();
  
  Clock.getTime(gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond);
  DateTime currentDateTime = { {gHour, gMinute, gSecond}, {gMonth, gDate} };

  Serial.print(F("Current Date is: "));
  Serial.print(currentDateTime.date.month, DEC);
  Serial.print("  ");
  Serial.println(currentDateTime.date.date, DEC);

  Serial.print(F("Current Time is: "));
  printTimeString(currentDateTime.time);
  Serial.println();
   
  AlarmBits = ALRM2_SET;
  AlarmBits <<= 4;
  AlarmBits |= ALRM1_SET;
  
  Time doorOpenTime = getDoorOpenTime( currentDateTime.date );
  Time doorCloseTime = getDoorCloseTime( currentDateTime.date );
  Serial.print(F("Door Open Time is: "));
  printTimeString(doorOpenTime);
  Serial.println();
  Serial.print(F("Door Close Time is: "));
  printTimeString(doorCloseTime);
  Serial.println();
  
  if( isTimeLessThan( currentDateTime.time, doorOpenTime ) )
  {
    //Time is before the door open time...close the door
    Serial.println(F("Current Time is less than Door Open Time"));
    Serial.println(F("Door should be closed!"));
    closeDoor();
  }
  else if( isTimeLessThan( currentDateTime.time, doorCloseTime ) )
  {
    //Time is before the door close time...open the door
    Serial.print(F("Current Time is greater than Door Open Time,"));
    Serial.println(F(" but less than Door Close Time."));
    Serial.println(F("Door should be open!"));
    openDoor();
  }
  else
  {
    //Time is after the door close time...close the door
    Serial.println(F("Current Time is greater than Door Close Time."));
    Serial.println(F("Door should be closed!"));
    closeDoor();
  }
  Serial.println(F("############### End ##############"));
  Serial.flush();
  
  Clock.checkIfAlarm(1);
  Clock.checkIfAlarm(2);  
  
  Clock.turnOffAlarm(2);  
}

void setNextAlarm()
{
  Clock.getTime(gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond);
  DateTime currentTime = { {gHour, gMinute, gSecond}, {gMonth, gDate} };
    
  DateTime alarmTime = getNextDoorAlarm( currentTime ); 
  Clock.setA1Time(alarmTime.date.date, alarmTime.time.hour, alarmTime.time.minute, alarmTime.time.seconds, AlarmBits, false, false, false);
  Clock.turnOnAlarm(1);
  //not sure why you have to do this... 
  Clock.checkIfAlarm(1);

  Serial.print(F("Alarm is set for "));
  if( alarmTime.date.date != currentTime.date.date )
  {
    Serial.print(F("tomorrow "));
  }
  printTimeString(alarmTime.time);
  Serial.println();
}

void printCurrentTime()
{
  Clock.getTime(gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond);
  DateTime currentTime = { {gHour, gMinute, gSecond}, {gMonth, gDate} };
  
  currentTime.time.hour = gHour;
  currentTime.time.minute = gMinute;
  currentTime.time.seconds = gSecond;
  currentTime.date.month = gMonth;
  currentTime.date.date = gDate;

  Serial.print(F("Current time is "));
  printTimeString(currentTime.time);
  Serial.println();  
}

void turnOffAlarm()
{
    Clock.turnOffAlarm(1);
    Clock.checkIfAlarm(1);  
}
