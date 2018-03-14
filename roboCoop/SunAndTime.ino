#include <stdint.h>
#include <stdio.h> // for function sprintf
#include "RoboCoopDefs.h"

//#define DEBUG_PRINT 1

SunRiseAndSetData sunData[24] = {
  { {1,  1 },  {8, 42, 0},  {17, 31, 0},  -4,  15 },
  { {1,  16},  {8, 37, 0},  {17, 48, 0},  -14, 21 },
  { {2,  1 },  {8, 22, 0},  {18, 10, 0},  -20, 20 },
  { {2,  16},  {8, 01, 0},  {18, 32, 0},  -20, 17 },
  { {3,  1 },  {7, 39, 0},  {18, 50, 0},  -26, 19 },
  { {3,  16},  {7, 11, 0},  {19, 10, 0},  -28, 20 },
  { {4,  1 },  {6, 41, 0},  {19, 31, 0},  -26, 18 },
  { {4,  16},  {6, 13, 0},  {19, 50, 0},  -22, 18 },
  { {5,  1 },  {5, 49, 0},  {20, 10, 0},  -18, 17 },
  { {5,  16},  {5, 30, 0},  {20, 28, 0},  -13, 15 },
  { {6,  1 },  {5, 17, 0},  {20, 44, 0},  -4,  9 },
  { {6,  16},  {5, 13, 0},  {20, 53, 0},  5,   2 },
  { {7,  1 },  {5, 18, 0},  {20, 54, 0},  11, -7 },
  { {7,  16},  {5, 30, 0},  {20, 47, 0},  16, -16 },
  { {8,  1 },  {5, 48, 0},  {20, 30, 0},  16, -21 },
  { {8,  16},  {6, 05, 0},  {20, 07, 0},  19, -26 },
  { {9,  1 },  {6, 25, 0},  {19, 39, 0},  17, -27 },
  { {9,  16},  {6, 43, 0},  {19, 11, 0},  17, -27 },
  { {10, 1 },  {7, 02, 0},  {18, 42, 0},  18, -26 },
  { {10, 16},  {7, 21, 0},  {18, 14, 0},  20, -24 },
  { {11, 1 },  {7, 43, 0},  {17, 49, 0},  19, -17 },
  { {11, 16},  {8, 04, 0},  {17, 31, 0},  17, -10 },
  { {12, 1 },  {8, 23, 0},  {17, 21, 0},  13, -1  },
  { {12, 16},  {8, 36, 0},  {17, 20, 0},  6,  9   },
};

extern Time sunriseExtraTime;
extern Time sunsetExtraTime;

struct SunRiseAndSetData getSunRiseAndSetData( struct Date currentDate )
{
  
#ifdef DEBUG_PRINT  
  Serial.print(F("*************Get Sunrise And Set Data******************\n"));
  Serial.print(F("  Current Date is: "));
  Serial.print(currentDate.month);
  Serial.print("  ");
  Serial.println(currentDate.date);
  Serial.flush();
#endif  
  
  
  int index = 2 * (currentDate.month - 1);
  float deltaDate;
  if( currentDate.date > 15 )
  {
    index++;
    deltaDate = 31 - currentDate.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }
  else
  {
    deltaDate = 15 - currentDate.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }
  
  SunRiseAndSetData currentSunData = sunData[index];

  
#ifdef DEBUG_PRINT  
  Serial.print(F("  Sun Data index is: "));
  Serial.println(index);
  Serial.print(F("  Current Sun Data is: "));
  Serial.print(currentSunData.startDate.month);
  Serial.print("  ");
  Serial.print(currentSunData.startDate.date);
  Serial.print(F("\n  Sunrise is: ")); 
  printTimeString(currentSunData.sunrise);
  Serial.print(F("\n  Sunset is: ")); 
  printTimeString(currentSunData.sunset);
  Serial.println();
  Serial.print(F("  Delta Date is: "));
  Serial.println(deltaDate);
  Serial.print(F("********************* End **********************\n"));
  Serial.flush();
#endif


  return currentSunData;  
}

struct Time getDoorOpenTime( struct Date currentDate )
{
  Time doorOpenTime = {0, 0, 0};
  SunRiseAndSetData currentSunData = getSunRiseAndSetData( currentDate );
  
#ifdef DEBUG_PRINT  
  Serial.print(F("************* Getting Door Open Time ***********\n"));  
  Serial.print(F("  Sunrise is: ")); 
  printTimeString(currentSunData.sunrise);
  Serial.print(F("  Sunrise Delta is: "));
  Serial.println(currentSunData.sunriseDelta);
  Serial.flush();
#endif  
  
