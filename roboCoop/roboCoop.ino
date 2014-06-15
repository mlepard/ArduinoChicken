#include "RoboCoopDefs.h"

const int DS18S20_Pin = 8; //temperature DS18S20 Signal pin on digital 8
const int PowerSwitch_Pin = 9; //Powerswitch Output pin on digital 9
const int TempLED_Pin = 10; //Temperate LED pin on digital 10
const int doorOpenPin =  6;   
const int doorClosedPin = 7;
const int motorNumber = 1;     
const int alarmPin = 2;
const int doorOverridePin = 3;

const boolean isWinter = true;

byte gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond;

void setup(void) 
{
  Serial.begin(9600);
  Serial.println(F("Initializing RoboCoop..."));
  
  //if( isWinter )
  //  setupTempControl();
    
  setupDoorControl();
  setupAlarmControl();
  setupSleepControl();
  
  Serial.flush();
}

void loop(void) 
{
  WakeUpReason wakeReason = goToSleep();
  
  printCurrentTime();
  
  //if( isWinter )
  //  loopTempControl();
    
  if( wakeReason == DOOR_OVERRIDE_WAKEUP )
  {
    if( isDoorOpen() )
    {
      Serial.println(F("close the door!"));
      closeDoor();
    }
    else
    {
      Serial.println(F("open the door!"));
      openDoor();
    }
    return;    
  }
  if( wakeReason == ALARM_WAKEUP )
  {
    //We've woken up at the next door event time.
    //Quick hack to determine which door event we should do..
    //If we woke up in the morning (before 12 noon) we must
    //want to open the door.
    if( gHour < 12 )
    {
       Serial.println(F("Open the Coop Door!"));
       openDoor();
    }
     else
     {
       Serial.println(F("Close the Coop Door!"));
       closeDoor();
     }  
     setNextAlarm();
  }
  Serial.flush();
}


