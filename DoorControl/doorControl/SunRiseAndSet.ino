#include <stdint.h>
#include <stdio.h> // for function sprintf

//#define DEBUG_PRINT

struct SunRiseAndSetData {
  Date startDate;
  Time sunrise;
  Time sunset;
  int8_t sunriseDelta;
  int8_t sunsetDelta;
};

typedef struct SunRiseAndSetData SunRiseAndSetData;

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

Time sunriseExtraTime = { 0, 20, 0 };
Time sunsetExtraTime = { 0, 15, 0 };

struct SunRiseAndSetData getSunRiseAndSetData( struct Date currentDate )
{
  
#ifdef DEBUG_PRINT  
  Serial.print("*************Get Sunrise And Set Data******************\n");
  Serial.print("  Current Date is: ");
  Serial.print(currentDate.month, DEC);
  Serial.print("  ");
  Serial.println(currentDate.date, DEC);
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
  Serial.print("  Sun Data index is: ");
  Serial.println(index, DEC);
  Serial.print("  Current Sun Data is: ");
  Serial.print(currentSunData.startDate.month, DEC);
  Serial.print("  ");
  Serial.print(currentSunData.startDate.date, DEC);
  Serial.print("\n  Sunrise is: "); 
  printTimeString(currentSunData.sunrise);
  Serial.print("\n  Sunset is: "); 
  printTimeString(currentSunData.sunset);
  Serial.println();
  Serial.print("  Delta Date is: ");
  Serial.println(deltaDate, DEC);
  Serial.print("********************* End **********************\n");
#endif

  return currentSunData;  
}

struct Time getDoorOpenTime( struct Date currentDate )
{
  Time doorOpenTime = {0, 0, 0};
  SunRiseAndSetData currentSunData = getSunRiseAndSetData( currentDate );
  
#ifdef DEBUG_PRINT  
  Serial.print("************* Getting Door Open Time ***********\n");  
  Serial.print("  Sunrise is: "); 
  printTimeString(currentSunData.sunrise);
  Serial.print("  Sunrise Delta is: ");
  Serial.println(currentSunData.sunriseDelta, DEC);
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
    dateDeltaTime.minute = deltaDate * -1*currentSunData.sunriseDelta;
    doorOpenTime = subtractTime( currentSunData.sunrise, dateDeltaTime );
    
#ifdef DEBUG_PRINT  
    Serial.print("  DateDeltaTime is: "); 
    printTimeString(dateDeltaTime);
    Serial.print("\n  Door Open Time 1 is: "); 
    printTimeString(doorOpenTime);
#endif

  }
  else
  {
    dateDeltaTime.minute = deltaDate * currentSunData.sunriseDelta;
    doorOpenTime = addTime( currentSunData.sunrise, dateDeltaTime );
#ifdef DEBUG_PRINT  
    Serial.print("  DateDeltaTime is: "); 
    printTimeString(dateDeltaTime);
    Serial.print("\n  Door Open Time 1 is: "); 
    printTimeString(doorOpenTime);
#endif    
  }
  doorOpenTime = addTime( doorOpenTime, sunriseExtraTime );

#ifdef DEBUG_PRINT  
  Serial.print("\n  Final Door Open Time is: "); 
  printTimeString(doorOpenTime);
  Serial.println();
  Serial.print("********************* End **********************\n");
#endif

  return doorOpenTime;  
}

struct Time getDoorCloseTime( struct Date currentDate )
{
  Time doorCloseTime = {0, 0, 0};
  SunRiseAndSetData currentSunData = getSunRiseAndSetData( currentDate );
  
#ifdef DEBUG_PRINT  
  Serial.print("************* Getting Door Close Time ***********\n");
  Serial.print("  Sunset is: "); 
  printTimeString(currentSunData.sunset);
  Serial.print("  Sunset Delta is: ");
  Serial.println(currentSunData.sunsetDelta, DEC);
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
    dateDeltaTime.minute = deltaDate * -1*currentSunData.sunsetDelta;
    doorCloseTime = subtractTime( currentSunData.sunset, dateDeltaTime );
#ifdef DEBUG_PRINT  
    Serial.print("  DateDeltaTime is: "); 
    printTimeString(dateDeltaTime);
    Serial.print("\n  Door Close Time 1 is: "); 
    printTimeString(doorCloseTime);
#endif
  }
  else
  {
    dateDeltaTime.minute = deltaDate * currentSunData.sunsetDelta;
    doorCloseTime = addTime( currentSunData.sunset, dateDeltaTime );
#ifdef DEBUG_PRINT  
    Serial.print("  DateDeltaTime is: "); 
    printTimeString(dateDeltaTime);
    Serial.print("\n  Door Close Time 1 is: "); 
    printTimeString(doorCloseTime);
#endif
  }
  doorCloseTime = addTime( doorCloseTime, sunriseExtraTime );

#ifdef DEBUG_PRINT  
  Serial.print("\n  Final Door Close Time is: "); 
  printTimeString(doorCloseTime);
  Serial.println();  
  Serial.print("********************* End **********************\n");  
#endif

  return doorCloseTime;  
}

struct DateTime getNextDoorAlarm( struct DateTime currentDateTime )
{
  
#ifdef DEBUG_PRINT    
  Serial.println("************ Getting next sun alarm *************");
  Serial.print("Current Time is: ");
  printTimeString(currentDateTime.time);
  Serial.println();
#endif

  DateTime alarmTime = { {0,0,0}, {0, 0} };
  
  Time doorOpenTime = getDoorOpenTime( currentDateTime.date );  
  
  if( currentDateTime.time.hour < doorOpenTime.hour  &&
      currentDateTime.time.minute < doorOpenTime.minute )
  {
    alarmTime.time = doorOpenTime;
    alarmTime.date = currentDateTime.date;

#ifdef DEBUG_PRINT  
    Serial.print("Door Open Alarm is: "); 
    printTimeString(alarmTime.time);
    Serial.println();
#endif

    return alarmTime;
  }
  
  Time doorCloseTime = getDoorCloseTime( currentDateTime.date );
  
  if( currentDateTime.time.hour < doorCloseTime.hour &&
      currentDateTime.time.hour < doorCloseTime.minute )
  {
    alarmTime.time = doorCloseTime;
    alarmTime.date = currentDateTime.date;
    
#ifdef DEBUG_PRINT  
    Serial.print("Door Close Alarm is: "); 
    printTimeString(alarmTime.time);
    Serial.println();
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
    Serial.print("Tomorrow Door Open Alarm is: "); 
    printTimeString(alarmTime.time);
    Serial.println();
#endif

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
  if( result.seconds > 60 )
  {
    result.seconds -= 60;
    result.minute++;
  }
  if( result.minute > 60 )
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
  int8_t wSeconds = t1.seconds - t2.seconds;
  int8_t wMinute = t1.minute - t2.minute;
  int8_t wHour = t1.hour - t2.hour;
    
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
    Serial.println("lower h");
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
