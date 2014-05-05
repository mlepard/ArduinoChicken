#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <DS3231.h>
#include <avr/sleep.h>
#include <avr/power.h>

const int motorNumber = 1;     
const int doorOpenPin =  3;   
const int doorClosedPin = 5;
const int alarmPin = 2;

const int maxMotorTime = 2 * 1000; //max time for motor to operate in ms
const int openDoorMotorDirection = FORWARD;
const int closeDoorMotorDirection = BACKWARD;

const int motorSpeed = 25;

#define ALRM1_MATCH_EVERY_SEC  0b1111  // once a second
#define ALRM1_MATCH_SEC        0b1110  // when seconds match
#define ALRM1_MATCH_MIN_SEC    0b1100  // when minutes and seconds match
#define ALRM1_MATCH_HR_MIN_SEC 0b1000  // when hours, minutes, and seconds match
byte ALRM1_SET = ALRM1_MATCH_HR_MIN_SEC;

#define ALRM2_ONCE_PER_MIN     0b111   // once per minute (00 seconds of every minute)
#define ALRM2_MATCH_MIN        0b110   // when minutes match
#define ALRM2_MATCH_HR_MIN     0b100   // when hours and minutes match
byte ALRM2_SET = 0b000;

byte AlarmBits;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *doorMotor;
DS3231 Clock;

byte gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond;

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

bool isDoorOpen()
{
  return !digitalRead(doorOpenPin);;
}

bool isDoorClosed()
{
  return !digitalRead(doorClosedPin);;
}

void openDoor()
{
  if( isDoorOpen() )
  {
    Serial.println(F("Door is already open!"));    
    return; 
  }
  
  long startTime = millis();
  long currentTime = millis();
  long runningTime = currentTime - startTime;
    
  while( !isDoorOpen() &&
         runningTime < maxMotorTime )
  
  {
    doorMotor->run(openDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(100);
  }
  
  if( isDoorOpen() )
  {
    Serial.println(F("Door is now open."));
  }
  else
  {
    Serial.println(F("Motor timeout..."));
  }
  
  doorMotor->run(RELEASE);
  digitalWrite(13, HIGH);
}

void closeDoor()
{
  if( isDoorClosed() )
  {
    Serial.println(F("Door is already closed!"));
    return; 
  }
  
  long startTime = millis();
  long currentTime = millis();
  long runningTime = currentTime - startTime;
    
  while( !isDoorClosed() &&
         runningTime < maxMotorTime )
  
  {
    doorMotor->run(closeDoorMotorDirection);
    currentTime = millis();
    runningTime = currentTime - startTime;
    delay(100);
  }
  
  if( isDoorClosed() )
  {
    Serial.println(F("Door is now closed."));
  }
  else
  {
    Serial.println(F("Motor timeout..."));
  }
  
  
  doorMotor->run(RELEASE);
  
  digitalWrite(13, LOW);
}

//Interrupt service routine for external interrupt on INT0 pin conntected to /INT
void RTCAlarmTriggered()
{
  detachInterrupt(0);
}

void enterSleep(void)
{
    /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, RTCAlarmTriggered, LOW);
  delay(100);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  sleep_enable();
  
  byte spi_save = SPCR;
  SPCR = 0;
  
  //digitalWrite(13, LOW);
  power_adc_disable();
  power_spi_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();   
  
  sleep_mode();
  
  /* The program will continue from here. */
  
  /* First thing to do is disable sleep. */
  sleep_disable();
  
  power_all_enable();
 
  SPCR = spi_save; 
}

void wakeUp()
{
  Clock.turnOffAlarm(1);
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("############### Setup ##############"));
  
  pinMode(doorOpenPin, INPUT_PULLUP);      
  pinMode(doorClosedPin, INPUT_PULLUP);
  pinMode(alarmPin, INPUT_PULLUP);    

  pinMode(13, OUTPUT);  
    
  doorMotor = AFMS.getMotor(motorNumber);
  
  Wire.begin();
  AFMS.begin();
  
  doorMotor->setSpeed(motorSpeed);
  
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
  
  if( currentDateTime.time.hour <= doorOpenTime.hour  &&
      currentDateTime.time.minute < doorOpenTime.minute )
  {
    //Time is before the door open time...close the door
    Serial.println(F("Current Time is less than Door Open Time"));
    Serial.println(F("Door should be closed!"));
    closeDoor();
  }
  else if( currentDateTime.time.hour <= doorCloseTime.hour &&
           currentDateTime.time.minute < doorCloseTime.minute )
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
}

void loop(){
  
  Clock.getTime(gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond);
  DateTime currentTime = { {gHour, gMinute, gSecond}, {gMonth, gDate} };
  
  DateTime alarmTime = getNextDoorAlarm( currentTime );
  Clock.setA1Time(gDoW, alarmTime.time.hour, alarmTime.time.minute, alarmTime.time.seconds, AlarmBits, true, false, false);
  Clock.turnOnAlarm(1);
  //not sure why you have to do this... 
  Clock.checkIfAlarm(1);

  Serial.print(F("Go to sleep until "));
  printTimeString(alarmTime.time);
  Serial.println();

  enterSleep();
  wakeUp();

  Serial.print(F("Stop Sleeping, it's "));
  printTimeString(currentTime.time);
  Serial.println();  
  
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
}
