#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <DS3231.h>
#include <avr/sleep.h>
#include <avr/power.h>

const int motorNumber = 1;     
const int forwardPin =  3;   
const int reversePin = 5;
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

bool tempDoorState = 0;

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
  return false;
}

bool isDoorClosed()
{
  return false;
}

void openDoor()
{
  if( isDoorOpen() )
  {
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
  }
  
  doorMotor->run(RELEASE);  
}

void closeDoor()
{
  if( isDoorClosed() )
  {
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
  }
  
  doorMotor->run(RELEASE);
}

//Interrupt service routine for external interrupt on INT0 pin conntected to /INT
void RTCAlarmTriggered()
{
  detachInterrupt(0);
}

void enterSleep(void)
{
  Serial.println("sleep!");
  
  
  /* Setup pin2 as an interrupt and attach handler. */
  attachInterrupt(0, RTCAlarmTriggered, LOW);
  delay(100);
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  sleep_enable();
  
  byte spi_save = SPCR;
  SPCR = 0;
  
  digitalWrite(13, LOW);
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
  Clock.getTime(gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond);  
  
  Serial.print("Wake Up! Current Time is: ");
  Serial.print(gHour, DEC);
  Serial.print(':');
  Serial.print(gMinute, DEC);
  Serial.print(':');
  Serial.print(gSecond, DEC);
  Serial.println();  
  
  Serial.flush();
}

void setup() {
  Serial.begin(9600);
  
  pinMode(forwardPin, INPUT_PULLUP);      
  pinMode(reversePin, INPUT_PULLUP);
  pinMode(alarmPin, INPUT_PULLUP);           
    
  doorMotor = AFMS.getMotor(motorNumber);
  
  Wire.begin();
  AFMS.begin();
  
  doorMotor->setSpeed(motorSpeed);
  
  Serial.print("Setup! Current Time is: ");

  Clock.getTime(gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond);    
  
  Serial.print(gHour, DEC);
  Serial.print(':');
  Serial.print(gMinute, DEC);
  Serial.print(':');
  Serial.print(gSecond, DEC);
  Serial.println();
  
  AlarmBits = ALRM2_SET;
  AlarmBits <<= 4;
  AlarmBits |= ALRM1_SET;
}

void loop(){
  /*int forwardState = digitalRead(forwardPin);
  int reverseState = digitalRead(reversePin);
  
  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
  if (forwardState == LOW) {     
    Serial.println("openDoor");
   openDoor();
  } 
  else if (reverseState == LOW) {
    Serial.println("closeDoor");
   closeDoor();
  }*/  
  
  Clock.getTime(gYear, gMonth, gDate, gDoW, gHour, gMinute, gSecond);
  DateTime currentTime = { {gHour, gMinute, gSecond}, {gMonth, gDate} };
  DateTime alarmTime = getNextSunAlarm( currentTime );
  Clock.setA1Time(gDoW, alarmTime.time.hour, alarmTime.time.minute, alarmTime.time.seconds, AlarmBits, true, false, false);
  Clock.turnOnAlarm(1);
  //not sure why you have to do this... 
  Clock.checkIfAlarm(1);

  enterSleep();
  wakeUp();   
}
