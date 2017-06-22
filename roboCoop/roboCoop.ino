#include "RoboCoopDefs.h"

const int DS18S20_Pin = 12; //temperature DS18S20 Signal pin on digital 12
const int PowerSwitch_Pin = 9; //Powerswitch Output pin on digital 9
const int TempLED_Pin = 13; //Temperate LED pin on digital 10
const int chickenDoorOpenPin =  5;   
const int chickenDoorClosedPin = 7;
const int turkeyPotAnalogPin = 0;
const int chickenMotorNumber = 1;     
const int turkeyMotorNumber = 2;     
const int alarmPin = 2;
const int doorOverridePin = 3;

const boolean isWinter = true;

byte gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond;

boolean tempIsDoorOpen = false;
boolean inDebugMode = false;

Time sunriseExtraTime = { 0, 0, 0 };
Time sunsetExtraTime = { 0, 0, 0 };


void setup(void)
{
  Serial.begin(9600);
  Serial.println(F("Initializing RoboCoop..."));
  

  //Set all pins to output LOW for lowest power conversion.
  byte i;
  for (i = 0; i <= 13; i++)
    pinMode (i, OUTPUT);     // as required
  for (i = 0; i <= 13; i++)
    digitalWrite (i, LOW);  // as required

  if( isWinter )
  {
    setupTempControl();
    sunriseExtraTime.minute = 5;
    sunsetExtraTime.minute = 45;
  }
  else
  {
    sunriseExtraTime.minute = 25;
    sunsetExtraTime.minute = 25;    
  }
      
  setupDoorControl();
  setupAlarmControl();
  setupSleepControl();
  
  setNextAlarm();
    
  Serial.flush();
}

void loop(void) 
{
  WakeUpReason wakeReason = goToSleep();
  
  printCurrentTime();
  
  switch( wakeReason )
  {
    case WATCHDOG_TIMER_WAKEUP:    
      if( isWinter )
      {
        loopTempControl();
        delay(200);
      }
      break;
    case DOOR_OVERRIDE_WAKEUP:
      if( isLongDoorOverride() )
      {

        Serial.println(F("Override the turkey door!"));
        overrideDoorControl(TURKEY);
        tempIsDoorOpen = false;
      }
      else
      {
        Serial.println(F("Override the chicken door!"));
        overrideDoorControl(CHICKEN);
        tempIsDoorOpen = true;
      }
      break;    
    case ALARM_WAKEUP:
      //We've woken up at the next door event time.
      //Quick hack to determine which door event we should do..
      //If we woke up in the morning (before 12 noon) we must
      //want to open the door.
      if( gHour < 12 )
      {
         Serial.println(F("Open the Coop Doors!"));
         openDoors();
         tempIsDoorOpen = true;
      }
       else
       {
         Serial.println(F("Close the Coop Doors!"));
         closeDoors();
         tempIsDoorOpen = false;
       }  
       setNextAlarm();
       break;
  }
  if( inDebugMode )
  {
    if( tempIsDoorOpen )
      digitalWrite(13, HIGH);
    else
      digitalWrite(13, LOW);
  }
  
  Serial.flush();
}

bool isLongDoorOverride()
{
  Serial.println(F("Testing for long press.."));
  
  int lastSwitchState = LOW, currentSwitchState = LOW;
  long lastDebounceTime = millis();
  long startTime;
  bool isPressed = false;
  
  //first deboung the door override switch to get the start time for press.
  int count = 0;
  while( count < 500 )
  {
    currentSwitchState = !digitalRead(doorOverridePin);
    if( currentSwitchState == HIGH )
    {
      //The switch is now closed
      if( lastSwitchState == LOW )
      {
        //The switch was last open...therefore
        //(re)start the debounce timer running
        Serial.println(F("Door override switch was low and is now high, start debounce countdown."));
        lastDebounceTime = millis();
      }
        
      if ((millis() - lastDebounceTime) > 50) {
        Serial.println(F("Door override switch is debounced and high, get start time."));
        //If the reading is still high after this debounce period, the switch is permanently closed.
        startTime = millis();
        isPressed = true;
        break;
      }
    }
    delay(5);
    count++;
    lastSwitchState = currentSwitchState;      
  }
  
  //It's not pressed, must be a short (very) press.
  if( !isPressed )
    return false;
  
  Serial.println(F("Door override switch is still pressed down, being wait for it to open."));
  //Now wait until the switch is released...but don't wait TOO long.
  count = 0;
  while( count < 750 )
  {
    currentSwitchState = !digitalRead(doorOverridePin);
    if( currentSwitchState == LOW )
    {
      Serial.println(F("Door override switch is now open, how long did we wait?"));
      //The switch was held down for less than a second...it was a short press.
      if( (millis() - startTime) < 2000 )
      {
        Serial.println(F("less than a second, it was a short press."));
        return false;
      }
      else
      {
        Serial.println(F("longer than a second, it was a long press."));
        return true;
      }
    }
    delay(5);
    count++;
  }
  Serial.println(F("Switch is still down after 750*5ms, it is a long press."));
  //The switch is still held down...it's been a long time (500 * 5 ms)
  return true;
}