  float deltaDate;
  if( currentDate.date > 15 )
  {
    deltaDate = 31 - currentDate.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }
  else
  {
    deltaDate = 15 - currentDate.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }

  Time dateDeltaTime = {0, 0, 0};
  if( currentSunData.sunriseDelta < 0 )
  { 
    float temp =  deltaDate * (float)(-1*currentSunData.sunriseDelta);
    dateDeltaTime.minute = (uint8_t)temp;
    doorOpenTime = subtractTime( currentSunData.sunrise, dateDeltaTime );
    
#ifdef DEBUG_PRINT  
    Serial.print(F("  DateDeltaTime is: ")); 
    printTimeString(dateDeltaTime);
    Serial.print(F("\n  Door Open Time 1 is: ")); 
    printTimeString(doorOpenTime);
    Serial.flush();
#endif

  }
  else
  {
    float temp =  deltaDate * (float)(currentSunData.sunriseDelta);
    dateDeltaTime.minute = (uint8_t)temp;
    doorOpenTime = addTime( currentSunData.sunrise, dateDeltaTime );
#ifdef DEBUG_PRINT  
    Serial.print(F("  DateDeltaTime is: ")); 
    printTimeString(dateDeltaTime);
    Serial.print(F("\n  Door Open Time 1 is: ")); 
    printTimeString(doorOpenTime);
    Serial.flush();
#endif    
  }
  doorOpenTime = addTime( doorOpenTime, sunriseExtraTime );

#ifdef DEBUG_PRINT  
  Serial.print(F("\n  Final Door Open Time is: ")); 
  printTimeString(doorOpenTime);
  Serial.println();
  Serial.print(F("********************* End **********************\n"));
  Serial.flush();
#endif

  return doorOpenTime;  
}

struct Time getDoorCloseTime( struct Date currentDate )
{
  Time doorCloseTime = {0, 0, 0};
  SunRiseAndSetData currentSunData = getSunRiseAndSetData( currentDate );
  
#ifdef DEBUG_PRINT  
  Serial.print(F("************* Getting Door Close Time ***********\n"));
  Serial.print(F("  Sunset is: ")); 
  printTimeString(currentSunData.sunset);
  Serial.print(F("  Sunset Delta is: "));
  Serial.println(currentSunData.sunsetDelta);
  Serial.flush();
#endif

  float deltaDate;
  if( currentDate.date > 15 )
  {
    deltaDate = 31 - currentDate.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }
  else
  {
    deltaDate = 15 - currentDate.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }

  Time dateDeltaTime = {0, 0, 0};
  if( currentSunData.sunsetDelta < 0 )
  {
    float temp =  deltaDate * (float)(-1*currentSunData.sunsetDelta);
    dateDeltaTime.minute = (int)temp;
    doorCloseTime = subtractTime( currentSunData.sunset, dateDeltaTime );
#ifdef DEBUG_PRINT  
    Serial.print(F("  DateDeltaTime is: ")); 
    printTimeString(dateDeltaTime);
    Serial.print(F("\n  Door Close Time 1 is: ")); 
    printTimeString(doorCloseTime);
    Serial.flush();
#endif
  }
  else
  {
    float temp =  deltaDate * (float)(currentSunData.sunsetDelta);
    dateDeltaTime.minute = (int)temp;
    doorCloseTime = addTime( currentSunData.sunset, dateDeltaTime );
#ifdef DEBUG_PRINT  
    Serial.print(F("  DateDeltaTime is: ")); 
    printTimeString(dateDeltaTime);
    Serial.print(F("\n  Door Close Time 1 is: ")); 
    printTimeString(doorCloseTime);
    Serial.flush();
#endif
  }
  doorCloseTime = addTime( doorCloseTime, sunsetExtraTime );

#ifdef DEBUG_PRINT  
  Serial.print(F("\n  Final Door Close Time is: ")); 
  printTimeString(doorCloseTime);
  Serial.println();  
  Serial.print(F("********************* End **********************\n"));  
  Serial.flush();
#endif

  return doorCloseTime;  
}

struct DateTime getNextDoorAlarm( struct DateTime currentDateTime )
{
  
#ifdef DEBUG_PRINT    
  Serial.println(F("************ Getting next sun alarm *************"));
  Serial.print(F("Current Time is: "));
  printTimeString(currentDateTime.time);
  Serial.println();
  Serial.flush();
#endif

