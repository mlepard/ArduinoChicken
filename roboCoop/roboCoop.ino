#include "RoboCoopDefs.h"

const int DS18S20_Pin = 12; //temperature DS18S20 Signal pin on digital 8
const int PowerSwitch_Pin = 9; //Powerswitch Output pin on digital 9
const int TempLED_Pin = 13; //Temperate LED pin on digital 10
const int doorOpenPin =  5;   
const int doorClosedPin = 7;
const int motorNumber = 1;     
const int alarmPin = 2;
const int doorOverridePin = 3;

const boolean isWinter = true;

byte gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond;

boolean tempIsDoorOpen = false;

void setup(void)
{
  Serial.begin(9600);
  Serial.println(F("Initializing RoboCoop..."));
  
  if( isWinter )
    setupTempControl();
  else
  {
    //Set all pins to output LOW for lowest power conversion.
    byte i;
    for (i = 0; i <= 13; i++)
      pinMode (i, OUTPUT);     // as required
    for (i = 0; i <= 13; i++)
      digitalWrite (i, LOW);  // as required
  
    pinMode( DS18S20_Pin, OUTPUT );
    digitalWrite( DS18S20_Pin, HIGH );
  }
    
  setupDoorControl();
  setupAlarmControl();
  setupSleepControl();
  
  setNextAlarm();
  
  if( isDoorOpen )
    tempIsDoorOpen = true;
  
  digitalWrite(13, tempIsDoorOpen);
  
  Serial.flush();
}

void loop(void) 
{
  WakeUpReason wakeReason = goToSleep();
  
  printCurrentTime();
  
  if( isWinter )
  {
    loopTempControl();
    if( tempIsDoorOpen )
      digitalWrite(13, HIGH);
    else
      digitalWrite(13, LOW);
  }
    
  if( wakeReason == DOOR_OVERRIDE_WAKEUP )
  {
    if( isDoorOpen() )
    {
      Serial.println(F("close the door!"));
      closeDoor();
      tempIsDoorOpen = false;
    }
    else
    {
      Serial.println(F("open the door!"));
      openDoor();
      tempIsDoorOpen = true;
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
       tempIsDoorOpen = true;
    }
     else
     {
       Serial.println(F("Close the Coop Door!"));
       closeDoor();
       tempIsDoorOpen = false;
     }  
     setNextAlarm();
  }
  Serial.flush();
}


