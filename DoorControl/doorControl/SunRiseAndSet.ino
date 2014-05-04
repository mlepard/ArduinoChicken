#include <stdint.h>
#include <stdio.h> // for function sprintf

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

struct DateTime getNextSunAlarm( struct DateTime currentDateTime )
{
  Serial.println("Getting next sun alarm");
  Serial.print("Current Time is: ");
  printTimeString(currentDateTime.time);
  Serial.print("\nCurrent Date is: ");
  Serial.print(currentDateTime.date.month, DEC);
  Serial.print("  ");
  Serial.println(currentDateTime.date.date, DEC);
  
  DateTime alarmTime = { {0,0,0}, {0, 0} };
  
  int index = 2 * (currentDateTime.date.month - 1);
  float deltaDate;
  if( currentDateTime.date.date > 15 )
  {
    index++;
    deltaDate = 31 - currentDateTime.date.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }
  else
  {
    deltaDate = 15 - currentDateTime.date.date;
    deltaDate = 1.0 - deltaDate / 15.0;
  }
  Serial.print("Sun Data index is: ");
  Serial.println(index, DEC);
  SunRiseAndSetData currentSunData = sunData[index];
  Serial.print("Current Sun Data is: ");
  Serial.print(currentSunData.startDate.month, DEC);
  Serial.print("  ");
  Serial.print(currentSunData.startDate.date, DEC);
  Serial.print("\nSunrise is: "); 
  printTimeString(currentSunData.sunrise);
  Serial.print("\nSunset is: "); 
  printTimeString(currentSunData.sunset);
  Serial.println();
  Serial.print("Delta Date is: ");
  Serial.println(deltaDate, DEC);
  
  Serial.print("For Sunrise:\n");
  Serial.print("Sunrise Delta is: ");
  Serial.println(currentSunData.sunriseDelta, DEC);
  
  Time dateDeltaTime = {0, 0, 0};
  Time adjustedSunTime = {0, 0, 0};
  if( currentSunData.sunriseDelta < 0 )
  { 
    dateDeltaTime.minute = deltaDate * -1*currentSunData.sunriseDelta;
    adjustedSunTime = subtractTime( currentSunData.sunrise, dateDeltaTime );
    Serial.print("AdjusedSunTime 1 is: "); 
    printTimeString(adjustedSunTime);
  }
  else
  {
    dateDeltaTime.minute = deltaDate * currentSunData.sunriseDelta;
    adjustedSunTime = addTime( currentSunData.sunrise, dateDeltaTime );
    Serial.print("AdjusedSunTime 1 is: "); 
    printTimeString(adjustedSunTime);
  }
  adjustedSunTime = addTime( adjustedSunTime, sunriseExtraTime );
  Serial.print("\nDateDeltaTime is: "); 
  printTimeString(dateDeltaTime);
  Serial.print("\nAdjusedSunTime 2 is: "); 
  printTimeString(adjustedSunTime);
  Serial.println();
  
  if( currentDateTime.time.hour < adjustedSunTime.hour  &&
      currentDateTime.time.minute < adjustedSunTime.minute )
  {
    alarmTime.time = adjustedSunTime;
    alarmTime.date = currentDateTime.date;
    Serial.print("Sunrise Alarm is: "); 
    printTimeString(alarmTime.time);
    Serial.println();
    return alarmTime;
  }
  
  Serial.print("For Sunset:\n");
  Serial.print("Sunset Delta is: ");
  Serial.println(currentSunData.sunsetDelta, DEC);
  if( currentSunData.sunsetDelta < 0 )
  { 
    dateDeltaTime.minute = deltaDate * -1*currentSunData.sunsetDelta;
    adjustedSunTime = subtractTime( currentSunData.sunset, dateDeltaTime );
    Serial.print("AdjusedSunTime 1 is: "); 
    printTimeString(adjustedSunTime);
  }
  else
  {
    dateDeltaTime.minute = deltaDate * currentSunData.sunsetDelta;
    adjustedSunTime = addTime( currentSunData.sunset, dateDeltaTime );
    Serial.print("AdjusedSunTime 1 is: "); 
    printTimeString(adjustedSunTime);
  }
  adjustedSunTime = addTime( adjustedSunTime, sunsetExtraTime );
  Serial.print("\nDateDeltaTime is: "); 
  printTimeString(dateDeltaTime);
  Serial.print("\nAdjusedSunTime 2 is: "); 
  printTimeString(adjustedSunTime);
  Serial.println();

  if( currentDateTime.time.hour < adjustedSunTime.hour &&
      currentDateTime.time.hour < adjustedSunTime.minute )
  {
    alarmTime.time = adjustedSunTime;
    alarmTime.date = currentDateTime.date;
    Serial.print("Sunset Alarm is: "); 
    printTimeString(alarmTime.time);
    Serial.println();
    return alarmTime;
  }
  else
  {
    //go into the next day (sunrise!)
    index++;
    if( index == 24 )
     index = 0;
    currentSunData = sunData[index];
    alarmTime.time = addTime( currentSunData.sunrise, sunriseExtraTime );
    Serial.print("Next Day Sunrise Alarm is: "); 
    printTimeString(alarmTime.time);
    Serial.println();
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