  DateTime alarmTime = { {0,0,0}, {0, 0} };
  
  /*SunRiseAndSetData currentSunData = getSunRiseAndSetData( currentDateTime.date );
  
  if( isTimeLessThan( currentSunData.sunset, currentDateTime.time ) )
    alarmTime.time = addTime( currentSunData.sunrise, sunriseExtraTime );
  else if( isTimeLessThan ( currentDateTime.time, currentSunData.sunrise ) )
    alarmTime.time = addTime( currentSunData.sunrise, sunriseExtraTime );
  else
    alarmTime.time = addTime( currentSunData.sunset, sunsetExtraTime );
  alarmTime.date = currentDateTime.date;
  
  return alarmTime;*/

    
  Time doorOpenTime = getDoorOpenTime( currentDateTime.date );  
  
  if( isTimeLessThan( currentDateTime.time, doorOpenTime ) )
  {
    alarmTime.time = doorOpenTime;
    alarmTime.date = currentDateTime.date;

#ifdef DEBUG_PRINT  
    Serial.print(F("Door Open Alarm is: ")); 
    printTimeString(alarmTime.time);
    Serial.println();
    Serial.flush();
#endif

    return alarmTime;
  }
  
  Time doorCloseTime = getDoorCloseTime( currentDateTime.date );
  
  if( isTimeLessThan( currentDateTime.time, doorCloseTime) )
  {
    alarmTime.time = doorCloseTime;
    alarmTime.date = currentDateTime.date;
    
#ifdef DEBUG_PRINT  
    Serial.print(F("Door Close Alarm is: ")); 
    printTimeString(alarmTime.time);
    Serial.println();
    Serial.flush();
#endif

    return alarmTime;
  }
  else
  {
    //go into the next day (sunrise!)
    Date tomorrow = currentDateTime.date;
    tomorrow.date++;
    if( tomorrow.date > 31 )
    {
      tomorrow.date = 1;
      tomorrow.month++;
    }
    if( tomorrow.month > 12 )
      tomorrow.month = 1;
    
    doorOpenTime = getDoorOpenTime( tomorrow );  
    alarmTime.time = doorOpenTime;
    alarmTime.date = tomorrow;

#ifdef DEBUG_PRINT  
    Serial.print(F("Tomorrow Door Open Alarm is: ")); 
    printTimeString(alarmTime.time);
    Serial.println();
    Serial.flush();
#endif

   //alarmTime.time.hour = 21;
   //alarmTime.time.minute = 29;

    return alarmTime;
  }
        
  return alarmTime;
}

struct Time addTime( struct Time t1, struct Time t2 )
{
  Time result = { 0, 0, 0};
  result.seconds = (t2.seconds + t1.seconds);
  result.minute = (t2.minute + t1.minute);
  result.hour = (t2.hour + t1.hour);
  if( result.seconds >= 60 )
  {
    result.seconds -= 60;
    result.minute++;
  }
  if( result.minute >= 60 )
  {
    result.minute -= 60;
    result.hour++;
  }
  if( result.hour > 24 )
  {
    result.hour -= 24;
  }  
  return result;
}

struct Time subtractTime( struct Time t1, struct Time t2 )
{
  Time result = { 0, 0, 0};
  int wSeconds = t1.seconds - t2.seconds;
  int wMinute = t1.minute - t2.minute;
  int wHour = t1.hour - t2.hour;
    
  if( wSeconds < 0 )
  {
    wMinute--;
    wSeconds += 60;
  }
  if( wMinute < 0 )
  {
    wHour--;
    wMinute += 60;
  }
  if( wHour < 0 )
  {
    Serial.println(F("lower h"));
    wHour += 24;
  }
  result.seconds = wSeconds;
  result.minute = wMinute;
  result.hour = wHour;
  return result;
}



void printTimeString( struct Time temp )
{
  char timeString[8];  
  sprintf( timeString, "%02u:%02u:%02u", 
           temp.hour, 
           temp.minute, 
           temp.seconds );
  Serial.print(timeString);
}

bool isTimeLessThan( struct Time t1, struct Time t2 )
{
  if( t1.hour < t2.hour )
    return true;
  else if( t1.hour == t2.hour &&
           t1.minute < t2.minute )
    return true;
  else if( t1.hour == t2.hour &&
           t1.minute == t2.minute &&
           t1.seconds < t2.seconds )
    return true;
  
  return false;  
}
